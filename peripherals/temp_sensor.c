#define START_DELAY 25
#define SETUP_DELAY 840

#include "temp_sensor.h"
#include "../includes/system_sam3x.h"
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"
temp_t temperature;

/************************************************************************/
/* Init function for temp temp_sensor				                    */
/************************************************************************/
void TEMP_init(void){
	temperature.status.RESET_READY = 0; //Shared!
	temperature.status.READ_READY = 0; //Shared!
	*AT91C_PMC_PCER = (0xF<<12);//Init PIOB, PID 12
	*AT91C_PIOB_PER = (1<<25); //PB25 - TIOA0 - Pin 2 arduino
	*AT91C_PIOB_ODR = (1<<25);
	*AT91C_PIOB_CODR = (1<<25); //We want low as default state

	//Init TC0, PID27 Used to capture high-low-high response
	*AT91C_PMC_PCER = (1<<27);
	*AT91C_TC0_CMR = 0x0;
	*AT91C_TC0_CMR = AT91C_TC_LDBDIS|BIT_17_AND_18; //Interrupts when second register loads
  	*AT91C_TC0_CCR = 1;
	*AT91C_TC0_IER = AT91C_TC_LDRBS;
	NVIC_ClearPendingIRQ(TC0_IRQn);
	NVIC_SetPriority(TC0_IRQn, 6);
	NVIC_EnableIRQ(TC0_IRQn);

	//Init TC1, PID28 Used to time 15ms for use in the Reset sequence of the chip
	*AT91C_PMC_PCER = (1<<28);
	*AT91C_TC1_RC = (615000);
	*AT91C_TC1_CMR = AT91C_TC_WAVE|AT91C_TC_WAVESEL_UP_AUTO|AT91C_TC_CPCSTOP; //Setup for 15ms count
	*AT91C_TC1_CCR = AT91C_TC_CLKEN;
	NVIC_ClearPendingIRQ(TC1_IRQn);
  	NVIC_SetPriority(TC1_IRQn, 7);
  	NVIC_EnableIRQ(TC1_IRQn);

}
/************************************************************************/
/* Making a reading	- 3 steps								            */
/************************************************************************/
/*First step of making a reading*/
void TEMP_reset(void){
	sta.status.TEMP_REQ = 0;
	*AT91C_PIOB_OER = (1<<25); //Set pin low
	*AT91C_TC1_CCR = AT91C_TC_SWTRG; //Start counting
	*AT91C_TC1_IER = AT91C_TC_CPCS; //Enable interrupt on
}
/*Second step of making a reading*/
void TEMP_read(void){
	temperature.status.RESET_READY = 0;
  	*AT91C_PIOB_ODR = (1<<25); //Setup with high 10us
	 Delay(SETUP_DELAY);
	 *AT91C_PIOB_OER = (1<<25); //Start pulse assuming low 2.5us
	 *AT91C_TC0_CCR = (1<<2);
	 Delay(START_DELAY); //Wait for start pulse to finish
	 *AT91C_PIOB_ODR = (1<<25); //High again and wait for response

	 *AT91C_TC0_CCR = 1;
}
/*Third step of making a reading*/
float TEMP_get(void){
	temperature.status.READ_READY = 0;
	uint32_t nTimeA = *AT91C_TC0_RA;
	uint32_t nTimeB = *AT91C_TC0_RB;
	uint32_t nTimeDiff = nTimeB - nTimeA;
	return TEMP_convCelsius(nTimeDiff);
}


/************************************************************************/
/* Internal											                    */
/************************************************************************/
//when RB has loaded value from chip response
void TC0_Handler(void){
  volatile int dummy = *AT91C_TC0_SR;
    //*AT91C_TC0_IDR = AT91C_TC_LDRBS;
     temperature.status.READ_READY = 1;
}
//When 15ms reset pulse is complete we get into tc0
void TC1_Handler(void){
  int dummy = *AT91C_TC1_SR;
    *AT91C_TC1_IDR = AT91C_TC_CPCS;
   temperature.status.RESET_READY = 1;
}
/*Converts ticks into celsius*/
float TEMP_convCelsius(int ticks){
  float result = ((float) ticks / (42*5) - 273.15); //(((float)ticks/42)/5)-273.15;
  return result;
}

