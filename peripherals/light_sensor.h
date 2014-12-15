/*----------------------------------------------------------------
 * Light sensor drivers header
 * This file contains declarations for the servo.
 *
 * This sensor uses interrupts triggered by ADC
 * You have to make a global variable in your program called cLight_Sensor_State set to -1. can make time module set state to 0 when temp reading is requested.
 * In main program do below
 * if state == 0 
 *   Call Light_Sensor_Start_Measure
 * 
 * ADC handler will set state to 1 when reading is done
 * cLight_Sensor_State 2 is reading light state
 * last updated 2014-12-10
 *----------------------------------------------------------------*/


/*----------------------------------------------------------------
 * This variable is used to keep in track what state the light sensor is.
 * 1 is done. -1 is inactive. 2 is busy. 0 is set when reading is required
 *----------------------------------------------------------------*/
extern char cLight_Sensor_State;


/*----------------------------------------------------------------
 * Light sensor init.
 * Enables clock for ADC block, and configures adc to 14mhz and resets adc
 * Enables interrupt for ADC in NVIC
 *---------------------------------------------------------------- */
void Light_Sensor_Init(void);


/*----------------------------------------------------------------
 * Light sensor start measurement.
 * Will enable adc channel 1 and start analog to digital conversion.
 * Interrupt is enabled on data ready
 *---------------------------------------------------------------- */
void Light_Sensor_Start_Measure(void);

/*----------------------------------------------------------------
 * ADC Handler
 * This function will handle interrupts on dataready from light sensor.
 * cLight_Sensor_State = 1 
 * This flag indicates that the conversion is ready and data is ready to be read.
 *----------------------------------------------------------------*/
void ADC_Handler(void);