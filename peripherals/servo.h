/*----------------------------------------------------------------
 * Servo drivers implementations
 * This file contains implementations for the servo.
 *
 * Uses pwm to controll servo. 1.5 ms is shortes pulse and 2.3 ms is longest.
 * Period used is 20 ms. Use register *AT91C_PWMC_CH2_CDTYR to set and get servo angle
 * 2625 ticks is 1 ms
 * last updated 2014-12-10
 *----------------------------------------------------------------*/


/*----------------------------------------------------------------
 * Servo init.
 * Clock for PWM block and PIOA is activated.
 * PIOA pin 20 absr and pdr will be set to let pwm controll pin. PWM Channel 2 is used and enabled
 * PWM is configured to set servo in default state.
 * Period time is 20 ms, pwm clock is mck/32
 * CDTYR register will be set to 1.5 ms duty time
 *---------------------------------------------------------------- */
void Servo_Init( void );