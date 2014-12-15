#include "display.h"
#include <stdio.h>


/* Initializes PIO for display */
void Display_Init(void){

  /*  enables clock for PIOC & PIOD */
  *AT91C_PMC_PCER = (3<<13);
  /*enables control of pins*/  
  *AT91C_PIOC_PER =  (BIT_2_TO_9);
  
  /* initializes PIOD pin 2 and 0*/
  *AT91C_PIOD_PER = (1<<2)  | 1;
  *AT91C_PIOD_OER = (1<<2)  | 1;
  *AT91C_PIOD_SODR = (1<<2) | 1;
  
  /* initializes PIO port 2 3 4 5 as input*/
  *AT91C_PIOC_ODR = BIT_2_3_4_5;
  
    /* initializes PIO pin 12-17 and 18,19 as output*/
  *AT91C_PIOC_OER = BIT_12_TO_17 | BIT_18_19;
  
  /* Disable displaycontrol as standard */
  *AT91C_PIOC_SODR = (1<<12);

}

/*
    sets display to default state
*/
void Display_Set_Default_State(void){
  *AT91C_PIOD_CODR = 1;         //Clear Reset display
  Delay(LONG_DELAY);            //LONG DELAY <------------------
  *AT91C_PIOD_SODR = 1;         //Set Reset display
  //*AT91C_PIOC_CODR = 5<<14;
  Display_Write_Data(0x00);
  Display_Write_Data(0x00);
  Display_Write_Command(0x40);//Set text home address
  Display_Write_Data(0x00);
  Display_Write_Data(0x40);
  Display_Write_Command(0x42); //Set graphic home address
  Display_Write_Data(0x1e);
  Display_Write_Data(0x00);
  Display_Write_Command(0x41); // Set text area
  Display_Write_Data(0x1e);
  Display_Write_Data(0x00);
  Display_Write_Command(0x43); // Set graphic area
  Display_Write_Command(0x80); // text mode
  Display_Write_Command(0x94); // Text on graphic off
  //set cusor to upper left
  Display_Write_Data(0x00);
  Display_Write_Data(0x00);
  Display_Write_Command(0x24);
  //clear screen
  Clear_Display();
  //set cusor to upper left
  Display_Write_Data(0x00);
  Display_Write_Data(0x00);
  Display_Write_Command(0x24);
 
  //clear pin 45 and 44 for font select and fr
  *AT91C_PIOC_CODR = BIT_18_19;
}

/* Read display status*/
unsigned char Display_Read_Status(void){
  unsigned char temp;
     
   *AT91C_PIOC_ODR =  BIT_2_TO_9; //set databus as input 
   *AT91C_PIOC_SODR =  BIT_13 | BIT_12;    //Set direction as input  and activate chip
   //*AT91C_PIOC_CODR =  BIT_12;   
 
   /* clear read */
   *AT91C_PIOC_SODR = (1<<17);     // set data
   *AT91C_PIOC_CODR  =3<<15;       // clear CS and rd
   //Delay
    Delay(SHORT_DELAY);
   //Read and save
    temp = (*AT91C_PIOC_PDSR);
    temp &= (3<<1);
    *AT91C_PIOC_SODR  =3<<15;        // set CS and rd
  //deactivate
   *AT91C_PIOC_SODR =  BIT_12;
   //Set Direction output
   *AT91C_PIOC_CODR =  BIT_13;
   return (temp>>1);
}

/* sends data to display */
void Display_Write_Data(unsigned char Data){
  while(Display_Read_Status() != 3){
    Delay(SHORT_DELAY);
  }
  /* clear pins for output*/
  *AT91C_PIOC_CODR = BIT_2_TO_9;
  /* set databus to Data */
  *AT91C_PIOC_SODR = Data<<2;
  
  /* set direction on chip to output */
  *AT91C_PIOC_CODR =  BIT_13 | BIT_12;
  //*AT91C_PIOC_CODR =  BIT_12;    // enabling 74 chip
  
  *AT91C_PIOC_OER =  BIT_2_TO_9; // enable output on pins
  
  *AT91C_PIOC_CODR = 0xD<<14;
 /* *AT91C_PIOC_CODR = (1<<17);     // clear data
  *AT91C_PIOC_CODR  = 1<<16;        // clear CS
  *AT91C_PIOC_CODR = 1<<14;       // and WR*/
  Delay(SHORT_DELAY);             //SHORT DELAY
 // printf("datainput: %d", Data);
  *AT91C_PIOC_SODR = 0x5<<14;      // set CS  and wr
  
  *AT91C_PIOC_SODR =  BIT_12;     // disable 74 chip
  
  *AT91C_PIOC_ODR =  BIT_2_TO_9;  // enable input on pins
}

/* sends command to display */
void Display_Write_Command(unsigned char Command){
  while(Display_Read_Status() != 3){
    Delay(SHORT_DELAY);
  }
  /* clear pins for output */
  *AT91C_PIOC_CODR = BIT_2_TO_9;
  /* set databus to command */
  *AT91C_PIOC_SODR = Command<<2;
  
  /* set direction on chip to output */
  *AT91C_PIOC_CODR =  BIT_13 | BIT_12; // set direction to output and enabling 74 chip
  //*AT91C_PIOC_CODR =  BIT_12;    
  
  *AT91C_PIOC_OER =  BIT_2_TO_9; // enable output on pins
  
  *AT91C_PIOC_SODR = (1<<17);     // set data
  *AT91C_PIOC_CODR = 0x5<<14;     //clear CS and WR
  Delay(SHORT_DELAY);             //SHORT DELAY
 // printf("Command: %d\n", Command);
  *AT91C_PIOC_SODR = 0x5<<14;     //set CS and WR
  
  *AT91C_PIOC_SODR =  BIT_12;    // disable 74 chip
  
  *AT91C_PIOC_ODR =  BIT_2_TO_9; // enable input on pins
  
  
}

/* clears display */
void Clear_Display(void){
  for(int i =0;i<480;i++ ){
    Display_Write_Data(0x00);
    Display_Write_Command(0xC0);   
  }
}

/* prints string at coordinate null terminated*/
void Display_Write(char *text, char x , char y){
  
  Display_Write_Data(x);
  Display_Write_Data(y);
  Display_Write_Command(0x24);//Set text coordinates
  
  for(int i = 0; *(text + i) != 0x00 ; i++){
    printf("%c", (*(text+i)-0x20));
    Display_Write_Data( (*(text+i)-0x20) );
    Display_Write_Command(0xC0);
  }
  /*for(int i =0;i<480;i++ ){
    Display_Write_Data(0x21);
    Display_Write_Command(0xC0);   
  }*/
}

