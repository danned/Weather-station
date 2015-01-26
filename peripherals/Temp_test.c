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
#include "temp_sensor.h"
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"

float TEMP_get(void);
char cTimeToReadTemp;
/* Test full operation*/
signed char mem_temp_test( void ){
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
	      TEMP_reset();
	    }
	    if(temperature.status.RESET_READY == 1){
	      temperature.status.RESET_READY = 0;
	      TEMP_read();
	    }
	    if(temperature.status.READ_READY == 1){
		    temperature.status.READ_READY = 0;
			float answer = TEMP_get();
		    //Check that answer is a decent value
		    if(answer >-5 && answer <50){
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