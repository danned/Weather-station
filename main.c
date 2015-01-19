/*
 * Station.c
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

/*float fTemp_Sum = 0;
int iN_Avg = 3;//default setting
int MODE = 0;//default mode

char nState = 0; //keeps track of current state*/
char nTempWarning = 0; //0 OR 1
char nMemWarning = 0; // 0 OR 2   2 is active warning!

int ms_counter = 0;

//char cMeasure = 0; deprecated. see sta.status.MEAS
//char cTimeToReadTemp = 0; deprecated. see sta.status.TEMP_REQ;
/** Prototypes. NOT USED
void saveMeas();
void tempSensor();
void lightSensor();
void measure();
void stationInit();
*/

/**
 * \brief inline method for saving current values to memory
 */
static void saveMeas(){
	if(sta.status.MEAS > 0){
		sta.status.MEAS = 0;
		MEM_saveTemp(sta.temp_sum_f/(float)sta.n_avg );
		sta.temp_sum_f = 0;
		//TODO: add saving of humidity. Maybe display updates
		if(cLight_Sensor_State == -1)
			cLight_Sensor_State = 0; //sets to update lux value onscreen
	}
}

/**
 * \brief call this function to start measuring
 */
static void measure(){
	 sta.status.TEMP_REQ = 1;
	 //TODO: add measuring of humidity
}

/**
 * \brief inline method used for measuring temperature. start flag is set by systick
 */
static void tempSens(){
	if(sta.status.TEMP_REQ == 1){
		TEMP_reset();
	}
	if(temperature.status.RESET_READY  == 1){
		TEMP_read();
	}
	if(temperature.status.READ_READY == 1){
		sta.temp_sum_f += TEMP_get();
		//printf("%f",temp);
	}

}

static void lightSens(){
	if(cLight_Sensor_State  == 0 ){
		LIGHTSENS_startMeas();
		cLight_Sensor_State = 2;
	}else if(cLight_Sensor_State == 1){
		//printf("Diff: %f\n",LIGHTSENS_getDiff());
		cLight_Sensor_State = -1;
	}
}


static void stationInit(){
	/* Initialize the SAM system */
	SystemInit();
	SysTick_Config(84000); // config systick to interrupt w/ 1 interrupt/ms
	//RTC_Init(00,00,22,20,14,12,14,7);//should be initialized at welcome screen

	/* start initializing variables */
	sta.FAST_MODE = 0;
	sta.state = 0;
	sta.n_avg = 3;
	sta.alm_h = 27; //Standard Upper and lower limits for temperature alarm indicator
	sta.alm_l = 15;
	sta.temp_sum_f = 0;
	sta.status.MEAS = 0;
	sta.status.TEMP_REQ = 0;
	/* stop initializing variables */
	MEM_init();
	//MEM_test();
	Keypad_Init();
	TEMP_init();
	DISPLAY_init();
	LIGHTSENS_init();
	SERVO_init();

	/*Show system test screen for a while*/
	DISPLAY_writeHeader(1,"System test","00:00");
	DISPLAY_writeTestingScreen(mem_temp_test(),0,0,0); //TODO add the other test results
	//Push any button to remove testing screen
	while(!Keypad_Read()){}
	DISPLAY_clearText();
	Delay(2000000);

	/*Force user to enter date and time*/
	DISPLAY_writeHeader(0,"Date and time","00:00");
	DISPLAY_writeSidebar();
	//DISPLAY_writeDateSetScreen(); //TODO PUT THIS BACK<-------------

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
	while (1)
    {


		 /*CONTROLLER CODE*/
		//---- State INDEPENDENT Keypad readings ----
		unsigned char pressed = Keypad_Read();
		Controller_User_Input(pressed);
		tempSens();//if flag is set, temp will be measured
		lightSens();
		saveMeas();//if flag is set. Values will be saved
		//DISPLAY_write("hejhej",0,0);
    }
}

/**
 * \brief Triggers at new day in normal mode. Every hour in fast mode.
 * adds a new node for temp and updates var for airpressure
 * Requires a global var of N saying how many measurements to average
 * aswell var with sum of measurement results. Globals are found in common.h.
 */
void RTC_Handler(void){
	//should trigger every minute for normal mode. Every second for fast mode

	sta.status.MEAS = 1;

	//should trigger every day for normal mode. Every hour for fast mode
	//sta.status.NEW_DAY = 1;




	*AT91C_RTC_SCCR = 3<<1;
	//printf("RTC Interrupt!");
}

/**
 * \brief SysTick triggers measurement of sensors TODO maybe use another counter. SysTick is used by other functions which needs it to interrupt every ms
 * Should make N interrupts per minute/second depending on mode. Only for temp sensor
 */
void SysTick_Handler(void){
	measure();
	ms_counter ++;
	if((ms_counter >= 100 && cLight_Sensor_State != 2) ){
	 cLight_Sensor_State = 0;
	 ms_counter = 0;
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
