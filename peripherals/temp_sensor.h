//temp_sensor.h
#define START_DELAY 25
#define SETUP_DELAY 840
#include "../includes/system_sam3x.h"
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"

/*  */
void Start_Temp_Reset( void );

/* */
void Temp_Start_Measure(void);
/* */
void Temp_Read();
/* initializes clock and pio for temp sensor*/
void Temp_Init(void);
/* */
void TC0_Handler(void);

/*  */
float Conv_Celsius(int ticks);
float Temp_Get(void);
void Temp_Reset(void);