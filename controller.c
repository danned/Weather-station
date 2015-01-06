/* Controller that handles the different states
and how they respond to user input.
Depends on:
nState
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
#include "includes/at91sam3x8.h"
#include "rtc.h"


/* Main flow control structure - takes a pressed key val*/
char Controller_User_Input(volatile char pressed){
		if(pressed != 0){
	      if(pressed <= 5){
			Display_Clear_Text();
	  		Display_Clear_Graphics(); //TODO change this to only clear whats necessary
	  		//Get a time string of current time
			char *time = malloc(15*sizeof(char *));
			if(time == 0){
			  return 0;
			}
			RTC_Get_Time_String(time);
	        switch(pressed){
		  //TODO CLEANUP HOME SCREEN CODE
	          case 1:
	            nState = 1;
		    char *temp = malloc(15*sizeof(char *)); //TODO change from 15
			if(temp == 0){
			  return 0;
			}
			char *lux = malloc(15*sizeof(char *));
			if(lux == 0){
			  return 0;
			}
			float temperature = Temp_Get();
			if(temperature >25 || temperature <0){
				nTempWarning = 1; //TODO not correct,
			}
			else{
				nTempWarning = 0;
			}
		    sprintf(temp, "%.2f", temperature);// TODO do not use sprintf

			int reading = *AT91C_ADCC_LCDR;
		    //convert from 12-bit value to volt to lux
		    float voltage = (3.3/(0xFFF))*reading;
		    float luxfloat = 3000 * (1/(voltage*voltage));//3v = 60lux , 1v 3000 lux 2v = 1000 lux

		    sprintf(lux, "%.2f", luxfloat);// TODO do not use sprintf
	        Display_Write_Header(Controller_Get_Warnings(), "Home screen", time);
	        Display_Write_Home_Screen(temp,lux);
		    free(lux);
			free(temp);

	            break;

	          case 2:
	            nState = 2;
		    Display_Write_Header(Controller_Get_Warnings(), "Sun tracker", time);
		    Display_Write_Light_Screen();
		    //TODO Enable tracking mode, also remember to disable on state change somehow
	            break;
	          case 3:
	            nState = 3;
	            Display_Write_Header(Controller_Get_Warnings(), "Temp Hist", time);
	            Display_Write_Temp_Screen(); //This has to take the  temp data or something

	            break;

	          case 4:
	            nState = 4;
	            Display_Write_Header(Controller_Get_Warnings(), "Air History", time);
	            Display_Write_Air_Screen();
	            break;
	          case 5:
	            nState = 5;
	            Display_Write_Header(Controller_Get_Warnings(), "Settings Screen", time);
	            Display_Write_Settings_Screen();
	            break;
	        }
			free(time);
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
	          break;
	        case 7:
	          break;
	        case 8:
	          //TODO State 5 move up (select setting above)(also increase)
	          break;
	        case 9:
	          break;
	        case 10:
	          //TODO State 3,4,5 move left (save setting and return to select)
	          break;
	        case 11:
	          //TODO State 5 move down (select setting below)(also decrease)
	          break;
	        case 12:
	          //State 3,4,5 move right (also edit setting and end select)
	          break;
			}
		}
	}
	return 1;
}

char Controller_Get_Warnings(void){
  return nTempWarning+nMemWarning;
}


