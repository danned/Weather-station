#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "../mem.h"
#include <stdlib.h>
/*------PUBLIC------*/
/* prints string at coordinate null terminated*/
void Display_Write(char *text, char x, char y);

/* initializes clock and pio for display */
void Display_Init(void);

/* sets display to default state */
void Display_Set_Default_State(void);

/* returns display status. 3 is ready to write */
unsigned char Display_Read_Status(void);

/* sends data to display */
void Display_Write_Command(unsigned char Data);
void Display_Write_Home_Screen(char* temp, char* lux, char* air, char* date);
void Display_Write_Light_Screen(void);
void Display_Write_Temp_Screen(char* date);
void Display_Write_Air_Screen(char* date);
void Display_Write_Settings_Screen(void);
void Display_Write_Date_Screen(void);
void Display_Write_Testing_Screen(char temp_pass,char air_pass,char light_pass,char mem_pass);
/* sends data to display */
void Display_Write_Data(unsigned char Data);
void Display_Write_Datetime_Screen(void);
/*Fills a single pixel*/
void Display_Draw_Pixel(int x, int y);
void Display_Draw_Sun(int xw, int yw, int rw);
void Display_Draw_Arc(int xw, int yw, int rw);
void Display_Draw_Axis(void);
void Display_Draw_Temp_Graph(temp_t* data, char count);
void Display_Draw_Air_Graph(void);
/*Empties graphics memory, EXPENSIVE!*/
void Display_Clear_Graphics();

void Display_Write_Header(char error_code,char* title,char* time);
void Display_Write_Sidebar();
void Display_Draw_Borders(void);

/* clears display */
void Display_Clear_Text(void);
#endif // _DISPLAY_H_
