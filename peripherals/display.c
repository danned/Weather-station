/* - Staffan Piledahl
Driver for LCD Display see display.h for more info
*/
#include "display.h"
#include "../rtc.h"
#include <math.h>
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"
#include "keypad.h"
#include "servo.h"
#include <stdio.h>

#define LONG_DELAY 100
#define SHORT_DELAY 20
extern int key_counter;
display_t disp;
/* Initializes PIO for display */
void DISPLAY_init(void){
	disp.counter = 0;
  /* TODO INLINE FUNCTION GOES HERE enables clock for PIOC & PIOD */
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

  /* Set areas etc. */
  DISPLAY_setDefaultState();

}

/*Writes to a cell, needs rewrite to take correct x and y
For help see display.h documentation!
*/
void DISPLAY_write(char *text, char x , char y){

  DISPLAY_writeData(x);
  DISPLAY_writeData(y);
  DISPLAY_writeCommand(0x24);//Set text coordinates

  //Go torugh each char in given string and write it using auto increment
  for(int i = 0; *(text + i) != 0x00 ; i++){
    DISPLAY_writeData( (*(text+i)-0x20) );
    DISPLAY_writeCommand(0xC0);
  }

}

/*Home screen that shows current values of everything*/
void DISPLAY_writeHomeScreen(char* temp, char* lux, char* air, char* date){

    DISPLAY_write(date,87,0); //Already includes wrapping string "Date: "

    DISPLAY_write("Temp: ",167,0);
    DISPLAY_write(temp,172,0);
    DISPLAY_write(" Cels",177,0);

    DISPLAY_write("Illu: ",207,0);
    DISPLAY_write(lux,212,0);
    DISPLAY_write(" Lux",218,0);

    DISPLAY_write("Air: ",247,0);
    DISPLAY_write(air,252,0);
    DISPLAY_write(" kPa",0,1);
}

/*Light follower screen, draws an arc and a sun*/
void DISPLAY_writeLightScreen(void){

  //Write out current servo pos
  DISPLAY_write("Sun position ",95,0);
  int reading = SERVO_getPos();
  reading = (reading)/44; //Turn into angle
  char *angle = malloc(12*sizeof(char));
  while(angle == 0){
    MEM_remove();
	angle = malloc(12*sizeof(char));
  }
  sprintf(angle, "%d degrees", reading);
  DISPLAY_write(angle,128,0);
  free(angle);

	//Draw suntracker painting.
	DISPLAY_drawArc(145, 135, 70);
	DISPLAY_drawSun(80, 110, 10); //TODO make this take the angle from servo
}

/*Temperature log screen, shows last 7 days worth of temp data as graphs*/
void DISPLAY_writeTempScreen(char* date){
  DISPLAY_write(date,87,0);

  //Prepare Graphing
  DISPLAY_drawAxis();
  DISPLAY_write("Tdy",57,2);
  DISPLAY_write("Ytd",61,2);
  DISPLAY_write("40",167,0);
  DISPLAY_write("20",71,1);
  DISPLAY_write("0",16,2);

  //Fetch last 7 days of temp data
  //datestamp_t todays_datestamp = mem.temp->date; //TODO get date from RTC
  mem_temp_t *tmp = mem.temp;
  char count = 0;
  //Write out in text
  //Create string to write out averages in text
  while(tmp != NULL && count <7 ){
    DISPLAY_drawTempGraph(tmp, count); //Draw bar graph

    //short average = tmp->avg;
    //sprintf(avg_str, "%d", average);
    //DISPLAY_write(avg_str,70+(count*40),0);

    tmp = tmp->next;
    count++;
  }
 // free(avg_str);

}
/*Draws the initial set date screen on startup a lot of user input
The algorithm used to determine which entry user is at is done using integer
diviosn and modulu

TODO: Split and move to Controller
*/
void DISPLAY_writeDateSetScreen(void){

  //Things needed to be filled in by user
  DISPLAY_write("Cent:              ",88,0);
	DISPLAY_write("Year:              ",128,0);
	DISPLAY_write("Month:              ",168,0);
	DISPLAY_write("Date:               ",208,0);
	DISPLAY_write("                    ",248,0);
  unsigned char date_entries_done = 0;
  unsigned char time_entries_done = 0;
  char cent = 0;
  char year = 0;
  char month = 0;
  char date = 0;
  char hr = 0;
  char sec = 0;
  char min = 0;

  /*Let user enter the 8 numbers needed and print them on screen accordingly
  After each number user must confirm and move to next number using star key
  */
  
  unsigned char pressed = 0;
  key_counter = 0;
  
  	char *cent_s = malloc(sizeof(char)*3);
	if(cent_s == 0){
		MEM_remove();
		cent_s = malloc(sizeof(char)*3);
	}
	*cent_s = 0;
	char *year_s = malloc(sizeof(char)*3);
	if(year_s == 0){
		MEM_remove();
		year_s = malloc(sizeof(char)*3);
	}
	*year_s = 0;
	char *month_s = malloc(sizeof(char)*3);
	if(month_s == 0){
		MEM_remove();
		month_s = malloc(sizeof(char)*3);
	}
	*month_s = 0;
	char *date_s = malloc(sizeof(char)*3);
	if(date_s == 0){
		MEM_remove();
		date_s = malloc(sizeof(char)*3);
	}
	*date_s = 0;		
  while(date_entries_done < 8){
		if(key_counter > 300){
			key_counter = 0;
			pressed = Keypad_Read();
		}
	if(((pressed > 0 && pressed < 10) || pressed == 11)){
		
	if(pressed == 11)
		pressed = 0;
	
	switch(date_entries_done){
		case 0:
		case 1:
			cent = (cent*10)+pressed;
			sprintf(cent_s,"%2d",cent);
			date_entries_done++;
			break;
		case 2:
		case 3:
			year = (year*10)+pressed;
			sprintf(year_s,"%2d",year);
			date_entries_done++;
		break;
		case 4:
		case 5:
			month = (month*10)+pressed;
			sprintf(month_s,"%2d",month);
			date_entries_done++;
			break;
			case 6:
			case 7:
			date = (date*10)+pressed;
			sprintf(date_s,"%2d",date);
			date_entries_done++;
		  break;
	}
	DISPLAY_write(cent_s, 94 ,0);
	DISPLAY_write(year_s, 134,0);
	DISPLAY_write(month_s, 174,0);
	DISPLAY_write(date_s, 214,0);
	pressed = 0;
    }
  }
  free(cent_s);
  free(year_s);
  free(month_s);
  free(date_s);
  /*Same procedure for time*/
  DISPLAY_write("Hr:                ",88,0);
  DISPLAY_write("Min:               ",128,0);
  DISPLAY_write("Sec:              ",168,0);
  DISPLAY_write("                   ",208,0);
  DISPLAY_write("                    ",248,0);
  char *hr_s = malloc(sizeof(char)*3);
	if(hr_s == 0){
		MEM_remove();
		cent_s = malloc(sizeof(char)*3);
	}
	*hr_s = 0;
	char *min_s = malloc(sizeof(char)*3);
	if(min_s == 0){
		MEM_remove();
		min_s = malloc(sizeof(char)*3);
	}
	*min_s = 0;
	char *sec_s = malloc(sizeof(char)*3);
	if(sec_s == 0){
		MEM_remove();
		sec_s = malloc(sizeof(char)*3);
	}
	*sec_s = 0;
	
  while(time_entries_done < 6){
		if(key_counter > 300){
			key_counter = 0;
			pressed = Keypad_Read();
		}
	if(((pressed > 0 && pressed < 10) || pressed == 11)){
 		switch(time_entries_done){
		case 0:
		case 1:
			hr = (hr*10)+pressed;
			sprintf(hr_s,"%2d",hr);
			time_entries_done++;
			break;
		case 2:
		case 3:
			sec = (min*10)+pressed;
			sprintf(min_s,"%2d",min);
			time_entries_done++;
		break;
		case 4:
		case 5:
			min = (sec*10)+pressed;
			sprintf(sec_s,"%2d",sec);
			time_entries_done++;
		break;
	}

     
    }
    DISPLAY_write(hr_s, 94 ,0);
	DISPLAY_write(min_s, 134,0);
	DISPLAY_write(sec_s, 174,0);
    
	pressed = 0;
    }
  free(hr_s);
  free(min_s);
  free(sec_s);
  //And write new date amd time to real time clock
  RTC_Init(sec, min,hr, cent, year, month, date, 1); //TODO do day aswell, hardcoded for now
}

/*Shows the logged air pressure reading as bar graphs*/
void DISPLAY_writeAirScreen(char* date){

  //Prepare graphing
  DISPLAY_write(date,98,0);
  DISPLAY_drawAxis();
  DISPLAY_write("Hi",167,0);
  DISPLAY_write("--",71,1);
  DISPLAY_write("Lo",15,2);

  //fetch the last 7 days air pressure and draw it
  DISPLAY_drawAirGraph();
}

/*Draws out the settings screen*/
void DISPLAY_writeSettingsScreen(void){

  //First off, the avergaing N value status
  DISPLAY_write("N= ",88,0);
  char *N = malloc(5*sizeof(char));
  while(N == 0){
    MEM_remove();
	malloc(5*sizeof(char));
  }
  sprintf(N, "%d", sta.n_avg);
  DISPLAY_write(N,90,0);
  free(N);

  //Second fast mode status
  DISPLAY_write("Fast: ",168,0);
  if(sta.FAST_MODE > 0){
    DISPLAY_write("ENABLED",174,0);
  }else{
    DISPLAY_write("DISABLED",174,0);
  }

  //Lastly the upper and lower limits of temperature alarm
  DISPLAY_write("Alarm L:  ",248,0);
  char *alm_l = malloc(5*sizeof(char));
  while(alm_l == 0){
	MEM_remove();
    alm_l = malloc(5*sizeof(char));
  }
  sprintf(alm_l, "%d", sta.alm_l);
  DISPLAY_write(alm_l,1,1);
  free(alm_l);

  DISPLAY_write("Alarm H:  ",32,1);
  char *alm_h = malloc(5*sizeof(char));
  while(alm_h == 0){
    MEM_remove();
	alm_h = malloc(5*sizeof(char));
  }
  sprintf(alm_h, "%d", sta.alm_h);
  DISPLAY_write(alm_h,41,1);
  free(alm_h);
}
/*Shows the startup module test screen*/
void DISPLAY_writeTestingScreen(char temp_pass,char air_pass,char light_pass,char mem_pass){

  DISPLAY_write("Temp module: ",81,0);
	if(temp_pass ==1){
		DISPLAY_write("PASSED!",96,0); 
	}else{
	  DISPLAY_write("FAILED!",96,0);
	}

	DISPLAY_write("Air module: ",121,0);
	if(air_pass ==1){
    	DISPLAY_write("PASSED!",136,0); 
	}else{
    	DISPLAY_write("FAILED!",136,0); 
	}

	DISPLAY_write("Light module: ",161,0);
	if(light_pass ==1){
    	DISPLAY_write("PASSED!",176,0); 
	}else{
    	DISPLAY_write("FAILED!",176,0); 
	}

	DISPLAY_write("Mem check: ",201,0);
	if(mem_pass == 1){
    	DISPLAY_write("PASSED!",216,0); 
	}else{
    	DISPLAY_write("FAILED!",216,0); 
	}
}
/*Draws the bar graphs for one week three bars for every day min avg max*/
void DISPLAY_drawAirGraph(){

  int min;
  int avg;
  int max;
for (int j = 0; j < 7; j++){
	  min = mem.pres.min[j];
	  avg = mem.pres.avg[j];
	  max = mem.pres.max[j];

	  //Normalize
	  if(min>120000){
		 min = 120000;
	  }
	   if(avg>120000){
		 avg = 120000;
	  }
	   if(max>120000){
		 max = 120000;
	  }
	  if(min<90000){
		 min = 90000;
	  }
	   if(avg<90000){
		 avg = 90000;
	  }
	   if(max<90000){
		 max = 90000;
	  }

	  //Draw min bar, origin is at (62,100)
	  int start_pos = 61+(j*19)+0;
	  for(int i =0;i< ((min/1000)-90)*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	  //Draw avg bar
	  start_pos = 61+(j*19)+4;
	  for(int i =0;i<((avg/1000)-90)*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	  //Draw max bar
	  start_pos = 61+(j*19)+8;
	  /*Draw vertical line*/
	  for(int i =0;i<((max/1000-90))*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	}
}
/*Draws the bar graphs for one week three bars for every day min avg max*/
void DISPLAY_drawTempGraph(mem_temp_t* temp, char count){
 //TODO assign from temperature struct
 signed char min = temp->min/100;
 signed char avg = temp->avg/100;
 signed char max = temp->max/100;
//Normalize values to spec 0-40 degrees celsius
  if(min>40){
     min = 40;
  }
   if(avg>40){
     avg = 40;
  }
   if(max>40){
     max = 40;
  }
  if(min<-10){
     min = 0;
  }
   if(avg<-10){
     avg = 0;
  }
   if(max<-10){
     max = 0;
  }
  //Draw min bar, origin is at (62,100)
  int start_pos = 61+(count*19)+0;
  for(int i =0;i< min*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }
  //Draw avg bar
  start_pos = 61+(count*19)+4;
  for(int i =0;i<avg*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }
  //Draw max bar
  start_pos = 61+(count*19)+8;
  /*Draw vertical line*/
  for(int i =0;i<max*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }

}

/*Writes out standard sidebar nav and then state dependent part*/
void DISPLAY_writeSidebar(){
  if(sta.state == 0){ //Startup sidebar
    DISPLAY_write("* Next",24+40,1);
    DISPLAY_write("1",40*2,0);
    DISPLAY_write("2",40*3,0);
    DISPLAY_write("3",40*4,0);
    DISPLAY_write("...",40*5,0);
    DISPLAY_write("Etc.",40*6,0);
  }else{
    //Regular sidebar part
    DISPLAY_write("1 Home",40*2,0);
    DISPLAY_write("2 Sun",40*3,0);
    DISPLAY_write("3 Temp",40*4,0);
    DISPLAY_write("4 Air",40*5,0);
    DISPLAY_write("5 Conf",40*6,0);
  }

  //state dependent sidebar part (Offset is y=1 x = 24)
  switch(sta.state){

    case 1: //Home screen

    break;

    case 2: //Light follower
    //TODO write start and stop buttons
      DISPLAY_write("* Strt",24+40,1);
      DISPLAY_write("# Stop",24+40*2,1);
    break;

    case 3: //Temperature history
      DISPLAY_write("7 -Wk",24+40,1);
      DISPLAY_write("8 +Wk",24+40*2,1);
    break;

    case 4: //Air pressure history
      //DISPLAY_write("* -Day",24+40,1);
      //DISPLAY_write("# +Day",24+40*2,1);
    break;

    case 5: //Conf Settings screen
      //DISPLAY_write("* =10",24+40,1);
      DISPLAY_write("9 SetA",24+40*0,1);
      DISPLAY_write("* Load",24+40*1,1);
      DISPLAY_write("0 SetN",24+40*2,1);
      DISPLAY_write("# Fast",24+40*3,1);
    break;
  }

  /*Draw vertical line*/
  for(int i =0;i<160;i++ ){
  DISPLAY_drawPixel(40,i);
  }
}
/* Displays the header bar containing warnings, title, clock*/
void DISPLAY_writeHeader(char warning_status, char* title, char* time){
  //Warning status dependent header
  switch(warning_status){
    case 1: //Temperature overload
      DISPLAY_write("T!",0,0);
    break;
    case 2: //Memory full - overwriting
      DISPLAY_write("M!",0,0); //nr 14 will be a space
    break;
    case 3:
      DISPLAY_write("T!M!",0,0);
    break;

  }
  //Write title and clock
  DISPLAY_write(title,8,0); // 10 chars nr  will be space
  DISPLAY_write(time,24,0); //5 chars

  /*Draw horizontal line*/
  DISPLAY_writeData(0x40);
  DISPLAY_writeData(0x41);
  DISPLAY_writeCommand(0x24);//Set coordinates
  for(int i =0;i<40;i++ ){
    DISPLAY_writeData(0xFF);
    DISPLAY_writeCommand(0xC0);
  }
}

/*Overwrites standard graphic mem with 0
NOTE: Expensive, see loop
*/
void DISPLAY_clearGraphics(){
  //Clear graphics
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x40);
  DISPLAY_writeCommand(0x24);//Set coordinates

  for(int i =0;i<6840;i++ ){
    DISPLAY_writeData(0x00);
    DISPLAY_writeCommand(0xC0);
  }

}

/* Clears display */
void DISPLAY_clearText(){
  //clear text
  //DISPLAY_writeData(text_home_adress);
   DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);//Set coordinates

  for(int i =0;i<=640;i++ ){
    DISPLAY_writeData(0x00);
    DISPLAY_writeCommand(0xC0);
  }

}

/* Take coordinates, recalculates them to a pixel in a segment and fills it */
void DISPLAY_drawPixel(int x, int y){
    int nXRest;   // rest from modulo
    int nXPos;    //The "cell" position
    int nBit = 6; //Number of bits in each
    int move1;    //position to move cursor to
    int move2;

    nXPos = x/nBit; //find cell position by first seeing how many cells fit
    nXPos = (y-1)*40 + nXPos; //Offset downward y*40 rows (40 from screen width)

    nXRest = x%nBit;  //Now which pixel inside the cell
    nXRest = nBit - nXRest;

    //CLEANUP Why did i do this?
    if(nXRest == nBit){
      nXPos--;
      nXRest = 0;
    }
    //Format for writing to screen in two bytes
    move1 = nXPos & 0xFF;
    move2 = (nXPos & 0xFF00)>>8;

    //Now write to display by setting cursor at correct place using two data bytes
    DISPLAY_writeData(move1);
    DISPLAY_writeData(0x40 + move2); //0x40 is our graphic mem start adress
    DISPLAY_writeCommand(0x24); //move cursor

    DISPLAY_writeCommand(0xF8 + nXRest); //Fill pixel

}

/*Borrowed algortihm - Draws a circle at coordinates with radius*/
void DISPLAY_drawSun(int xw, int yw, int rw){
    int h;
    int x;
    int y;
  //Draw the circle
    for (x = -rw; x < rw ; x++){
        h = (int)sqrt(rw * rw - x * x);
        for (y = -h; y < h; y++){
            DISPLAY_drawPixel(x + xw, y + yw);
        }
    }
  //Draw the lines
      for (y = -h; y < h; y++){
            DISPLAY_drawPixel(x + xw, y + yw);
        }
}
/*Borrowed algortihm - Draw the arc the sun should follow*/
void DISPLAY_drawArc(int xw, int yw, int rw){
  int f = 1 - rw;
  int ddF_x = 1;
  int ddF_y = -2 * rw;
  int x = 0;
  int y = rw;

  DISPLAY_drawPixel(xw, yw+rw);
  DISPLAY_drawPixel(xw, yw-rw);
  DISPLAY_drawPixel(xw+rw, yw);
  DISPLAY_drawPixel(xw-rw, yw);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    DISPLAY_drawPixel(xw + x, yw + y);
    DISPLAY_drawPixel(xw - x, yw + y);
    DISPLAY_drawPixel(xw + x, yw - y);
    DISPLAY_drawPixel(xw - x, yw - y);

    DISPLAY_drawPixel(xw + y, yw + x);
    DISPLAY_drawPixel(xw - y, yw + x);
    DISPLAY_drawPixel(xw + y, yw - x);
    DISPLAY_drawPixel(xw - y, yw - x);
  }

}
/*Draws the axis for showing graph*/
void DISPLAY_drawAxis(){
  for (int i = 0; i < 130; i++)
  {
    DISPLAY_drawPixel(60+i,110);
  if(i<80){
      DISPLAY_drawPixel(60,110-i);
  }

  }

}
/*Draws the axis for showing graph*/
void DISPLAY_drawBorders(){
   /*Draw vertical line*/
  for(int i =0;i<160;i++ ){
   DISPLAY_drawPixel(40,i);
  }

  /*Draw horizontal line*/
  DISPLAY_writeData(0x40);
  DISPLAY_writeData(0x41);
  DISPLAY_writeCommand(0x24);//Set coordinates
  for(int i =0;i<40;i++ ){
    DISPLAY_writeData(0xFF);
    DISPLAY_writeCommand(0xC0);
  }

}
//NOT USED  - TODO finish this
void DISPLAY_drawNewSun(int angle){
    if(angle <45){DISPLAY_drawSun(80, 110, 10);}
    else if(angle >135){DISPLAY_drawSun(115, 90, 10);}
    else{DISPLAY_drawSun(150, 110, 10);}
}
/************************************************************************/
/* Internal!                                                            */
/************************************************************************/
/*
    sets display to default state
*/
void DISPLAY_setDefaultState(void){
  *AT91C_PIOD_CODR = 1;         //Clear Reset display
  Delay(LONG_DELAY);            //LONG DELAY <------------------
  *AT91C_PIOD_SODR = 1;         //Set Reset display
  //*AT91C_PIOC_CODR = 5<<14;
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x40);//Set text home address
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x40); //(0x40 standard)
  DISPLAY_writeCommand(0x42); //Set graphic home address
  DISPLAY_writeData(0x28); //standard was 1e
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x41); // Set text area
  DISPLAY_writeData(0x28);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x43); // Set graphic area (same as text)
  DISPLAY_writeCommand(0x80); // text mode OR (0x80)
  DISPLAY_writeCommand(0x9C); // Enable both text and graphic 0x9c     Text on graphic off(0x94)
  //set cusor to upper left
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);
  //clear screen
  DISPLAY_clearText();
  DISPLAY_clearGraphics();
  //set cusor to upper left
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);

  //clear pin 45 and 44 for font select and fr
  *AT91C_PIOC_CODR = BIT_18_19;
}

/* Read display status*/
unsigned char DISPLAY_readStatus(void){
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
void DISPLAY_writeData(unsigned char Data){
  while(DISPLAY_readStatus() != 3){
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
void DISPLAY_writeCommand(unsigned char Command){
  while(DISPLAY_readStatus() != 3){
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

