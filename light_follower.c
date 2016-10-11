/*light_follower.h 
Mini module to make use of the light sensor module and servo control module.
Basically this enters a loop that tracks strongest light source (or dark source) until stopped.
During this time only interrupts may happen
Functions to use> void LightFollow();

Note: BLOCKING! When in follow mode NO main-loop readings are possible as of current implementation.

Blocks used: Indirectly through light and servo modules PWM, ADC

------- Usage example: -----
if(pressed == start){
	LightFollow();
}
---------------------------
TODO:
- 
*/
#include "light_follower.h"
#include <stdio.h>
#include "peripherals/light_sensor.h"
#include "peripherals/keypad.h"
#include "peripherals/servo.h"
#include "peripherals/display.h"
#include "includes/system_sam3x.h"
#include "includes/at91sam3x8.h"
#include <stdlib.h>

//extern int ms_counter = 0;
static void lightSens(){
	if(lightsens.state.READ_REQ ){
		LIGHTSENS_startMeas();
	}else if(lightsens.state.READ_DONE){
		//printf("Diff: %f\n",LIGHTSENS_getDiff());
		LIGHTSENS_setState(LIGHTSENS_INACTIVE);
		if(*AT91C_ADCC_CDR0 > 0x800){
				SERVO_setPos(SERVO_getPos()-100);
		}else{
			SERVO_setPos(SERVO_getPos()+100);
		}
	}
}
/*This enters light follow mode - blocking!*/
int LightFollow(){
	//SystemInit();
	SERVO_init();
	LIGHTSENS_init();
	SysTick_Config(84000);
	int counter = 0;
	while(1){
	  counter++;
	  if(counter > 5000){
		counter = 0;
	  	int reading = SERVO_getPos();
		  reading = (reading)/44; //Turn into angle
		  char *angle_str = malloc(12*sizeof(char *));
		  if(angle_str == 0){
		    //TODO Handle error
		  }
		  sprintf(angle_str, "%d degrees", reading);
		  DISPLAY_write(angle_str,128,0);
		  free(angle_str);
	  }
		lightSens();
		if(KEYPAD_read()==12){break;}
	}
	return 1;
}
