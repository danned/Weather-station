/*----------------------------------------------------------------
 * Servo drivers implementations
 * This file contains implementations for the servo.
 *
 * Uses pwm to control servo. 1.5 ms is shortest pulse and 2.3 ms is longest.
 * Period used is 20 ms. Use register *AT91C_PWMC_CH2_CDTYR to set and get servo angle
 * 2625 ticks is 1 ms
 * last updated 2014-12-10
 *----------------------------------------------------------------*/

#include "servo.h"
#include "../includes/at91sam3x8.h"



/*----------------------------------------------------------------
 * Servo init.
 * Clock for PWM block and PIOA is activated.
 * PIOA pin 20 absr and pdr will be set to let pwm control pin. PWM Channel 2 is used and enabled
 * PWM is configured to set servo in default state.
 * Period time is 20 ms, pwm clock is mck/32
 * CDTYR register will be set to 1.5 ms duty time
 *---------------------------------------------------------------- */
void Servo_Init( void ){
   *AT91C_PMC_PCER1 =   (1<<4);
   *AT91C_PMC_PCER =   (1<<11);
   *AT91C_PIOA_ABMR = (1<<20);
   *AT91C_PIOA_PDR  =   (1<<20);
   
   *AT91C_PWMC_ENA   =   (1<<2);
   *AT91C_PWMC_CH2_CMR = AT91C_PWMC_CPRE_MCK_DIV_32;
   *AT91C_PWMC_CH2_CPRDR = 52500;// set to 20 ms
   
   *AT91C_PWMC_CH2_CDTYR = (int)2625*1.5;
   
}