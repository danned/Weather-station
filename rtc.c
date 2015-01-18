/*
 RTC Time module
*/

#include "rtc.h"
#include "mem.h"
#include <stdio.h>
#include "includes/common.h"
#include "includes/at91sam3x8.h"
#include "includes/system_sam3x.h"

void intStart();
void intSetMode(char mode);
 /* Real time clock - see page ~256 of atmel doc when working with this
Cent is 19 or 20, year is for example 15, day is 1-7 weekday. date is 1-31
Usage: Set values in natural BCD numbers. for example highest value in sec and min is 59
1. Pause counting of both time and date
2. Time: First 7 bits is second, 1 empty bit, next 7 bits are minute, 1 empty, next 6 bits hour, and the next bit is AM/PM
3. Date: First 7 bits is century, 1 empty, next 7 bits are year, next 5 is month, next 3 is day, next six is date
*/
void RTC_Init(char sec, char min, char hr, char cent, char year, char month, char date, char day){
  //Stop counting with upd_cal and upd_tim to be able to set datetime
  *AT91C_RTC_CR = AT91C_RTC_UPDTIM|AT91C_RTC_UPDCAL; //AT91C_RTC_TIMEVSEL_DAY24 if we want trigger every night
  *AT91C_RTC_MR = (0); //Set 24-hr mode, not necessary but added for clarity AT91C_RTC_HRMOD
  Delay(40000);
  *AT91C_RTC_SCCR = 1; //acknowledge clear status now we can set time
  *AT91C_RTC_TIMR = make_BCD_pattern(sec)|(make_BCD_pattern(min)<<8)|(make_BCD_pattern(hr)<<16); //
  *AT91C_RTC_CALR = (make_BCD_pattern(cent))|(make_BCD_pattern(year)<<8)|(make_BCD_pattern(month)<<16)|(make_BCD_pattern(date)<<24)|(make_BCD_pattern(day)<<21);
  *AT91C_RTC_CR = 0; //Start counting


	//Interupt_RTC_Set_Mode();
	intStart();
}

void intStart(){
	
	intSetMode(1);
	NVIC_ClearPendingIRQ(RTC_IRQn);
  	NVIC_SetPriority(RTC_IRQn, 7);
  	NVIC_EnableIRQ(RTC_IRQn);
}
/************************************************************************/
/* 0 = normal, 1 = fast                                                 */
/************************************************************************/
void intSetMode(char mode){
	*AT91C_RTC_IDR = 0x1f; // disable all interrupts
	if((mode) == 1){
		*AT91C_RTC_IER = 1<<2; //enable second periodic interrupt
		*AT91C_RTC_TIMALR = 1<<7;// set minute interrupt on time interrupt
		*AT91C_RTC_IER = 1<<1;
		//*AT91C_RTC_TIMALR = 1<<15;
	}else if(mode == 0){
		*AT91C_RTC_TIMALR = 1<<23|1<<7;// set hour and minute interrupt on time interrupt
		*AT91C_RTC_IER = 1<<1;
	}//wrong mode do nothing
}

//NOTE: Only small numbers <100
char make_BCD_pattern(char val){
 	char result =0;
	result = ((val / 10) << 4) | (val % 10); //shift "tens" left 4 steps
  	return result;
}

//Nothing larger than 100!
char reverse_BCD_pattern(char val){
	char result = 0;
	// get the dec bcd value
	//printf("The val: %d \n", val);
	result = ((val >> 4)*10) + (val & 0x0F);
	//printf("Tens: %d \n", (val >> 4)*10);
	//printf("Singles: %d \n", (val & 0x0F));
	//printf("Reversed: %d \n", result);
	return result;
}
/*RTC Get current time or date in raw BCD format*/
int RTC_Get_Time(void){
 return *AT91C_RTC_TIMR;

}
int RTC_Get_Date(void){
 return *AT91C_RTC_CALR;
}

/*
Returns the string of the entire time
  time = malloc(sizeof(char *));
and free(time); after function call
*/
void RTC_Get_Time_String(char* time){
  int bcd_time = RTC_Get_Time();
  int mask = 0x00;

  mask = 0x7F;
  char sec = (mask & (bcd_time));
  char min = (mask & (bcd_time >> 8));
  mask = 0x3F;
  char hr = (mask & (bcd_time >> 16));
  //printf("Hr: %d\n",hr );
  //printf("min: %d\n",min );
  //printf("sec: %d\n",sec );
  sprintf(time, "Time: %d:%d:%d \n", reverse_BCD_pattern(hr),reverse_BCD_pattern(min),reverse_BCD_pattern(sec));// TODO do not use sprintf
  	//printf("%s\n", time);
}

//TODO FOR STAEF NEEDS DOCUMENTATION OR REWRITE BADLY
void RTC_Get_Date_String(char* date){
  int bcd_date = RTC_Get_Date();
  int mask = 0x00;

  mask = 0x7F;
  char cent = (char)(mask & (bcd_date));
  mask = 0xFF;
  char year = (char)(mask & (bcd_date >> 8));
  mask = 0x1F;
  char month = (char)(mask & (bcd_date >> 16));
   mask = 0x3F;
  char date2 = (char)(mask & (bcd_date >> 24));

  sprintf(date, "Date: %d%d-%d-%d \n", reverse_BCD_pattern(cent),reverse_BCD_pattern(year),reverse_BCD_pattern(month),reverse_BCD_pattern(date2));// TODO do not use sprintf
}

datestamp_t RTC_getDate(){
  int bcd_date = RTC_Get_Date();
  int mask = 0xFF;
  char year = reverse_BCD_pattern( (char)(mask & (bcd_date >> 8)) );
  mask = 0x1F;
  char month = reverse_BCD_pattern( (char)(mask & (bcd_date >> 16)) );
   mask = 0x3F;
  char date2 = reverse_BCD_pattern( (char)(mask & (bcd_date >> 24)) );

	datestamp_t newStamp;
	newStamp.date =  date2;
	newStamp.month = month;
	newStamp.year =  year;
	return newStamp;

}

void RTC_Get_Day_String(char* day){
  int bcd_date = RTC_Get_Date();
  int mask = 0x00;

  mask = 0x7F;
  char dayz = (char)(mask && (bcd_date >> 21));
sprintf(day, "Day: %d\n", reverse_BCD_pattern(dayz));// TODO do not use sprintf
}



