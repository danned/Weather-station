#include "../includes/at91sam3x8.h"
#include "../includes/common.h"

#define LONG_DELAY 100
#define SHORT_DELAY 20



/* initializes clock and pio for display */
void Display_Init(void);


/* sets display to default state */
void Display_Set_Default_State(void);


/* returns display status. 3 is ready to write */
unsigned char Display_Read_Status(void);

/* sends data to display */
void Display_Write_Command(unsigned char Data);

/* sends data to display */
void Display_Write_Data(unsigned char Data);

/* clears display */
void Clear_Display(void);

/* prints string at coordinate null terminated*/
void Display_Write(char *text, char x, char y);


