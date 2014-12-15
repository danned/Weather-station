/*
 * Station.c
 *
 * Created: 2014-12-11 17:03:22
 *  Author: Daniel
 */ 

#include "mem.h"
#include "peripherals/keypad.h"
#include "peripherals/display.h"
#include "includes/system_sam3x.h"
#include "includes/common.h"
#include "rtc.h"

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	int MODE = 0;
	RTC_Init(00,00,22,20,14,12,14,7);
	//Memory_Init();
	//Keypad_Init();
	//Display_Init();
	//Display_Set_Default_State();
    while (1) 
    {
		//Display_Write("hejhej",0,0);
    }
}

/**
 * \brief handles saving of measurements
 */
void RTC_Handler(void){
	
	//printf("RTC Interrupt!");
}