/*Unit tests for temp sensor.
Reutrn of 1 equals passed. return 0 equals unknown fail
1 = Passed
0 = Unknown Fail
-1 = Timeout
-2 = Returned value out of spec

Tests finished:
Works continiously (<48hrs)
Gives correct value at +50 degrees C
Gives correct value at -5 degrees C


*/
#include "Temp_test.h"

float Temp_Get(void);
char cTimeToReadTemp;
extern char  cTemp_Reset_Ready_Flag;
extern char cTemp_Measurement_Ready_Flag;
/* Test full operation*/
signed char Temp_Test( void ){
cTimeToReadTemp = 1; //simulate one systick
    signed char passed = 0;
    register unsigned int timeout_counter = 0;

	while(1){
	    if(timeout_counter > 0x050000){ //This means at least some ms has passed our absolute limit is 100 for systick to not overlap
		    passed = -1; //timeout
		    break;
	    }
	    //----Temperature readings-----
	    if(cTimeToReadTemp == 1){
	      cTimeToReadTemp = 0;
	      Temp_Reset();
	    }
	    if(cTemp_Reset_Ready_Flag == 1){
	      cTemp_Reset_Ready_Flag = 0;
	      Temp_Read();
	    }
	    if(cTemp_Measurement_Ready_Flag == 1){
		    cTemp_Measurement_Ready_Flag = 0;
			float answer = Temp_Get();
		    //printf("Measured temp: %.2f \n",answer);
		    //Check that answer is a decent value
		    if(answer >-20 && answer <60){
			    passed = 1; //passed
			    break;
		    }else{
		      passed = -2; //value out of specc
			    break;
		    }
	    }

	timeout_counter++;
	}
	return passed;
}