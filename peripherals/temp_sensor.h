//temp_sensor.h
#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_

/*  */
void Start_Temp_Reset( void );

/* */
void Temp_Start_Measure(void);
/* */
void Temp_Read();
/* initializes clock and pio for temp sensor*/
void Temp_Init(void);
/* */
void TC0_Handler(void);

/*  */
float Conv_Celsius(int ticks);
float Temp_Get(void);
void Temp_Reset(void);
#endif // _TEMP_SENSOR_H_
