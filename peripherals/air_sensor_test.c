#include "air_sensor.h"
#include <stdio.h>
#include "../includes/system_sam3x.h"

int main2(){
    SystemInit();
    AIRSENS_init();
    unsigned char who = AIRSENS_read(0xc);
    printf("who am i: %x\n", who);
    unsigned char status = AIRSENS_read(0x06);
    printf("Status: %x \n", status);

  while(1){
    printf("Pressure: %d\n", AIRSENS_getPres() );
  }
}
