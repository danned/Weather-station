
#include <stdio.h>
#include <stdlib.h>
#include "includes/at91sam3x8.h"
#include "includes/system_sam3x.h"
#include "includes/common.h"
#include "peripherals/display.h"
#include "peripherals/keypad.h"
#define START_DELAY 25
#define SETUP_DELAY 840
#define BIT_0_AND_2 0x5
#define BIT_17_AND_18 (6<<16)

int32_t nResetCounter = -1; // -1 equals reset not active
int32_t nMsCounter = 0; //counts ms
char cResetFlag = 0;  //reset flag is 1 when restart should start. else 0
char cTemp_Ready = 0;
char cLight_Measure_Start = 0;
char cLight_Measure_Ready = 0;
int32_t nTimeDiff = 0;


void Start_Temp_Reset( void );
void SysTick_Handler( void );
void Temp_Start_Measure(void);
void Pio_Init(void);
void Timer_Init(void);
void Light_Sensor_Init(void);
void Servo_Init( void );
void TC0_Handler(void);
float Conv_Celsius(int ticks);
void ADC_Handler(void);
void Light_Sensor_Start_Measure(void);


int main()
{
  SystemInit();
  SysTick_Config(84000); // config systick to interrupt w/ 1 interrupt/ms
  Pio_Init();
  Timer_Init();
  Display_Init();
  Display_Set_Default_State();
  Light_Sensor_Init();
  Servo_Init();
  Keypad_Init();
  
  char *text = malloc(sizeof(char *));
  
  //Display_Write( "Hej hej", 0,0 ); 
  while(1){
    //printf("PIOA2 = %d\n" ,(*AT91C_PIOA_PDSR && (1<<2)));
    unsigned char pressed = Keypad_Read(); 
    if(pressed != 0){
      if(pressed != 12){
        //*AT91C_PWMC_CH2_CDTYR = (int)1838.0*(1.0+((1.0/9.0)*(float)pressed));
        *AT91C_PWMC_CH2_CDTYR = 1838+233*pressed;
       // printf("\n Val: %d \n",(int)1838.0*(1+(0.09*(float)pressed)));
        printf("%d",pressed);
        pressed = 0;
      }
      else{
         *AT91C_PWMC_CH2_CDTYR = 1838;
      }

    }
    
    
    //start reset
    if(cResetFlag == 1){
      Start_Temp_Reset();
    }
    
 
      
    if(cTemp_Ready == 1){

      uint32_t nTimeA = *AT91C_TC0_RA;
      uint32_t nTimeB = *AT91C_TC0_RB;
      uint32_t nTimeDiff = nTimeB - nTimeA;
      cTemp_Ready = 0;
      char *text = malloc(sizeof(char *));
      sprintf(text, "%.2f ", Conv_Celsius(nTimeDiff));// TODO do not use sprintf
      //printf(text);
      Display_Write(text,100,0);
      //Display_Write(text,0,0);
      //printf("Counter val: %f\n",Conv_Celsius(nTimeDiff));
      nTimeDiff = 0;
    }
    //wait for flag
    if(nResetCounter> 15){
      Temp_Start_Measure();
    }
	
    if(cLight_Measure_Start){
        cLight_Measure_Start = 0;
        Light_Sensor_Start_Measure();
    }
    
    //check if light reading is ready
     if(cLight_Measure_Ready == 1){
      cLight_Measure_Ready = 0;
      int reading = *AT91C_ADCC_LCDR;
      //convert from 12-bit value to volt
      float voltage = (3.3/(0xFFF))*reading;
      
      sprintf(text, "%.2f ", voltage);// TODO do not use sprintf
      Display_Write(text,160,0);
      
    }
    
    
   }
  
  return 0;
}


//init analog pin 7 on arduino pin 2 on sam3
/*void Pio_Init(void){
  *AT91C_PMC_PCER = (3<<11);
  *AT91C_PIOA_PER = (1<<2);
  *AT91C_PIOA_ODR = (1<<2);
  *AT91C_PIOA_CODR = (1<<2); //We want low as default state
  
}*/

//init timer counter PID 27
void Timer_Init(void){
  *AT91C_PMC_PCER = (1<<27); //enable clock
  //*AT91C_PIOB_PDR = (1<<25); //paralell io disable
  //*AT91C_PIOB_ABSR = (0xF<<25); //Peripheral B enable
  *AT91C_TC0_CMR = 0x0; //set register to 0 to choose clock
  
  
  //NVIC enable for timer
  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 4);
  NVIC_EnableIRQ(TC0_IRQn);
  
  //prepare RegA:16,17 and RegB:18,19 for falling and rising edge
  *AT91C_TC0_CMR = (1<<7)|BIT_17_AND_18; //Disabling clock when RB loads etc.
  *AT91C_TC0_CCR = 1; //clear and enable timer
  
  
}

void Pio_Init(void){
  *AT91C_PMC_PCER = (1<<12);
  *AT91C_PIOB_PER = (1<<25);
  *AT91C_PIOB_ODR = (1<<25);
  *AT91C_PIOB_CODR = (1<<25); //We want low as default state
  
}

void Start_Temp_Reset( void ){
  //start reset counter and reset flag
  *AT91C_TC0_CCR = (1);
  nResetCounter = 0;
  cResetFlag = 0;
  *AT91C_PIOB_OER = (1<<25);
  
 
}





void Temp_Start_Measure( void ){

  nResetCounter = -1; //disable reset counter
 *AT91C_PIOB_ODR = (1<<25);
//TODO is this necessary?
 //while((*AT91C_PIOB_PDSR && (1<<25)) == 0){}
 Delay(SETUP_DELAY);
 //Start pulse assuming low
 *AT91C_PIOB_OER = (1<<25);
 //start interrupts
 *AT91C_TC0_IER = (1<<6);
 *AT91C_TC0_CCR = (1<<2); //clear timer 
 
 Delay(START_DELAY); //Wait for start pulse to finish
 
 *AT91C_PIOB_ODR = (1<<25); //High again and wait for response

}



//When second timer register ges value
void TC0_Handler(void){
    //disable interrupts
    *AT91C_TC0_IDR = (1<<6);
    //flag temp reading finished
    cTemp_Ready = 1;
   // *AT91C_TC0_CCR = 2; //disable timer
}

void SysTick_Handler( void ){
  nMsCounter++;
  if(nResetCounter >= 0){
    nResetCounter++;
  }
  if(nMsCounter > 1000){
    cResetFlag = 1;
    nMsCounter = 0;
    cLight_Measure_Start = 1;
  }
}



float Conv_Celsius(int ticks){
  float result = ((float) ticks / (42*5) - 273.15); //(((float)ticks/42)/5)-273.15;
  return result;
}


/*Light sensor temp code*/

void Light_Sensor_Init(void){
  //Init clock ADC
  *AT91C_PMC_PCER1 = (1<<5);
  
   //Config adc
   *AT91C_ADCC_MR = (2<<8); //prescale clock to 14mhz
   *AT91C_ADCC_CR = (1);
  NVIC_ClearPendingIRQ(ADC_IRQn);
  NVIC_SetPriority(ADC_IRQn, 5);
  NVIC_EnableIRQ(ADC_IRQn);
}

/*Light sensor temp code*/

void Light_Sensor_Start_Measure(void){
   
  *AT91C_ADCC_CHER = (1<<1); //channel enable AD1
  *AT91C_ADCC_CR = (2); //channel start conversion
  *AT91C_ADCC_IER = (1<<24);
  //*AT91C_ADCC_IER = (1<<1);
  
}

void ADC_Handler(void){
  cLight_Measure_Ready = 1;
  *AT91C_ADCC_IDR = (1<<24);
}

void Servo_Init( void ){
   *AT91C_PMC_PCER1 =   (1<<4);
   *AT91C_PMC_PCER =   (1<<11);
   *AT91C_PIOA_ABMR = (1<<20);
   //*AT91C_PIOB_ABSR =   (1<<20);
   *AT91C_PIOA_PDR  =   (1<<20);
   *AT91C_PWMC_ENA   =   (1<<2);
   *AT91C_PWMC_CH2_CMR = AT91C_PWMC_CPRE_MCK_DIV_32;
   *AT91C_PWMC_CH2_CPRDR = 52500;// set to 20 ms
   //*AT91C_PWMC_CH2_CDTYR = 2625;// set to 1 ms;
   //*AT91C_PWMC_CH2_CDTYR = (int)2625*0.7;
   //*AT91C_PWMC_CH2_CDTYR = (int)2625*1.5;
   *AT91C_PWMC_CH2_CDTYR = (int)2625*2.3;
   
}