/*
 * ReadButtonWithInterrupt.c
 *
 * Created: 05/10/2019 12:59:05
 * Author : Elekes Norbert
 *
 * Description: Test for reading the state of the button connected to PA15. The button is activated when PA15
 *              is pulled to ground. When the button is activated, the LED which is connected to PB30 will be
 *              toggled. The LED turns on when PB30 is pulled to ground.
 */ 

#include "sam.h"

#define GCLK_GENCTRL_ID_GCLKGEN1 0x01  //generic clock generator 1 id
#define GCLK_GENDIV_ID_GCLKGEN1  0x01  //generic clock generator 1 id

void EIC_Handler();

int main(void)
{
  /* Initialize the SAM system */
  SystemInit();

  //set up the LED
  REG_PORT_DIRSET1 = PORT_PB30;  //set PB30 to output
  REG_PORT_OUTSET1 = PORT_PB30;  //pull PB30 to VCC -> LED off

  //set up button
  REG_PORT_WRCONFIG0 = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PULLEN |  PORT_WRCONFIG_PMUXEN |
                       PORT_WRCONFIG_INEN | PORT_WRCONFIG_PINMASK(PORT_PA15); //write configuration reg for port PA15 (enable input and pull resistor)
  REG_PORT_OUTSET0 = PORT_PA15;  //set pull resistor to pull-up

  //set up the generic clock generator, which is required for the external interrupt controller
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(0x0002) | GCLK_GENDIV_ID(GCLK_GENDIV_ID_GCLKGEN1);  //set clock divider to 2
  REG_GCLK_GENCTRL = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M |
                     GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_GCLKGEN1);  //enable generic clock 1 and set its source to the 8MHz internal oscillator
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_EIC;  //assign generic clock 1 to the external interrupt controller
  
  REG_EIC_CONFIG1 |= EIC_CONFIG_FILTEN7 | EIC_CONFIG_SENSE7_FALL;  //set interrupt to falling edge (1 = second part(8 sense/part): 8 + 7 = 15)
  REG_EIC_INTENSET = EIC_INTENSET_EXTINT15;  //enable interrupt for PA15
  
  REG_EIC_CTRL = EIC_CTRL_ENABLE;  //enable the external interrupt controller
  while (EIC->STATUS.bit.SYNCBUSY);  //wait until the external interrupt controller is activated
  
  NVIC_EnableIRQ(EIC_IRQn);  //enable interrupt

  while (1) {}
}

void EIC_Handler()  //interrupt service routine for the external interrupt controller
{
  REG_PORT_OUTTGL1 = PORT_PB30;  //toggle PB30 -> switch LED status
  REG_EIC_INTFLAG = EIC_INTFLAG_EXTINT15;
}