/*****************************************************************************
Copyright (c) 2011, Nic McDonald
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************
                                Copyright 2011
                             All Rights Reserved

 Information:
   File Name  :  i2c0.c
   Author(s)  :  Nic McDonald
   Project    :  Quadrotor
   Hardware   :  LPCXpresso LPC1768
   Purpose    :  I2C Driver

******************************************************************************
 Modification History:
   Revision   Date         Author    Description of Revision
   1.00       03/04/2011   NGM       initial

*****************************************************************************/
#include "i2c.h"
#include <stdlib.h>

// IC2 control bits
#define AA      (1 << 2)
#define SI      (1 << 3)
#define STO     (1 << 4)
#define STA     (1 << 5)
#define I2EN    (1 << 6)

// pointers setup by users functions
static volatile uint8_t  slave_address; // formatted by send or receive
static volatile uint8_t* buf;
static volatile uint32_t buf_len;
static volatile uint32_t num_transferred;
static volatile uint32_t i2c0_busy;

static inline uint8_t to_read_address(uint8_t address);
static inline uint8_t to_write_address(uint8_t address);

/*************DEBUG**************************************************************************************/
uint8_t i2c_status_buf[100];
uint32_t i2c_status_pos;
uint8_t* i2c_buf(void) {return i2c_status_buf;}
uint32_t i2c_pos(void) {return i2c_status_pos;}
/*************DEBUG**************************************************************************************/

LPC_I2C_TypeDef*  regs;
IRQn_Type         irqn;
uint32_t ignore_data_nack = 1;


void i2c0_init(uint32_t i2c_freq, uint8_t int_pri) {
    uint32_t pclk, fdiv;

    regs = LPC_I2C0;
    irqn = I2C0_IRQn;

    // setup initial state
    i2c0_busy = 0;
    buf = NULL;
    buf_len = 0;
    num_transferred = 0;

    // give power to the I2C hardware
    LPC_SC->PCONP |= (1 << 7);

    // set PIO0.27 and PIO0.28 to I2C0 SDA and SCK
    LPC_PINCON->PINSEL1 &= ~0x03C00000;
    LPC_PINCON->PINSEL1 |=  0x01400000;

    // set peripheral clock selection for I2C0
    LPC_SC->PCLKSEL0 &= ~(3 << 14); // clear bits
    LPC_SC->PCLKSEL0 |=  (1 << 14); // set to "01" (full speed)
    pclk = SystemCoreClock;

    // clear all flags
    regs->I2CONCLR = AA | SI | STO | STA | I2EN;

    // determine the frequency divider and set corresponding registers
    //  this makes a 50% duty cycle
    fdiv = pclk / i2c_freq;
    regs->I2SCLL = fdiv >> 1; // fdiv / 2
    regs->I2SCLH = fdiv - (fdiv >> 1); // compensate for odd dividers

    // install interrupt handler
    NVIC_EnableIRQ(irqn);

    // set the priority of the interrupt
    NVIC_SetPriority(irqn, int_pri); // '0' is highest

    // enable the I2C (master only)
    regs->I2CONSET = I2EN;
}

uint32_t i2c0_send(uint8_t address, uint8_t* buffer, uint32_t length) {
    // check software FSM
    if (i2c0_busy)
        //error_led_trap(0x11000001, i2c0_busy, 0, 0, 0, 0, 0, 0, 0);
        return 0;

    // set to status to 'busy'
    i2c0_busy = 1;

    // setup pointers
    slave_address = to_write_address(address);
    buf = buffer;
    buf_len = length;
    num_transferred = 0;

    // trigger a start condition
    regs->I2CONSET = STA;

    // wait for completion
    while (i2c0_busy);

    // get how many bytes were transferred
    return num_transferred;
}

uint32_t i2c0_receive(uint8_t address, uint8_t* buffer, uint32_t length) {
    // check software FSM
    if (i2c0_busy)
        //error_led_trap(0x11000002, i2c0_busy, 0, 0, 0, 0, 0, 0, 0);
        return 0;

    // set to status to 'busy'
    i2c0_busy = 1;

    // setup pointers
    slave_address = to_read_address(address);
    buf = buffer;
    buf_len = length;
    num_transferred = 0;

    // trigger a start condition
    regs->I2CONSET = STA;

    // wait for completion
    while (i2c0_busy);

    // get how many bytes were transferred
    return num_transferred;
}

void I2C0_IRQHandler(void) {
    // get reason for interrupt
    uint8_t status = regs->I2STAT;

    // ignore data nack when control is true
    if ((status == 0x30) && (ignore_data_nack))
            status = 0x28;

    // LPC17xx User Manual page 443:
    //      "...read and write to [I2DAT] only while ... the SI bit is set"
    //      "Data in I2DAT remains stable as long as the SI bit is set."


    /**************************************DEBUG************************************************************/
    i2c_status_buf[i2c_status_pos] = status;
    i2c_status_pos++;
    if (i2c_status_pos > 99)
        i2c_status_pos = 0;
    /**************************************DEBUG************************************************************/


    switch(status) {

    // Int: start condition has been transmitted
    // Do:  send SLA+R or SLA+W
    case 0x08:
        regs->I2DAT = slave_address; // formatted by send or receive
        regs->I2CONCLR = STA | SI;
        break;

    // Int: repeated start condition has been transmitted
    // Do:  send SLA+R or SLA+W
    //case 0x10:
    //    regs->I2DAT = slave_address;
    //    regs->I2CONCLR = STA | SI;
    //    break;

    // Int: SLA+W has been transmitted, ACK received
    // Do:  send first byte of buffer if available
    case 0x18:
        if (num_transferred < buf_len) {
            regs->I2DAT = buf[0];
            regs->I2CONCLR = STO | STA | SI;
        }
        else {
            regs->I2CONCLR = STA | SI;
            regs->I2CONSET = STO;
        }
        break;

    // Int: SLA+W has been transmitted, NACK received
    // Do:  stop!
    case 0x20:
        regs->I2CONCLR = STA | SI;
        regs->I2CONSET = STO;
        num_transferred = 0xFFFFFFFF;
        i2c0_busy = 0;
        break;

    // Int: data byte has been transmitted, ACK received
    // Do:  load next byte if available, else stop
    case 0x28:
        num_transferred++;
        if (num_transferred < buf_len) {
            regs->I2DAT = buf[num_transferred];
            regs->I2CONCLR = STO | STA | SI;
        }
        else {
            regs->I2CONCLR = STA | SI;
            regs->I2CONSET = STO;
            i2c0_busy = 0;
        }
        break;

    // Int: data byte has been transmitted, NACK received
    // Do:  stop!
    case 0x30:
        regs->I2CONCLR = STA | SI;
        regs->I2CONSET = STO;
        i2c0_busy = 0;
        break;

    // Int: arbitration lost in SLA+R/W or Data bytes
    // Do:  release bus
    case 0x38:
        regs->I2CONCLR = STO | STA | SI;
        i2c0_busy = 0;
        break;

    // Int: SLA+R has been transmitted, ACK received
    // Do:  determine if byte is to be received
    case 0x40:
        if (num_transferred < buf_len) {
            regs->I2CONCLR = STO | STA | SI;
            regs->I2CONSET = AA;
        }
        else {
            regs->I2CONCLR = AA | STO | STA | SI;
        }
        break;

    // Int: SLA+R has been transmitted, NACK received
    // Do:  stop!
    case 0x48:
        regs->I2CONCLR = STA | SI;
        regs->I2CONSET = STO;
        num_transferred = 0xFFFFFFFF;
        i2c0_busy = 0;
        break;

    // Int: data byte has been received, ACK has been returned
    // Do:  read byte, determine if another byte is needed
    case 0x50:
        buf[num_transferred] = regs->I2DAT;
        num_transferred++;
        if (num_transferred < buf_len) {
            regs->I2CONCLR = STO | STA | SI;
            regs->I2CONSET = AA;
        }
        else {
            regs->I2CONCLR = AA | STO | STA | SI;
        }
        break;

    // Int: data byte has been received, NACK has been returned
    // Do:  transfer is done, stop.
    case 0x58:
        regs->I2CONCLR = STA | SI;
        regs->I2CONSET = STO;
        i2c0_busy = 0;
        break;

    // something went wrong, trap error
    default:
        while (1); // flash a LED or something <img draggable="false" role="img" class="emoji" alt="" src="https://s0.wp.com/wp-content/mu-plugins/wpcom-smileys/twemoji/2/svg/1f626.svg">
        break;

    }
}

static inline uint8_t to_read_address(uint8_t address) {
    return (address << 1) | 0x01;
}
static inline uint8_t to_write_address(uint8_t address) {
    return (address << 1);
}
