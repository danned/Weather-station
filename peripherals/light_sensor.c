/*----------------------------------------------------------------
 * Light sensor drivers implementations
 * This file contains implementations for the servo.
 *
 * This sensor uses interrupts triggered by ADC
 * You have to make a extern variable in your program called cLight_Sensor_State. Set state to 0 when reading is required
 * In main program do below
 * if state == 0 
 *   Call Light_Sensor_Start_Measure
 * 
 * ADC handler will set state to 1 when reading is done
 * cLight_Sensor_State 2 is reading light state
 * last updated 2014-12-10
 *----------------------------------------------------------------*/
#include "light_sensor.h"
#include "../includes/common.h"
#include "../includes/at91sam3x8.h"
#include "../includes/system_sam3x.h"



char cLight_Sensor_State = -1;
/*----------------------------------------------------------------
 * Light sensor init.
 * Enables clock for ADC block, and configures adc to 14mhz and resets adc
 * Enables interrupt for adc in NVIC
 *---------------------------------------------------------------- */
void Light_Sensor_Init(void){
	//Init clock ADC
	*AT91C_PMC_PCER1 = (1<<5);
  
	//Config adc
	*AT91C_ADCC_MR = (2<<8); //prescale clock to 14mhz
	*AT91C_ADCC_CR = (1);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 5);
	NVIC_EnableIRQ(ADC_IRQn);
}

/*----------------------------------------------------------------
 * Light sensor start measurement.
 * Will enable adc channel 1 and start analog to digital conversion.
 * Interrupt is enabled on data ready
 *---------------------------------------------------------------- */
void Light_Sensor_Start_Measure(void){
  cLight_Sensor_State = 2;
  *AT91C_ADCC_CHER = (1<<1); //channel enable AD1
  *AT91C_ADCC_CR = (2); //channel start conversion
  *AT91C_ADCC_IER = (1<<24);
  //*AT91C_ADCC_IER = (1<<1);
  
}

float Light_Sensor_Get(){
	int reading = *AT91C_ADCC_LCDR;
	//convert from 12-bit value to volt
	return (3.3/(0xFFF))*reading;
}


/*----------------------------------------------------------------
 * ADC Handler
 * This function will handle interrupts on dataready from light sensor.
 * cLight_Sensor_State = 1 
 * This flag indicates that the conversion is ready and data is ready to be read.
 *----------------------------------------------------------------*/
void ADC_Handler(void){
  cLight_Sensor_State = 1;
  *AT91C_ADCC_IDR = (1<<24);
}



