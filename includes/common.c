#include "common.h"
#include "../includes/at91sam3x8.h"

void Delay(int Value){
  int i;
  for(i=0;i<Value;i++)
    asm("nop");
}





  