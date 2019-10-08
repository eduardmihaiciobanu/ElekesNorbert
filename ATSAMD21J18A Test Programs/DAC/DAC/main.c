/*
 * DAC.c
 *
 * Created: 07/10/2019 21:33:29
 * Author : Elekes Norbert
 *
 * Description: Test for DAC. An analog DC value will be generated on PA02, which will be read by a multimeter. The generated DC value will be changed 
 *              when the button on PA15 is pushed.
 */ 

#include "sam.h"

#define PORT_WRCONFIG_PMUX_B   0x1  //port multiplexer option B
#define GCLK_GENDIV_ID_GCLK1  0x01  //generic clock 1 id for GENDIV
#define GCLK_GENDIV_ID_GCLK2  0x02  //generic clock 2 id for GENDIV
#define GCLK_GENCTRL_ID_GCLK1 0x01  //generic clock 1 id for GENCTRL
#define GCLK_GENCTRL_ID_GCLK2 0x02  //generic clock 2 id for GENCTRL

volatile uint8_t state = 0x01;

void EIC_Handler();

int main(void)
{
  SystemInit();

  //configure port, PA02
  REG_PORT_DIRSET0 = PORT_PA02;                                                                                    //set PA02 to output
  REG_PORT_WRCONFIG0 = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUX(PORT_WRCONFIG_PMUX_B) | 
                       PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PINMASK(PORT_PA02);                                    //configure port; choose port function B
  
  //set up generic clock 1 for the DAC
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(0x0001) | GCLK_GENDIV_ID(GCLK_GENDIV_ID_GCLK1);                         //set clock divider for GCLK1
  REG_GCLK_GENCTRL = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_GCLK1);  //enable GCLK1 and set it's source
  while (GCLK->STATUS.bit.SYNCBUSY);  //write synchronization
  
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_DAC;  //enable GCLK1 and assign it to DAC

  //set up DAC
  REG_PM_APBCMASK |= PM_APBCMASK_DAC;  //enable clock for DAC in PM
  
  REG_DAC_CTRLB |= DAC_CTRLB_REFSEL_AVCC | DAC_CTRLB_EOEN;  //set reference to analog VDD
  REG_DAC_DATA = DAC_DATA_DATA(0);                          //set digital value which will be converted to analog
  while (DAC->STATUS.bit.SYNCBUSY);

  REG_DAC_CTRLA |= DAC_CTRLA_ENABLE;  //enable DAC
  while (DAC->STATUS.bit.SYNCBUSY);

  //set up generic clock 2 for the button
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(0x0002) | GCLK_GENDIV_ID(GCLK_GENDIV_ID_GCLK2);                         //set prescaler
  REG_GCLK_GENCTRL = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_GCLK2);  //set clock source for GCLK2
  while (GCLK->STATUS.bit.SYNCBUSY);
  
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_EIC;  //enable GCLK2 and assign it to EIC
  
  //set up PA15
  REG_PORT_WRCONFIG0 = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PULLEN | PORT_WRCONFIG_INEN | 
                       PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PINMASK(PORT_PA15);              //set multiplexer option A and enable pull resistor  
  REG_PORT_OUTSET0 = PORT_PA15;                                                              //set pull resistor to pull-up                       

  //set EIC
  REG_EIC_INTENSET = EIC_INTENSET_EXTINT15;  //enable external interrupt on PA15
  REG_EIC_CONFIG1 |= EIC_CONFIG_FILTEN7 | EIC_CONFIG_SENSE7_FALL;  //enable filter and set interrupt sense to falling edge
  
  REG_EIC_CTRL |= EIC_CTRL_ENABLE;  //enable EIC
  while (EIC->STATUS.bit.SYNCBUSY);
  
  NVIC_EnableIRQ(EIC_IRQn);  //enable interrupt

  while (1) {}
}

void EIC_Handler()  //ISR for EIC
{
  REG_DAC_DATA = DAC_DATA_DATA((uint16_t)(state) * 256 - 1); 
  
   if (state < 3)
     state++;
   else
     state = 0;
  
  REG_EIC_INTFLAG |= EIC_INTFLAG_EXTINT15;  //reset flag
}