/*----------------------------------------------------------------
 * Keypad drivers
 * This file contains implementations for the keypad.
 *
 * last updated 2014-12-10
 *----------------------------------------------------------------*/

#include <stdio.h>
#include "keypad.h"
#include "../includes/common.h"
#include "../includes/at91sam3x8.h"

/* --------------------------------------------------------
 * Keypad Init
 * Will activate clock on PIOC and PIOD
 * PIOC pin 2 3 4 5 will be configured as input
 * PIOD pin 2 and 0 will be configured as output set to 1
 * --------------------------------------------------------*/
void Keypad_Init(void){
  
	/*  enables clock for PIOC & PIOD  if not already active*/
	*AT91C_PMC_PCER = (3<<13);
	/*enables control of pins*/  
	*AT91C_PIOC_PER =  BIT_2_TO_9 | BIT_12_TO_17 | BIT_18_19;//(0xFF3FC);
  
	/* initializes PIOD pin 2 and 0*/
	*AT91C_PIOD_PER = (1<<2)  | 1;
	*AT91C_PIOD_OER = (1<<2)  | 1;
	*AT91C_PIOD_SODR = (1<<2) | 1;
  
	/* initializes PIOC pin 2 3 4 5 as input*/
	*AT91C_PIOC_ODR = BIT_2_3_4_5;
}


/* ----------------------------------------------------------------
 * Keypad Read
 * Configures pin 7 8 9 as output. To set columns high and low
 * Enables 74 chip to enable databus to keypad
 * Loops through and checks if any button is pressed
 * if a button is found to be pressed, value will be save to variable.
 * Disables 74 chip
 * Configures pin 7 8 9 as input again
 * Returns last found button
 * ----------------------------------------------------------------
 */
int Keypad_Read(void){
	unsigned int button = 0;
  
	/* enables PIOC pin 7 8 9 as output */
	*AT91C_PIOC_OER =  BIT_7_8_9;
  
	/* enabling keypad */
	*AT91C_PIOD_CODR = (1<<2);
  
	
	/* 	Will iterate through every button on the keypad and check if pressed
		when a pressed button is found, value of button is saved to variable button */
	for(int col = 0 ; col < 3 ; col++){
		*AT91C_PIOC_SODR = BIT_7_8_9;
		*AT91C_PIOC_CODR = (1<<(7+col));
		for(int row = 0; row < 4 ;row++){
      
			if((*AT91C_PIOC_PDSR & (1<<(5-row))) == 0){
				button = row*3 + col + 1;
      		}
		}
	}
	
	/* disabling keypad */
	*AT91C_PIOD_SODR = (1<<2);
	/* enables PIOC pin 7 8 9 as input */
	*AT91C_PIOC_ODR =  BIT_7_8_9;
	return button;
}