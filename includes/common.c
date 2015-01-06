#include "common.h"
#include "../includes/at91sam3x8.h"

void Delay(int Value){
  for(int i=0;i<Value;i++)
    asm("nop");
}





