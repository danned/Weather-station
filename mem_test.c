#include "mem.h"
#include "rtc.h"
#include "includes/common.h"
#include "includes/at91sam3x8.h"
#include "includes/system_sam3x.h"

#include "peripherals/temp_sensor.h"
#include "peripherals/air_sensor.h"
program_t sta;
//static int meas_count = 0;
extern char cTemp_Reset_Ready_Flag;
extern char cTemp_Measurement_Ready_Flag;

static void tempSens(){
	if(sta.status.TEMP_REQ == 1){
		sta.status.TEMP_REQ = 0;
		Temp_Reset();
	}
	if(cTemp_Reset_Ready_Flag  == 1){
	 	cTemp_Reset_Ready_Flag = 0;
		Temp_Read();
	}
	if(cTemp_Measurement_Ready_Flag == 1){
		cTemp_Measurement_Ready_Flag = 0;
		sta.temp_sum_f += Temp_Get();
		mem.temp->count++;
		//printf("%f",temp);
	}

}

void main(void) {
	SystemInit();
	Temp_Init();
	AIRSENS_init();
	MEM_init();
	SysTick_Config(84000); // config systick to interrupt w/ 1 interrupt/ms
	sta.FAST_MODE = 1;
	sta.state = 0;
	sta.n_avg = 3;
	sta.temp_sum_f = 0;
	sta.fast_count = 0;
	sta.status.MEAS = 0;
	sta.status.TEMP_REQ = 0;
	
	
	
	RTC_Init(50, 00, 14, 20, 15, 01, 18, 7);
	while(1){
		tempSens();
		if(sta.status.MEAS == 1){
			sta.status.MEAS = 0;
			if(mem.temp->count>0){
				MEM_save((sta.temp_sum_f/mem.temp->count), AIRSENS_getPres() );
				sta.temp_sum_f = 0;
				mem.temp->count = 0;
			}
		}
		if(sta.status.NEW_DAY){
			sta.status.NEW_DAY = 0;
			MEM_newDay();
			//meas_count = 0;
		}
	}
}

void RTC_Handler(void){

	
	
	if( (*AT91C_RTC_TIMR&0xFF) == 0 ){
		if(!sta.FAST_MODE){
			sta.status.MEAS = 1;
		}else if(sta.FAST_MODE){
			if(sta.fast_count > 3){
				sta.fast_count = 0;
				sta.status.NEW_DAY = 1;
			}else{
				sta.fast_count++;
			}
		}
	}
	
	if( ( *AT91C_RTC_SR&AT91C_RTC_SECEV ) > 0){
		sta.status.MEAS = 1;
	}
	
	if(*AT91C_RTC_TIMR&(0x7F<<16) == 0){
		sta.status.NEW_DAY = 1;
	}
	
	

	*AT91C_RTC_SCCR = 3<<1;
	//printf("RTC Interrupt!");
}

void SysTick_Handler(void){
	static int meas_count = 0;
	meas_count++;
	if(sta.FAST_MODE){//fast mode
		if(meas_count > 300){
			sta.status.TEMP_REQ = 1;
			
			meas_count = 0;
		}
	}else if(!sta.FAST_MODE){//normal mode
		if(meas_count > 19999){
			
			sta.status.TEMP_REQ = 1;
			meas_count = 0;
		}
	}
}