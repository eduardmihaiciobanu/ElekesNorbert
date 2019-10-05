/*
 * TurnOnLED.c
 *
 * Created: 05/10/2019 10:16:39
 * Author : Elekes Norbert
 *
 * Description: Test for turning on an LED connected to PB30. The LED is on when PB30 is pulled to ground.
 */ 

#include "sam.h"

int main(void)
{
  /* Initialize the SAM system */
  SystemInit();

  REG_PORT_DIRSET1 = PORT_PB30;  //Set the direction of PB30 to output. (1 = Port B, PORT_PB30 = 1 << 30)
  REG_PORT_OUTSET1 = PORT_PB30;  //Pull PB30 to VCC -> LED off
  REG_PORT_OUTCLR1 = PORT_PB30;  //Pull PB30 to ground -> LED on
  
  while (1) 
  {
  }
}
