#include "LPC17xx.h"

#include "lpc17xx_dac.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_i2c.h"
#include <cr_section_macros.h>

#include "Const.h"

#ifndef CONFIG_H_
#define CONFIG_H_

void cfgPines();
void cfgDAC();
void cfgDMA(uint32_t* actualSig);
void cfgNVIC();
void cfgADC();
void cfgTIM0();
void cfgTIM1();
void cfgI2C();
void makeSignals(uint32_t signals[][TRANSFERSIZE],uint32_t actualSig[TRANSFERSIZE]);


#endif /* CONFIG_H_ */
