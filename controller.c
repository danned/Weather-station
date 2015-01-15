/* Controller that handles the different states
and how they respond to user input.
Depends on:
sta.state
nTempWarning
nMemWarning

Usage example:
while(1){
   char pressed = Keypad_Read();
   Controller_User_Input(pressed);
}
TODO:
- Big cleanup
- Dependency checks
- Create helpers
- Testing

*/

#include "controller.h"
#include <stdlib.h>
#include <stdio.h>
#include "peripherals/display.h"
#include "peripherals/temp_sensor.h"
#include "peripherals/air_sensor.h"
#include "includes/at91sam3x8.h"
#include "includes/common.h"
#include "rtc.h"
extern signed char cLight_Sensor_State;

/* Main flow control structure - takes a pressed key val*/
char Controller_User_Input(volatile char pressed){
		if(pressed != 0){
	      if(pressed <= 5){
			Display_Clear_Text();
	  		Display_Clear_Graphics(); //TODO change this to only clear whats necessary
	  		//Alloc time and date string
			char *time = malloc(15*sizeof(char *));
			if(time == 0){
			  return 0;
			}
	        char *date = malloc(12*sizeof(char *)); //TODO change from 15
			if(date == 0){
			  return 0;
			}
			//Populate strings with time and date
			RTC_Get_Time_String(time);
			RTC_Get_Date_String(date);

			//First 5 menu options:
	        switch(pressed){
	          case 1:
		        sta.state = 1;
		        //Allocate memory for strings to be used
				char *air = malloc(15*sizeof(char *));
				if(air == 0){
				  return 0;
				}
			    char *temp = malloc(15*sizeof(char *));
				if(temp == 0){
				  return 0;
				}
				char *lux = malloc(15*sizeof(char *));
				if(lux == 0){
				  return 0;
				}

				//Get current temperature
				float temperature = Temp_Get();
				if(temperature >25 || temperature <0){
					nTempWarning = 1; //TODO not correct
				}
				else{
					nTempWarning = 0;
				}
			    sprintf(temp, "%.2f", temperature);// Populate string

				//Get current light
				int reading = *AT91C_ADCC_LCDR;
			    float voltage = (3.3/(0xFFF))*reading;//convert from 12-bit value to volt to lux
			    float luxfloat = 3000 * (1/(voltage*voltage));//3v = 60lux , 1v 3000 lux 2v = 1000 lux
			    sprintf(lux, "%.2f", luxfloat);// Populate string

			 	//Get current Air pressure
			    int air_reading = AIRSENS_getPress();
			    sprintf(air, "%d", air_reading);// Populate string

				//Write to display and finish
		        Display_Write_Header(Controller_Get_Warnings(), "Home screen", time);
		        Display_Write_Home_Screen(temp,lux,air,date);
			    free(lux);
				free(temp);
				free(air);

	            break;

	          case 2:
	            sta.state = 2;
			    Display_Write_Header(Controller_Get_Warnings(), "Sun tracker", time);
			    Display_Write_Light_Screen();
			    //TODO Enable tracking mode, also remember to disable on state change somehow
	            break;
	          case 3:
	            sta.state = 3;

	            Display_Write_Header(Controller_Get_Warnings(), "Temp Hist", time);
	            Display_Write_Temp_Screen(date); //This has to take the  temp data or something

	            break;

	          case 4:
	            sta.state = 4;
	            Display_Write_Header(Controller_Get_Warnings(), "Air History", time);
	            Display_Write_Air_Screen(date);
	            break;
	          case 5:
	            sta.state = 5;
	            Display_Write_Header(Controller_Get_Warnings(), "Settings Screen", time);
	            Display_Write_Settings_Screen();
	            break;
	        }
			free(time);
			free(date);
	        Display_Write_Sidebar();
	        /*
	        *AT91C_PWMC_CH2_CDTYR = (int)1838.0*(1.0+((1.0/9.0)*(float)pressed));
	        printf("%d",pressed);*/
	        pressed = 0;
	      }
	      //State dependent keypad readings
	      else{
		switch(pressed){
	        case 6:
	        //TODO Stop light follower

	          break;
	        case 7:
	        //TODO Start light follower
	          break;
	        case 8:
	          //TODO State 5 move up (select setting above)(also increase)
	          break;
	        case 9:
	          break;
	        case 10:
	        if(sta.state == 2){cLight_Sensor_State = 0;int a = LightFollow();}
	          break;
	        case 11:
	          //TODO State 5 move down (select setting below)(also decrease)
	          break;
	        case 12:
	          if(sta.state == 2){cLight_Sensor_State = -1;}
	          break;
			}
		}
	}
	return 1;
}

char Controller_Get_Warnings(void){
  return nTempWarning+nMemWarning;
}


