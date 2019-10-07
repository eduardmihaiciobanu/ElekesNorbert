/*
 * PWM.c
 *
 * Created: 06/10/2019 08:52:49
 * Author : Elekes Norbert
 *
 * Description: Test for PWM generation. The controller will change the duty cycle of the PWM signal, connected to PB30,
 *              which also is connected to an LED. When the button connected to PA15 is pushed, the duty cycle of the
 *              PWM signal will be changed. The PWM signal will be generated using the timer/counter TCC0.
 */ 

#include "sam.h"

#define GCLK_GENDIV_ID_GCLKGEN1    0x01     //generic clock generator 1 id for GENDIV
#define GCLK_GENDIV_ID_GCLKGEN2    0x02     //generic clock generator 2 id for GENDIV
#define GCLK_GENCTRL_ID_GCLKGEN1   0x01     //generic clock generator 1 id for GENCTRL
#define GCLK_GENCTRL_ID_GCLKGEN2   0x02     //generic clock generator 1 id for GENCTRL
#define PORT_WRCONFIG_PMUX_E       0x04     //port multiplex option F
#define PORT_WRCONFIG_PINMASK_PB30 1 << 14  //pinmask for wrconfig

void EIC_Handler();

volatile uint8_t state = 0x01;

int main(void)
{
  /* Initialize the SAM system */
  SystemInit();
  
  //set up PB30
  REG_PORT_DIRSET1 = PORT_PB30;  //set port direction to output
  REG_PORT_WRCONFIG1 = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_WRPMUX |
                       PORT_WRCONFIG_PMUX(PORT_WRCONFIG_PMUX_E) | PORT_WRCONFIG_PMUXEN |
                       PORT_WRCONFIG_PINMASK(PORT_WRCONFIG_PINMASK_PB30);  //write config     

  //set generic clock for TCC0
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(0x0001) | GCLK_GENDIV_ID(GCLK_GENDIV_ID_GCLKGEN1);         //set prescaler
  REG_GCLK_GENCTRL = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M |
                     GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_GCLKGEN1);                                //enable generic clock generator 1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_TCC0_TCC1;  //configure generic clock 
  
  //set up timer TCC0 (PWM generation)
  REG_PM_APBCMASK |= PM_APBCMASK_TCC0;     //enable the TCC0 bus clock (CLK_TCx_APB)
  
  REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;  //set waveform generation mode
  while (TCC0->SYNCBUSY.bit.WAVE); 
  REG_TCC0_PER = TCC_PER_PER(6000);        //set period
  while (TCC0->SYNCBUSY.bit.PER);
  REG_TCC0_CC0 = TCC_CC_CC(0);          //set comparator value
  while (TCC0->SYNCBUSY.bit.CC0);
  REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;      //enable timer (PWM)
  while (TCC0->SYNCBUSY.bit.ENABLE);
  
  //set up button
  REG_PORT_WRCONFIG0 = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PULLEN | PORT_WRCONFIG_PMUXEN |
                       PORT_WRCONFIG_INEN | PORT_WRCONFIG_PINMASK(PORT_PA15);  //write configuration reg for port PA15 (enable input and pull resistor)
  REG_PORT_OUTSET0 = PORT_PA15;  //set pull resistor to pull-up
  
  //set up generic clock 2 for the external interrupt controller (EIC)
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(0x0002) | GCLK_GENDIV_ID(GCLK_GENDIV_ID_GCLKGEN2);                         //set frequency divider
  REG_GCLK_GENCTRL = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(GCLK_GENCTRL_ID_GCLKGEN2);  //enable generic clock 2 and set its source to the 8MHz internal oscillator
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_EIC;                        //assign generic clock 2 to EIC
  
  //set up EIC
  REG_EIC_CONFIG1 |= EIC_CONFIG_FILTEN7 | EIC_CONFIG_SENSE7_FALL;  //set interrupt to the falling edge (1 = second part(8 sense/part): 8 + 7 = 15)
  REG_EIC_INTENSET = EIC_INTENSET_EXTINT15;                        //enable interrupt for PA15
 
  REG_EIC_CTRL = EIC_CTRL_ENABLE;   //enable the external interrupt controller
  while(EIC->STATUS.bit.SYNCBUSY);  //wait until the external interrupt controller is activated
  
  NVIC_EnableIRQ(EIC_IRQn);  //enable interrupt
  
  while (1) {}
}

void EIC_Handler()  //interrupt service routine for the external interrupt controller
{
  REG_TCC0_CC0 = TCC_CC_CC((uint16_t)(state) * 2000);
  while (TCC0->SYNCBUSY.bit.CC0);
  
  if (state < 3)
    state++;
  else
    state = 0x00;
    
  REG_EIC_INTFLAG = EIC_INTFLAG_EXTINT15;  //reset flag 
}