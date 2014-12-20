//controller.h


#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

extern char nTempWarning;
extern char nMemWarning;
extern char nState;
char Controller_Get_Warnings(void);
char Controller_User_Input(char pressed);

#endif // _CONTROLLER_H_
