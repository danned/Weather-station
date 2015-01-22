/*controller.h - Staffan Piledahl, Daniel Dahlberg
Driver for air pressure/altitude sensor BMP180 (sparkfun breakout) I2C device.
Application specific drivers for reading air pressure, see example.
Full support read/write to device possible. See device manual for other functionality.

Note: Library includes I2C drivers tailored for this device.

Blocks used: God knows...

------- Usage example: -----
while(1){
   char pressed = Keypad_Read();
   CTRL_userInput(pressed);
}
---------------------------
TODO:
-
*/
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

typedef struct{
	char TEMP_WARN  : 1;
	char MEM_WARN : 1;
}ctrl_status_t;

typedef struct{
	ctrl_status_t status;
}ctrl_t;

extern ctrl_t ctrl;

extern char nState;
char CTRL_getWarnings(void);
char CTRL_userInput(char pressed);

#endif // _CONTROLLER_H_
