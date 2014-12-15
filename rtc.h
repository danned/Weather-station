/*
 RTC Time module header
*/
#include "includes/at91sam3x8.h"
#include "includes/system_sam3x.h"
#include "includes/common.h"


char make_BCD_pattern(char val);
char reverse_BCD_pattern(char val);
void RTC_Init(char sec, char min, char hr, char cent, char year, char month, char date, char day);
int RTC_Get_Date(void);
int RTC_Get_Time(void);
void RTC_Get_Time_String(char* time);
void RTC_Get_Date_String(char* time);
void RTC_Get_Day_String(char* time);
void RTC_Handler(void);