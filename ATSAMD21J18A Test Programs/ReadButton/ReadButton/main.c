/*
 * ReadButton.c
 *
 * Created: 05/10/2019 10:53:25
 * Author : Elekes Norbert
 *
 * Description: Test for reading the state of the button connected to PA15. The button is activated when PA15 
 *              is pulled to ground. When the button is activated, the LED which is connected to PB30 will be 
 *              turned on. The LED turns on when PB30 is pulled to ground.
 *
 */ 

#include "sam.h"

int main(void)
{
  /* Initialize the SAM system */
  SystemInit();

  //set up the LED
  REG_PORT_DIRSET1 = PORT_PB30;  //set PB30 to output
  REG_PORT_OUTSET1 = PORT_PB30;  //pull PB30 to VCC -> LED off
  
  //set up the button
  REG_PORT_WRCONFIG0 = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PULLEN | 
                       PORT_WRCONFIG_INEN | PORT_WRCONFIG_PINMASK(PORT_PA15); //write configuration reg for port PA15 (enable input and pull resistor )
  REG_PORT_OUTSET0 = PORT_PA15;  //set pull resistor to pull-up  
  
  while (1) 
  {
    if (!(REG_PORT_IN0 & PORT_PA15))
    {
      //button pushed
      REG_PORT_OUTCLR1 = PORT_PB30;  //turn on LED
    }
    else
    {
      //button released
      REG_PORT_OUTSET1 = PORT_PB30;  //turn off LED
    }
  }
}
