/*display.h - Staffan Piledahl
Display driver for RA6963 Dot Matrix LCD Controller. 
Note that init function must be modified to use screens of different sizes.
Screen configured as 40 cells wide as standad (with 16 rows).
Note that it uses an octal transciever to multiplex the screens 8 data lines. 

Performance: Each pixel or character being written to the display blocks the code for some time.
Until Screen memory switch is implemented, beware!  

Blocks used: PIOC, PIOD

------- Usage example: -----
DISPLAY_init(void)
DISPLAY_write("Hello1", 0, 0) //Writes Hello starting at cell number 0 (upper left)
DISPLAY_write("Hello2", 40, 0) //Writes one line below
DISPLAY_write("Hello3", 0, 1) //Writes Hello at cell number 256 (middle somewhere)

DISPLAY_drawPixel(15, 15); //Fills single pixel at cooridnates
...
DISPLAY_clearGraphics(); //expensive!
DISPLAY_clearText();
---------------------------
TODO:
- Load static parts of screens into display memory and just switch beteween memory places.
- Create proper writeTextAt()
- Remove dependencies!
*/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "../mem.h"
#include <stdlib.h>


typedef struct{
	unsigned short int counter;
}display_t;

extern display_t disp;
/************************************************************************/
/* Basic drivers (Single screen mem mode)			                    */
/************************************************************************/
/* prints string at coordinate null terminated*/
void DISPLAY_write(char *text, char x, char y);
/* initializes clock and pio for display */
void DISPLAY_init(void);
/*Empties graphics memory, EXPENSIVE!*/
void DISPLAY_clearGraphics();
/* clears display from text*/
void DISPLAY_clearText(void);
/*Fills a single pixel*/
void DISPLAY_drawPixel(int x, int y);

/************************************************************************/
/* Private - Internal use only										    */
/************************************************************************/
/* sends command to display */
void DISPLAY_writeCommand(unsigned char Data);
/* sends data to display */
void DISPLAY_writeData(unsigned char Data);
/* sets display to default state */
void DISPLAY_setDefaultState(void);
/* returns display status. 3 is ready to write */
unsigned char DISPLAY_readStatus(void);

/************************************************************************/
/* Application specific helpers									        */
/************************************************************************/
/*Header sidebar*/
void DISPLAY_writeHeader(char error_code,char* title,char* time);
void DISPLAY_writeSidebar(void);

/*Pre made Screens*/
void DISPLAY_writeHomeScreen(char* temp, char* lux, char* air, char* date);
void DISPLAY_writeLightScreen(void);
void DISPLAY_writeTempScreen(char* date);
void DISPLAY_writeAirScreen(char* date);
void DISPLAY_writeSettingsScreen(void);
void DISPLAY_writeDateSetScreen(void);
void DISPLAY_writeTestingScreen(char temp_pass,char air_pass,char light_pass,char mem_pass);
void DISPLAY_writeDatetimeScreen(void);

/*Drawing helpers*/
void DISPLAY_drawSun(int xw, int yw, int rw);
void DISPLAY_drawArc(int xw, int yw, int rw);
void DISPLAY_drawAxis(void);
void DISPLAY_drawTempGraph(mem_temp_t* data, char count);
void DISPLAY_drawAirGraph(void);
void DISPLAY_drawBorders(void);
#endif // _DISPLAY_H_
