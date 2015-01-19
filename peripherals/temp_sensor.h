/*temp_sensor.h - Staffan Piledahl
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
