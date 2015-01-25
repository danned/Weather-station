/*
 * Station.c
 *
 *
 *
 * Created: 2014-12-11 17:03:22
 *  Author: Daniel and Staffan
 */
#include "mem.h"
#include "rtc.h"
#include "controller.h"
#include "includes/common.h"
#include "includes/at91sam3x8.h"
#include "includes/system_sam3x.h"
#include "peripherals/air_sensor.h"
#include "peripherals/temp_sensor.h"
#include "peripherals/light_sensor.h"
#include "peripherals/keypad.h"
#include "peripherals/servo.h"
#include "peripherals/display.h"
#include "peripherals/Temp_test.h"

program_t sta;

char *time;
int ms_counter = 0;
int key_counter = 0;
char update_time = 0;
int sec_counter = 0;
//char cMeasure = 0; deprecated. see sta.status.MEAS
//char cTimeToReadTemp = 0; deprecated. see sta.status.TEMP_REQ;

/**
 * \brief inline method for saving current values to memory
 */
static void saveMeas(){
	if(sta.status.MEAS){
			sta.status.MEAS = 0;
			if(sta.n_count>0){
				MEM_save((sta.temp_sum_f/mem.temp_count), AIRSENS_getPres() );
				sta.temp_sum_f = 0;
				sta.n_count = 0;
			}
		}
		if(sta.status.NEW_DAY){
			sta.status.NEW_DAY = 0;
			MEM_newDay();
		}
}

/**
 * \brief call this function to start measuring temperature
 */
static void measure(){
	static int meas_count = 0;
	meas_count++;
	if(sta.FAST_MODE){//fast mode
		if(meas_count > (1000/sta.n_avg-40)){
			sta.status.TEMP_REQ = 1;

			meas_count = 0;
		}
	}else if(!sta.FAST_MODE){//normal mode
		if(meas_count > (60000/sta.n_avg)){
			sta.status.TEMP_REQ = 1;
			meas_count = 0;
		}
	}
}

/**
 * \brief inline method used for measuring temperature. start flag is set by systick
 */
static void tempSens(){
	if(sta.status.TEMP_REQ == 1){
		TEMP_reset();
	}
	if(temperature.status.RESET_READY){
		TEMP_read();
	}
	if(temperature.status.READ_READY){
		sta.temp_sum_f += TEMP_get();
		sta.n_count++;
	}

}

static void lightSens(){
	if(lightsens.state.READ_REQ){
		LIGHTSENS_startMeas();
	}else if(lightsens.state.READ_DONE){
		LIGHTSENS_setState(0);
	}
}


static void stationInit(){
	/* Initialize the SAM system */
	SystemInit();
	
	RTC_Init(13,30,00,20,15,01,28,1);//should be initialized at welcome screen

	/* start initializing variables */
	sta.FAST_MODE = 0;
	sta.state = 0;
	sta.n_avg = 3;
	sta.n_count = 0;
	sta.alm_h = 27; //Standard Upper and lower limits for temperature alarm indicator
	sta.alm_l = 15;
	sta.temp_sum_f = 0;
	sta.status.MEAS = 0;
	sta.status.TEMP_REQ = 0;
	/* stop initializing variables */
	MEM_init();
	//MEM_test();
	KEYPAD_init();
	TEMP_init();
	DISPLAY_init();
	LIGHTSENS_init();
	SERVO_init();
	SysTick_Config(84000); // config systick to interrupt w/ 1 interrupt/ms
	/*Show system test screen for a while*/
	DISPLAY_writeHeader(1,"System test","00:00");
	DISPLAY_writeTestingScreen(mem_temp_test(),0,0,MEM_test()); //TODO add the other test results
	//Push any button to remove testing screen
	while(!KEYPAD_read()){}
	DISPLAY_clearText();
	//Delay(2000000); //FIXME Quickfix for bouncing

	/*Force user to enter date and time*/
	DISPLAY_writeHeader(0,"Date and time","00:00");
	DISPLAY_writeSidebar();
	DISPLAY_writeDateSetScreen(); //TODO PUT THIS BACK<-------------
	ctrl.status.MEM_WARN = 0;
	ctrl.status.TEMP_WARN = 0;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{


    stationInit();// initializes station
    //TODO Insert set state and draw date-time screen with void DISPLAY_writeDatetimeScreen(void)
	time = malloc(8*sizeof(char));
	if(time == 0){
		 MEM_remove();
		 time = malloc(8*sizeof(char));
	}
	static char pressed = 0;
	while (1){
		if(key_counter>100 && pressed == 0){
			key_counter = 0;
			pressed = KEYPAD_read();
		}
		if(pressed != 0){
			CTRL_userInput(pressed);
			pressed = 0;
		}
		/* -- FIXME Quickfix for ISSUE #10 --- */
		//Delay(40000);
		/*--------------------------------------*/

		/*Readings - if flags are set*/
		tempSens();//if flag is set, temp will be measured
		lightSens();
		saveMeas();//if flag is set. Values will be saved

		//Update dynamic things every second
		if(update_time){
			update_time = 0;
			 RTC_Get_Time_String(time);
			 DISPLAY_write(time,24,0);
		}

    }
}


/**
 * \brief SysTick triggers measurement of sensors TODO maybe use another counter. SysTick is used by other functions which needs it to interrupt every ms
 * Should make N interrupts per minute/second depending on mode. Only for temp sensor
 */
void SysTick_Handler(void){
	measure();
	ms_counter ++;
	sec_counter ++;
	key_counter++;
	if((ms_counter >= 100 && !lightsens.state.READING) ){
	 lightsens.state.READ_REQ = 1;
	 ms_counter = 0;
	}
	if(sec_counter >999){
		sec_counter = 0;
		update_time = 1;

	}
}

void NMI_Handler( void ){while(1){}}
void HardFault_Handler( void ){while(1){}}
void MemManage_Handler( void ){while(1){}}
void BusFault_Handler( void ){while(1){}}
void UsageFault_Handler( void ){while(1){}}
void SVC_Handler( void ){while(1){}}
void DebugMon_Handler( void ){while(1){}}
void PendSV_Handler( void ){while(1){}}
//void SysTick_Handler( void ){while(1){}}
