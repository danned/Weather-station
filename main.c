/*
 * Station.c
 *
 * Created: 2014-12-11 17:03:22
 *  Author: Daniel and Staffan
 */ 

#include "mem.h"
#include "peripherals/temp_sensor.h"
#include "peripherals/light_sensor.h"
#include "peripherals/keypad.h"
#include "peripherals/display.h"
#include "controller.h"
#include "includes/system_sam3x.h"
#include "includes/common.h"
#include "rtc.h"

float fTemp_Sum = 0;
int iN_Avg = 3;//default setting
int MODE = 0;//default mode

char nState = 0; //keeps track of current state
char nTempWarning = 0; //0 OR 1
char nMemWarning = 0; // 0 OR 2   2 is active warning!

extern char cTemp_Reset_Ready_Flag;
extern char cTemp_Measurement_Ready_Flag;

char cMeasure = 0;
char cTimeToReadTemp = 0;

void Save_Measurements();
void Temp_Sensor();
void Light_Sensor();
void Measure();
void Station_Init();
/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	
    Station_Init();// initializes station


	while (1) 
    {
		 /*CONTROLLER CODE*/
		//---- State INDEPENDENT Keypad readings ----
		unsigned char pressed = Keypad_Read();
		Controller_User_Input(pressed);
		
		
		Temp_Sensor();//if flag is set, temp will be measured
		Light_Sensor();	
		Save_Measurements();//if flag is set. Values will be saved
		
		//Display_Write("hejhej",0,0);
    }
}

/**
 * \brief handles saving of measurements
 * Requires a global var of N saying how many measurements to average
 * aswell var with sum of measurement results. Globals are found in common.h.
 */
void RTC_Handler(void){
	cMeasure = 1;
	*AT91C_RTC_SCCR = 3<<1;
	//printf("RTC Interrupt!");
}

/**
 * \brief SysTick triggers measurement of sensors TODO maybe use another counter. SysTick is used by other functions which needs it to interrupt every ms
 * Should make N interrupts per minute/second depending on mode. 
 */
void SysTick_Handler(void){
	Measure();
}

/**
 * \brief inline method for saving current values to memory
 */
inline void Save_Measurements(){
	if(cMeasure > 0){
		cMeasure = 0;
		Memory_Save(fTemp_Sum/(float)iN_Avg);
		fTemp_Sum = 0;
		//TODO: add saving of humidity. Maybe display updates
		cLight_Sensor_State = 0; //sets to update lux value onscreen
	}
}

/**
 * \brief call this function to start measuring
 */
inline void Measure(){
	 cTimeToReadTemp = 1;
	 //TODO: add measuring of humidity
}

/**
 * \brief inline method used for measuring temperature. start flag is set by systick
 */
inline void Temp_Sensor(){
	if(cTimeToReadTemp == 1){
		Temp_Reset();
	}
	if(cTemp_Reset_Ready_Flag  == 1){
		Temp_Read();
	}
	if(cTemp_Measurement_Ready_Flag == 1){
		fTemp_Sum += Temp_Get();
		//printf("%f",temp);
	}

}

inline void Light_Sensor(){
	if(cLight_Sensor_State  == 1){
		cLight_Sensor_State = -1;
	}
}


inline void Station_Init(){
	/* Initialize the SAM system */
	SystemInit();
	SysTick_Config(84000); // config systick to interrupt w/ 1 interrupt/ms
	
	RTC_Init(00,00,22,20,14,12,14,7);//should be initialized at welcome screen
	Keypad_Init();
	Temp_Init();	
	Display_Init();
	/*Build UI first time*/
	Display_Write_Header(1,"System test","00:00");
	Display_Write_Sidebar(0);
	//Display_Write_Testing_Screen(Temp_Test(),0,0,0); //TODO implement
}

//TEST FROM STAEF GIT