/*
See controller.h for info
*/

#include "controller.h"
#include "mem.h"
#include "light_follower.h"
#include <stdlib.h>
#include <stdio.h>
#include "peripherals/display.h"
#include "peripherals/temp_sensor.h"
#include "peripherals/air_sensor.h"
#include "peripherals/light_sensor.h"
#include "peripherals/keypad.h"
#include "includes/at91sam3x8.h"
#include "includes/common.h"
#include "rtc.h"

extern char* time;	//Pointer to the time string that gets updated all the time
static int cur_week = 0; // Keeps track of how many weeks back you have browsed
ctrl_t ctrl;

/* Main flow control structure - takes a pressed key val*/
char CTRL_userInput(char pressed){
		printf("%d", pressed);
		if(pressed != 0){
	      if(pressed <= 5){
			DISPLAY_clearText();
	  		DISPLAY_clearGraphics(); //TODO change this to only clear whats necessary
	        char *date = malloc(10*sizeof(char)); //TODO change from 15
			while(date == 0){
			  MEM_remove();
			  date = malloc(10*sizeof(char)); //TODO change from 15
			}
			//Populate strings with time and date
			RTC_Get_Time_String(time);
			RTC_Get_Date_String(date);
			//First 5 menu options:
	        switch(pressed){
	          case 1:
		        sta.state = 1;
		        //Allocate memory for strings to be used
				char *air = malloc(15*sizeof(char));
				while(air == 0){
				  MEM_remove();
				  air = malloc(15*sizeof(char));
				}
			    char *temp = malloc(15*sizeof(char));
				while(temp == 0){
				  MEM_remove();
				  temp = malloc(15*sizeof(char));
				}
				char *lux = malloc(15*sizeof(char));
				while(lux == 0){
					MEM_remove();
					lux = malloc(15*sizeof(char));
				}

				//Get current temperature and check towards alarm
				float temperature = mem.cur_temp;
				if(temperature >sta.alm_h || temperature <sta.alm_l){
					ctrl.status.TEMP_WARN = 1;
				}
				else{
					ctrl.status.TEMP_WARN = 0;
				}
			    sprintf(temp, "%.2f", temperature);// Populate string

				//Get current light
				int reading = *AT91C_ADCC_LCDR;
			    float voltage = (3.3/(0xFFF))*reading;//convert from 12-bit value to volt to lux
			    float luxfloat = 3000 * (1/(voltage*voltage));//3v = 60lux , 1v 3000 lux 2v = 1000 lux
			    sprintf(lux, "%.2f", luxfloat);// Populate string
				AIRSENS_init();
			 	//Get current Air pressure
			    int air_reading = AIRSENS_getPres();
			    sprintf(air, "%d", air_reading);// Populate string

				//Write to display and finish
		        DISPLAY_writeHeader(CTRL_getWarnings(), "Home screen", time);
		        DISPLAY_writeHomeScreen(temp,lux,air,date);
			    free(lux);
				free(temp);
				free(air);

	            break;

	          case 2:
	            sta.state = 2;
			    DISPLAY_writeHeader(CTRL_getWarnings(), "Sun tracker", time);
			    DISPLAY_writeLightScreen();
			    //TODO Enable tracking mode, also remember to disable on state change somehow
	            break;
	          case 3:
	            sta.state = 3;

	            DISPLAY_writeHeader(CTRL_getWarnings(), "Temp Hist", time);
	            DISPLAY_writeTempScreen(date); //This has to take the  temp data or something

	            break;

	          case 4:
	            sta.state = 4;
	            DISPLAY_writeHeader(CTRL_getWarnings(), "Air History", time);
	            DISPLAY_writeAirScreen(date);
	            break;
	          case 5:
	            sta.state = 5;
	            DISPLAY_writeHeader(CTRL_getWarnings(), "Settings Screen", time);
	            DISPLAY_writeSettingsScreen();
	            break;
	        }
			//free(time);
			free(date);
	        DISPLAY_writeSidebar();
	        pressed = 0;
	      }
	      //State dependent keypad readings
	      else{
		switch(pressed){
	        case 6:


	          break;
	        case 7: //TODO CLEAR GRAPHICS!
		        if(sta.state == 3){
		        	DISPLAY_clearGraphics();
		        	DISPLAY_drawAxis();
		        	DISPLAY_drawBorders();
					char count = 0;
					cur_week++;
			        mem_temp_t* temp = mem.temp;
					 while(temp != NULL && count <cur_week*7 ){
					     count++;
					     temp = temp->next;
					}

					 count = 0;
					 //Get last 7 days worth of data from database
					 while(temp != NULL && count <7 ){
					     DISPLAY_drawTempGraph(temp, count);
					     count++;
					     temp = temp->next;
					}
								char *cur_week_str = malloc(2*sizeof(char));
				while(cur_week_str == 0){
				  MEM_remove();
				  cur_week_str = malloc(2*sizeof(char));
				}
		    	sprintf(cur_week_str, "%2d", cur_week);// Populate string
		    	DISPLAY_write("Weeks ago:               ",87,0);
				DISPLAY_write(cur_week_str,132,0);
				free(cur_week_str);
				Delay(800000);
			}

	          break;
	        case 8: //TODO CLEAR GRAPHICS!
	          if(sta.state == 3 && cur_week >0){
	          	DISPLAY_clearGraphics();
	          	DISPLAY_drawAxis();
	          	DISPLAY_drawBorders();
					char count = 0;
					cur_week--;
			        mem_temp_t* temp = mem.temp;
					 while(temp != NULL && count <cur_week*7 ){
					     count++;
					     temp = temp->next;
					}

					 count = 0;
					 //Get last 7 days worth of data from database
					 while(temp != NULL && count <7 ){
					     DISPLAY_drawTempGraph(temp, count);
					     count++;
					     temp = temp->next;
					}
				char *cur_week_str1 = malloc(3*sizeof(char));
				if(cur_week_str1 == 0){
				  MEM_remove();
				  cur_week_str1 = malloc(3*sizeof(char));
				}
		    	sprintf(cur_week_str1, "%2d", cur_week);// Populate string
		    	DISPLAY_write("Weeks ago:               ",87,0);
				DISPLAY_write(cur_week_str1,132,0);
				free(cur_week_str1);
				Delay(800000);
			}

	          break;
	        case 9:
	        	if(sta.state == 5){ //If we are in settings screen
			  DISPLAY_write("_ ",1,1);
			  Delay(4000000);
			  char l_val = 0;
			  char h_val = 0;
			  char entries_done = 0;
			  //DISPLAY_write("_",94+((entries_done%2)*40),0);
			  unsigned char pressed;
			  while(entries_done < 4){
				  pressed = Keypad_Read();
			    if(pressed != 0){

			    	switch(pressed){
			        case 1:
			          DISPLAY_write("1",1+((entries_done%2)*40),1); //write the number at correct place
			            entries_done++;
					    break;
			        case 2:
			          DISPLAY_write("2",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
					    break;
			        case 3:
			          DISPLAY_write("3",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 4:
						   DISPLAY_write("4",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			          entries_done++;
			        break;
			        case 5:
						DISPLAY_write("5",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 6:
						DISPLAY_write("6",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 7:
						DISPLAY_write("7",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 8:
						DISPLAY_write("8",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 9:
						DISPLAY_write("9",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
			            entries_done++;
			        break;
			        case 11:
						DISPLAY_write("0",1+((entries_done/2)*40)+(entries_done%2),1); //write the number at correct place
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
			  DISPLAY_write("_",1+((entries_done/2)*40)+(entries_done%2),1);
				Delay(2000000);
			    }
			  }
			  sta.alm_h = h_val;
			  sta.alm_l = l_val;

			}
	          break;
	        case 10:
			//If we are in light follower screen
		        if(sta.state == 2){
				  LIGHTSENS_setState(LIGHTSENS_READ_REQ);
				  int a = LightFollow();
				}
		        //Load test data small
			    if(sta.state == 5 && !sta.FAST_MODE){ //If we are in settings screen
					DISPLAY_write("Loading data:",112,1);
					DISPLAY_write("[          ]",152,1);

		        	for (int i = 0; i < 10; i++)
		        	{
		        		DISPLAY_write("~",153+i,1);
						Delay(1000000);
					}
						//MEM_init();
						    MEM_save(35, 98000);
						    MEM_save(15, 102000);
						    MEM_newDay();
						    MEM_save(25, 115000);
						    MEM_save(10, 100000);
						    MEM_newDay();
							MEM_save(10, 95000);
						    MEM_save(15, 102000);
						    MEM_newDay();
						    MEM_save(25, 100000);
						    MEM_save(35, 105000);
						    MEM_newDay();
						    MEM_save(15, 102000);
							MEM_save(20, 100000);
						    MEM_newDay();
						    MEM_save(5, 140000);
						    MEM_save(10, 145000);
						    MEM_newDay();
							MEM_save(1, 110000);
							MEM_save(39, 90000);
						    MEM_newDay();

		        	DISPLAY_write("Completed.  ",152,1);
					Delay(2000000);

				}
						        //Load test data
			    else if(sta.state == 5 && sta.FAST_MODE){ //If we are in settings screen
					DISPLAY_write("Filling mem:",112,1);
					DISPLAY_write("[          ]",152,1);

		        	for (int i = 0; i < 10; i++)
		        	{
		        		DISPLAY_write("~",153+i,1);
						Delay(1000000);
					}
					
					int days = MEM_fill();

					DISPLAY_write("Completed. ",152,1);
					char *days_s = malloc(10*sizeof(char));
					if(days_s == 0){
						MEM_remove();
						days_s = malloc(10*sizeof(char));
					}
					sprintf(days_s,"%d days added",days);
					DISPLAY_write(days_s,192,1);
					Delay(2000000);

				}


	          break;
	        case 11:
	          if(sta.state == 5){

			  DISPLAY_write("_ ",90,0);
			  unsigned char pressed;
			  while(1){
				pressed = Keypad_Read();
				if(pressed != 0 && pressed < 11){
					char *pressed_str = malloc(3*sizeof(char));
					if(pressed_str == 0){
					  MEM_remove();
					  pressed_str = malloc(3*sizeof(char));
					}
			    	sprintf(pressed_str, "%d", pressed);// Populate string
					DISPLAY_write(pressed_str,90,0);
					free(pressed_str);
					sta.n_avg = pressed;
					Delay(4000000);
					break;
				}
			  }
	          break;
	        case 12:
	          if(sta.state == 2){LIGHTSENS_setState(LIGHTSENS_INACTIVE);}
	          if(sta.state == 5){
	          	DISPLAY_write("_         ",174,0);
	          	if(sta.FAST_MODE > 0){
	          		sta.FAST_MODE = 0;
	          		DISPLAY_write("DISABLED",174,0);
	          	}else{
	          		sta.FAST_MODE = 1;
	          		DISPLAY_write("ENABLED",174,0);
	          	}
				RTC_intSetMode(sta.FAST_MODE);
	          	Delay(2000000);

	          }
	          break;
			}
		}
	}
	return 1;
	}
	return 1;
}
char CTRL_getWarnings(void){
  return ctrl.status.TEMP_WARN+(mem.status.MEM_FULL << 1);
}


