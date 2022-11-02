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
File Name : i2c0.h
Author(s) : Nic McDonald
Project : Quadrotor
Hardware : LPCXpresso LPC1768
Purpose : I2C Driver

******************************************************************************
Modification History:
Revision Date Author Description of Revision
1.00 03/04/2011 NGM initial

******************************************************************************
Warning:
This I2C implementation is only for master mode. It also only
gives one transfer per transaction. This means that this driver
only does 'send' or 'receive' per function call. The user
functions 'receive' and 'send' are NOT thread safe.

*****************************************************************************/
#ifndef _I2C0_H_
#define _I2C0_H_

/* includes */
//#include &lt;stdlib.h&gt;
//#include &lt;stdint.h&gt;
#include "LPC17xx.h"

/* defines */
#define MODE_100kbps 100000
#define MODE_400kbps 400000
#define MODE_1Mbps 1000000

/* typedefs */

/* functions */

// Initialize the I2C hardware.
// see 'readme'
void i2c0_init(uint32_t i2c_freq, uint8_t int_pri);

// Performs a I2C master send function.
// Returns the number of bytes sent successfully.
// Returns 0xFFFFFFFF if slave did not response on bus.
// This is NOT thread safe.
uint32_t i2c0_send(uint8_t address, uint8_t* buffer, uint32_t length);

// Performs a I2C master receive function.
// Returns the number of bytes received successfully.
// Returns 0xFFFFFFFF if slave did not response on bus.
// This is NOT thread safe.
uint32_t i2c0_receive(uint8_t address, uint8_t* buffer, uint32_t length);

/*** DEBUG ***/uint8_t* i2c_buf(void);
/*** DEBUG ***/uint32_t i2c_pos(void);

#endif /* _I2C0_H_ */
