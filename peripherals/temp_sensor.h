/*temp_sensor.h - Staffan Piledahl
Temp sensor driver for MAX6575L. Using asynchronous one-wire interface readings. Uses flags to tell main what to do next.
Sequence started using Reset (required before every reading). See example.
Works safely down to one reading every 20ms. can be read once ever 17ms with testing setup but not recommended.s

Functions to use> void TEMP_init(), void _Reset(), void _Read() and float _Get()

Blocks used: TC0,TC1, PIOB

------- Usage example: -----
extern char temperature.status.RESET_READY = 0;
extern char temperature.status.READ_READY = 0;

//<---Inside main loop--->
    //Start temp sensor reset somehow (for example using systick)
    if(cTimeToReadTemp == 1){
       TEMP_reset();
    }
     //start temp sensor reading
    if(temperature.status.RESET_READY  == 1){
     TEMP_read();
    }
	//fetch reading
    if(temperature.status.READ_READY == 1){
    float temp = TEMP_get();
    printf("%f",temp);
    }
---------------------------
TODO:
- 
*/
#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_
typedef struct{
	char RESET_READY  : 1;
	char READ_READY : 1;
}temp_status_t;

typedef struct{
	temp_status_t status;
}temp_t;

extern temp_t temperature;
/*Reset sequence - needed to give a start pulse*/
void TEMP_reset(void);
/*Read value*/
void TEMP_read();
/* initializes clock and pio for temp sensor*/
void TEMP_init(void);
/*Handles the interrupt from captured answer pulse */
void TC0_Handler(void);
/*Handles the interrupt from timer counting to 15ms*/
void TC1_Handler(void);
/*gets latest finished temp reading*/
float TEMP_get(void);
/* Simple helper to convert from time to celsius */
float TEMP_convCelsius(int ticks);
#endif // _TEMP_SENSOR_H_
