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
#include "mem.h"
#include <stdlib.h>
#include <stdio.h>
#include "peripherals/display.h"
#include "peripherals/temp_sensor.h"
#include "peripherals/air_sensor.h"
#include "includes/at91sam3x8.h"
#include "includes/common.h"
#include "rtc.h"
extern signed char cLight_Sensor_State;
static int cur_week = 0;
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
				AIRSENS_init();
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
		        if(sta.state == 3){
					char count = 0;
					cur_week++;
			        temp_t* temp = mem.temp;
					 while(temp != NULL && count <cur_week*7 ){
					     count++;
					     temp = temp->next;
					}

					 count = 0;
					 //Get last 7 days worth of data from database
					 while(temp != NULL && count <7 ){
					     Display_Draw_Temp_Graph(temp, count);
					     count++;
					     temp = temp->next;
					}
				}
	          break;
	        case 8:
	          if(sta.state == 3){
					char count = 0;
					cur_week--;
			        temp_t* temp = mem.temp;
					 while(temp != NULL && count <cur_week*7 ){
					     count++;
					     temp = temp->next;
					}

					 count = 0;
					 //Get last 7 days worth of data from database
					 while(temp != NULL && count <7 ){
					     Display_Draw_Temp_Graph(temp, count);
					     count++;
					     temp = temp->next;
					}
				}
	          break;
	        case 9:
	        	if(sta.state == 5){ //If we are in settings screen
			  Display_Write("_ ",1,1);
			  Delay(4000000);
			  char l_val = 0;
			  char h_val = 0;
			  char entries_done = 0;
			  //Display_Write("_",94+((entries_done%2)*40),0);
			  unsigned char pressed;
			  while(entries_done < 4){
				  pressed = Keypad_Read();
			    if(pressed != 0){

			    	switch(pressed){
			        case 1:
			          Display_Write("1",1+((entries_done%2)*40),1); //write the number at correct place
			            entries_done++;
					    break;
			        case 2:
			          Display_Write("2",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
					    break;
			        case 3:
			          Display_Write("3",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 4:
						   Display_Write("4",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			          entries_done++;
			        break;
			        case 5:
						Display_Write("5",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 6:
						Display_Write("6",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 7:
						Display_Write("7",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 8:
						Display_Write("8",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 9:
						Display_Write("9",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 11:
						Display_Write("0",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;

			        break;
			    }

				if(!(pressed == 10 || pressed == 12)){
					if(pressed == 11){pressed = 0;} //Quickfix to make saving easier

					//Find out if its cent year month or date user is entering
					switch((entries_done-1)/2){
					  case 0:
						l_val = (l_val*10)+pressed;
					  break;
					  case 1:
						h_val = (h_val*10)+pressed;
					  break;
					}
				}


			    //Press star to move to next item
			  while(Keypad_Read() != 10){}
			  Display_Write("_",1+((entries_done/2)*40)+(entries_done%2),1);
				Delay(2000000);
			    }
			  }
			  sta.alm_h = h_val;
			  sta.alm_l = l_val;

			}
	          break;
	        case 10:

		        if(sta.state == 2){cLight_Sensor_State = 0;int a = LightFollow();}//If we are in light follower screen
		        //Load test data
			    if(sta.state == 5){ //If we are in settings screen
					Display_Write("Loading data:",112,1);
					Display_Write("[          ]",152,1);

		        	for (int i = 0; i < 10; i++)
		        	{
		        		Display_Write("~",153+i,1);
						Delay(2000000);
						//MEM_init();
						    MEM_save(25.5, 100000);
						    MEM_save(23.3, 102000);
						    MEM_newDay();
						    MEM_save(10, 140000);
						    MEM_save(12, 145000);
						    MEM_newDay();
						    MEM_save(-2, 90000);
						    MEM_save(-5, 100000);
							MEM_newDay();
						    MEM_save(-2, 90000);
						    MEM_save(-5, 100000);
							MEM_newDay();
						    MEM_save(20, 80000);
						    MEM_save(15, 90000);
							MEM_newDay();
						    MEM_save(15, 103000);
						    MEM_save(16, 102000);
							MEM_newDay();
						    MEM_save(20, 90000);
						    MEM_save(13, 100000);
							MEM_newDay();
						    MEM_save(12, 90000);
						    MEM_save(11, 100000);
							MEM_newDay();
						    MEM_save(10, 90000);
						    MEM_save(9, 100000);
							MEM_save(11,90000);
							MEM_save(12,90000);
							MEM_save(13,90000);
							MEM_save(14,90000);
							MEM_save(15,90000);
							MEM_save(16,90000);
							MEM_save(17,90000);
		        	}
		        	Display_Write("Completed.  ",152,1);

				}


	          break;
	        case 11:
	          if(sta.state == 5){

			  Display_Write("_ ",90,0);
			  unsigned char pressed;
			  while(1){
				pressed = Keypad_Read();
				if(pressed != 0 && pressed < 11){
					char *pressed_str = malloc(3*sizeof(char *));
					if(pressed_str == 0){
					  return 0;
					}
			    	sprintf(pressed_str, "%d", pressed);// Populate string
					Display_Write(pressed_str,90,0);
					free(pressed_str);
					sta.n_avg = pressed;
					Delay(4000000);
					break;
				}
			  }
	          break;
	        case 12:
	          if(sta.state == 2){cLight_Sensor_State = -1;}
	          if(sta.state == 5){
	          	Display_Write("_         ",174,0);
	          	if(sta.mode > 0){
	          		sta.mode = 0;
	          		Display_Write("DISABLED",174,0);
	          	}else{
	          		sta.mode = 1;
	          		Display_Write("ENABLED",174,0);
	          	}
	          	Delay(2000000);

	          }
	          break;
			}
		}
	}
	return 1;
}
}
char Controller_Get_Warnings(void){
  return nTempWarning+nMemWarning;
}


