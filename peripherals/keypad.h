/*----------------------------------------------------------------
 * Keypad drivers header
 * This file contains declarations for the keypad.
 *
 * Call for Keypad_Init from program to set up keypad.
 * Then use Keypad_Read to check if there are any buttons pressed on pad
 * last updated 2014-12-10
 *----------------------------------------------------------------*/
#ifndef _KEYPAD_H_
#define _KEYPAD_H_
/* ----------------------------------------------------------------
 * Keypad Read
 * Configures pin 7 8 9 as output, to set columns high and low
 * Enables 74 chip to enable databus to keypad
 * Loops through and checks if any button is pressed
 * if a button is found to be pressed, value will be save to variable.
 * Disables 74 chip
 * Configures pin 7 8 9 as input again
 * Returns value of last found button
 * ----------------------------------------------------------------*/
int Keypad_Read(void);


/* --------------------------------------------------------
 * Keypad Init
 * Will activate clock on PIOC and PIOD
 * PIOC pin 2 3 4 5 will be configured as input
 * PIOD pin 2 and 0 will be configured as output set to 1
 * --------------------------------------------------------*/
void Keypad_Init(void);

#endif // _KEYPAD_H_
