#ifndef _COMMON_H_
#define _COMMON_H_
/*----------------------------------------------------------------
 * Below are common used bitmasks
 *----------------------------------------------------------------*/
#define BIT_2_TO_9 (BIT_2_3_4_5 | BIT_6 | BIT_7_8_9)
#define BIT_6 ((0x1)<<6)
#define BIT_12 ((0x1)<<12)
#define BIT_13 ((0x1)<<13)
#define BIT_7_8_9 ((0x7)<<7)
#define BIT_2_3_4_5 ((0xF)<<2)
#define BIT_12_TO_17 ((0x3F)<<12)
#define BIT_18_19 (3<<18)
#define BIT_17_AND_18 6<<16
#define TRUE 1
#define FALSE 0
/*----------------------------------------------------------------
 * End of bitmasks
 *----------------------------------------------------------------*/
//extern char cLight_Sensor_State = -1;
extern float fTemp_Sum;		// Can probably be local variables
extern int iN_Avg;			// in main program
extern int MODE;
extern void Delay(int Value);

typedef struct{
	char MEAS : 1;
	char TEMP_REQ : 1;
	char NEW_DAY : 1;
}status_t;

typedef struct{
	status_t status;
	char mode;
	char state;
	int n_avg;
	float temp_sum_f;
}program_t;

extern program_t sta;
#endif // _COMMON_H_
