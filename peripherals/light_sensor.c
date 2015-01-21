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


lightsens_t lightsens;

void LIGHTSENS_setState(char state){
	if(state == LIGHTSENS_INACTIVE){
		lightsens.state.INACTIVE = 1;
		lightsens.state.READING = 0;
		lightsens.state.READ_DONE = 0;
		lightsens.state.READ_REQ = 0;
	}else if(state == LIGHTSENS_READ_REQ){
		lightsens.state.INACTIVE = 0;
		lightsens.state.READING = 0;
		lightsens.state.READ_DONE = 0;
		lightsens.state.READ_REQ = 1;
	}else if(state == LIGHTSENS_DONE){
		lightsens.state.INACTIVE = 0;
		lightsens.state.READING = 0;
		lightsens.state.READ_DONE = 1;
		lightsens.state.READ_REQ = 0;
	}else if(state == LIGHTSENS_READING){
		lightsens.state.INACTIVE = 0;
		lightsens.state.READING = 1;
		lightsens.state.READ_DONE = 0;
		lightsens.state.READ_REQ = 0;
	}
}
/*----------------------------------------------------------------
 * \brief Light sensor init.
 * Enables clock for ADC block, and configures adc to 14MHz and resets adc
 * Enables interrupt for adc in NVIC
 *---------------------------------------------------------------- */
void LIGHTSENS_init(void){
	//Init clock ADC
	LIGHTSENS_setState(LIGHTSENS_INACTIVE);
	*AT91C_PMC_PCER1 = (1<<5);

	//Config adc
	*AT91C_ADCC_MR = (2<<8); //prescale clock to 14mhz
	*AT91C_ADCC_EMR = 3; //Interrupt on OUT of window
	*AT91C_ADCC_CWR = 0x70F|(0x8FF<<16); //Window thresholds
	*AT91C_ADCC_COR = (1<<16); //Enable diff mode
	//*AT91C_ADCC_CR = AT91C_ADC_SWRST;
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 5);
	NVIC_EnableIRQ(ADC_IRQn);
}

/*----------------------------------------------------------------
 * Light sensor start measurement.
 * Will enable adc channel 1 and start analog to digital conversion.
 * Interrupt is enabled on data ready
 *---------------------------------------------------------------- */
void LIGHTSENS_startMeas(void){
	LIGHTSENS_setState(LIGHTSENS_READING);
  *AT91C_ADCC_CHER = AT91C_ADC_CH0|AT91C_ADC_CH1; //channel enable AD13
  *AT91C_ADCC_IER = (1<<24); //Enabling COMPARE interrupt
  *AT91C_ADCC_CR = AT91C_ADC_START; //channel start conversion
  //*AT91C_ADCC_IER = (1<<1);

}

float LIGHTSENS_get(char ch){
    int reading;
    if(ch == 0){
	reading = *AT91C_ADCC_CDR0;
    }
    if(ch == 1){
	  reading = *AT91C_ADCC_CDR1;
	}

	//convert from 12-bit value to volt
	return (3.3/(0xFFF))*reading;
}

float LIGHTSENS_getDiff(){
	float rd0 =  LIGHTSENS_get(0);
	float rd1 =  LIGHTSENS_get(1);
	return (rd1-rd0);

}

/*----------------------------------------------------------------
 * ADC Handler
 * This function will handle interrupts on dataready from light sensor.
 * cLight_Sensor_State = 1
 * This flag indicates that the conversion is ready and data is ready to be read.
 *----------------------------------------------------------------*/
void ADC_Handler(void){
	LIGHTSENS_setState(LIGHTSENS_DONE);
	*AT91C_ADCC_IDR = (1<<24);

}



