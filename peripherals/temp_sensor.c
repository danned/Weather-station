/*temp_sensor.c - Staffan Piledahl
Temp sensor driver for MAX6575L. Using asynchronous one-wire interface readings. Uses flags to tell main what to do next.
Sequence started using Reset (required before every reading). See example.
Works safely down to one reading every 20ms. can be read once ever 17ms with testing setup but not recommended.s

Functions to use> void Temp_Init(), void _Reset(), void _Read() and float _Get()

Blocks used: TC0,TC1, PIOB

------- Usage example: -----
extern char cTemp_Reset_Ready_Flag = 0;
extern char cTemp_Measurement_Ready_Flag = 0;

//<---Inside main loop--->
    //Start temp sensor reset somehow (for example using systick)
    if(cTimeToReadTemp == 1){
       Temp_Reset();
    }
     //start temp sensor reading
    if(cTemp_Reset_Ready_Flag  == 1){
     Temp_Read();
    }
	//fetch reading
    if(cTemp_Measurement_Ready_Flag == 1){
    float temp = Temp_Get();
    printf("%f",temp);
    }
---------------------------
TODO:
- Cleanup comments
- Testing, what if systicks gets ahead of my timer
*/
#define START_DELAY 25
#define SETUP_DELAY 840

#include "temp_sensor.h"
#include "../includes/system_sam3x.h"
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"
char cTemp_Reset_Ready_Flag = 0; //Shared!
char cTemp_Measurement_Ready_Flag = 0; //Shared!

/*
----------------PUBLIC functions-----------------
*/
void Temp_Init(void){
	*AT91C_PMC_PCER = (1<<12);//Init PIOB, PID 12
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
void Temp_Reset(void){

	*AT91C_PIOB_OER = (1<<25); //Set pin low
	*AT91C_TC1_CCR = AT91C_TC_SWTRG; //Start counting
	*AT91C_TC1_IER = AT91C_TC_CPCS; //Enable interrupt on
}
void Temp_Read(void){
  	*AT91C_PIOB_ODR = (1<<25); //Setup with high 10us
	 Delay(SETUP_DELAY);
	 *AT91C_PIOB_OER = (1<<25); //Start pulse assuming low 2.5us
	 *AT91C_TC0_CCR = (1<<2);
	 Delay(START_DELAY); //Wait for start pulse to finish
	 *AT91C_PIOB_ODR = (1<<25); //High again and wait for response

	 *AT91C_TC0_CCR = 1;
}
float Temp_Get(void){

	uint32_t nTimeA = *AT91C_TC0_RA;
	uint32_t nTimeB = *AT91C_TC0_RB;
	uint32_t nTimeDiff = nTimeB - nTimeA;
	//printf("Measured temp: %.2f \n",Conv_Celsius(nTimeDiff));
	return Conv_Celsius(nTimeDiff);
}


/* ---------------PRIVATE ------------------------
*/
//When 15ms reset pulse is complete we get into tc0
//OR when RB has loaded value from chip response
void TC0_Handler(void){
  volatile int dummy = *AT91C_TC0_SR;
    //*AT91C_TC0_IDR = AT91C_TC_LDRBS;
     cTemp_Measurement_Ready_Flag = 1;
}
void TC1_Handler(void){
  int dummy = *AT91C_TC1_SR;
    *AT91C_TC1_IDR = AT91C_TC_CPCS;
   cTemp_Reset_Ready_Flag = 1;
}

/*
Converts ticks into celsius
*/
float Conv_Celsius(int ticks){
  float result = ((float) ticks / (42*5) - 273.15); //(((float)ticks/42)/5)-273.15;
  return result;
}

