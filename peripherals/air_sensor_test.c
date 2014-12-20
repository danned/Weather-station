#include "air_sensor.h"
#include <stdio.h>
#include "../includes/system_sam3x.h"

int main2(){
    SystemInit();
    Air_Sensor_Init();
    unsigned char who = Air_Sensor_Read(0xc);
    printf("who am i: %x\n", who);
    unsigned char status = Air_Sensor_Read(0x06);
    printf("Status: %x \n", status);

  while(1){
    printf("Pressure: %d\n", Air_Sensor_Get_Pressure() );
  }
}
