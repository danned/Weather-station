#include "peripherals/light_sensor.h"
#include "peripherals/servo.h"
#include "includes/system_sam3x.h"
#include "includes/at91sam3x8.h"

//extern int ms_counter = 0;
static void lightSens(){
	if(cLight_Sensor_State  == 0 ){
		LIGHTSENS_startMeas();
	}else if(cLight_Sensor_State == 1){
		//printf("Diff: %f\n",LIGHTSENS_getDiff());
		cLight_Sensor_State = -1;
		if(*AT91C_ADCC_CDR0 > 0x800){
				SERVO_setPos(SERVO_getPos()-100);
		}else{
			SERVO_setPos(SERVO_getPos()+100);
		}
	}
}

int LightFollow(){
	//SystemInit();
	SERVO_init();
	LIGHTSENS_init();
	SysTick_Config(84000);
	while(1){
		lightSens();
		if(Keypad_Read()==12){break;}
	}
}

/*
void SysTick_Handler_Follower(void){//use this when starting followmode

	ms_counter ++;


	if((ms_counter >= 100 && cLight_Sensor_State != 2) ){
	 cLight_Sensor_State = 0;
	 ms_counter = 0;
	}
}*/