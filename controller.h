//controller.h

#include <stdlib.h>
#include <stdio.h>
#include "peripherals/display.h"
#include "peripherals/temp_sensor.h"
#include "rtc.h"

extern char nTempWarning;
extern char nMemWarning;
extern char nState;
char Controller_Get_Warnings(void);
char Controller_User_Input(char pressed);