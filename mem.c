/************************************************************************************************************************/
/* Memory module																										*/
/* This is the implementation of the memory module for the project														*/
/* This module will perform all memory specific code																	*/
/* Uses a single liked list data structure and requires only calls to													*/
/* add and get data. Will set flag and remove oldest entry when															*/
/* All variables of module can be found in mem struct, defined in header												*/
/* root of temp list ist at mem.temp																					*/
/* status flags are found at mem.status																					*/
/************************************************************************************************************************/

#include "mem.h"
#include "rtc.h"
#include <stdlib.h>
#include <stdio.h>
#include "includes/common.h"
mem_t mem;

/************************************************************************/
/* Internal functions                                                    */
/************************************************************************/
int addNode(short int, datestamp_t);
datestamp_t getDate();//deprecated


static void initTemp(){
	mem.temp->min = 30000;    // If min value is very high, it will be overwritten at first MEM_save
	mem.temp->max = -30000;   // If max value is very low, it will be overwritten at first MEM_save
	mem.temp->avg = 0; 		 // initialize avg to 0
	mem.temp->date = RTC_getDate();
	mem.temp_count = 0;
}

static void initPres(int day){
	mem.pres.min[day] = 2000000;
	mem.pres.max[day] = 0;
	mem.pres.avg[day] = 0;
	mem.pres.count = 0;
}

/************************************************************************/
/* Will initialize the data structure. Call this method only once       */
/* 																        */
/* return value -1. Unable to allocate memory							*/
/************************************************************************/
int MEM_init( void ){
	//mem.temp = NULL;

	/* temp init start */
	mem.temp = malloc(sizeof(mem_temp_t) );
	if(mem.temp == NULL){ // Address of root to mem at null, error
		/* need a error message*/
		return -1;
	}

	mem.temp->next = NULL;
	mem.temp->date = RTC_getDate();
	/* temp init end */
	
	/* pres init start */
	for(int i = 0; i<7;i++){
		initPres(i);
	}
	mem.pres.count = 0;
	/* pres init end */
	return 1;
}


/************************************************************************/
/* Updates temp values of current day						                */
/* if new meas is less then saved min. Min val is updated				*/
/* same for max. Avg will hold sum of values until new day event		*/
/*	count holds number of measurements									*/
/*  0 Success. No new min, max saved									*/
/* 	1 Success. new min saved											*/
/*  2 Success. New Max saved											*/
/*  3 Success. New Max and Min saved									*/
/* -1 fail. no value TODO: implement									*/
/************************************************************************/
int MEM_saveTemp(float new_val_f){
	mem_temp_t *cur_node = mem.temp;

	short int new_val_s = (short int) (new_val_f*100);// saves value of 2 decimals. truncate rest
	char ret_val = 0;
	int temp_sum = (cur_node->avg*mem.temp_count) + new_val_s;
	mem.temp_count++;
	cur_node->avg= temp_sum/mem.temp_count;

	if(new_val_s < cur_node->min){
		cur_node->min = new_val_s;
		ret_val += 1;
	}

	if( new_val_s > cur_node->max){
		cur_node->max = new_val_s;
		ret_val += 2;
	}
	return ret_val;
}


/************************************************************************/
/* Updates pressure values of current day						        */
/* if new meas is less then saved min. Min val is updated				*/
/* same for max. Avg will hold sum of values until new day event		*/
/*	count holds number of measurements									*/
/*  0 Success. No new min, max saved									*/
/* 	1 Success. new min saved											*/
/*  2 Success. New Max saved											*/
/*  3 Success. New Max and Min saved									*/
/* -1 fail. no value TODO: implement									*/
/************************************************************************/
int MEM_savePres(unsigned int new_val_u32){
	int ret_val = 0;
	if( new_val_u32 > mem.pres.max[mem.pres.day] ){
		mem.pres.max[mem.pres.day] = new_val_u32;
		ret_val+=2;
	}
	if( new_val_u32 < mem.pres.min[mem.pres.day] ){
		mem.pres.min[mem.pres.day] = new_val_u32;
		ret_val+=1;
	}
	int pres_sum = (mem.pres.avg[mem.pres.day] * mem.pres.count) + new_val_u32;
	mem.pres.count++;
	mem.pres.avg[mem.pres.day]= pres_sum / mem.pres.count;
	//mem.pres.count++;
	return ret_val;
}

/************************************************************************/
/* Updates pressure and temp values of current day				        */
/*	calls MEM_savePress and MEM_saveTemp								*/
/* 	1 only temp value saved												*/
/*  2 only pressure value saved											*/
/*  3 Success. pressure and temp value saved							*/
/* -1 fail. unable to save any value									*/
/************************************************************************/
int MEM_save(float new_temp_f, unsigned int new_pres_u32){
	int ret_val = 0;
	mem.cur_temp = new_temp_f;
	if(MEM_saveTemp( new_temp_f ) > 0)
		ret_val += 1;
	if(MEM_savePres( new_pres_u32 ) > 0)
		ret_val += 2;

	if(ret_val == 0)
		ret_val = -1;
	return ret_val;
}


/************************************************************************/
/* Removes oldest entry of list											*/
/* Returns 1 if success.												*/
/* Returns 2 if removed node is only node								*/
/* -1 = unable to remove node											*/
/* -2 = no nodes to remove, empty list									*/
/************************************************************************/
int MEM_remove(){
	mem_temp_t *it_pr = mem.temp;
	if(it_pr != NULL){
		if(it_pr->next != NULL){

			while(it_pr->next->next != NULL){
				it_pr = it_pr->next;
			}
			// it_pr->next is the last node in the list. will clear up memory at adress and set the pointer value to null
			free(it_pr->next); //removes last entry in list.
			it_pr->next = NULL;
			return 1;
		}else{//list has only 1 node
			free(mem.temp);
			mem.temp = NULL;
			return 2;
		}
	}else{//trying to remove item of empty list
		return -2;
	}
}





/************************************************************************/
/* Adds new day to linked list	and updates variable for pressure 		*/
/* Will update status flag mem.status.MEM_FULL if old temp values  		*/
/* are overwritten. mem.status.MEM_ERROR indicates error, BAD			*/
/* Returns 1 if temp is success.										*/
/* Returns 2 if pressure is success.									*/
/* Returns 3 if all is success.											*/
/* Returns -1 if fail													*/
/************************************************************************/
int MEM_newDay(){
	/* start update day temp */
	int ret_val = 0;
	mem_temp_t *new_node_pr = malloc(sizeof(mem_temp_t));
	if(new_node_pr == NULL){// memory is full. Remove oldest entry
		mem.status.MEM_FULL = TRUE;
		signed char resp = MEM_remove();
		if(resp < 0){ // unable to clear space in memory
			//mem.status.MEM_ERROR = TRUE;
		}else{
			new_node_pr = malloc(sizeof(mem_temp_t));
		}
	}

	if(new_node_pr != NULL){
		new_node_pr->next = mem.temp;
		mem.temp = new_node_pr;
		initTemp();
		ret_val += 1;
	}else{
		mem.status.MEM_ERROR = TRUE;
	}
	/* end update day temp */

	/* start update day pressure */

	if(++mem.pres.day >= 7)
		mem.pres.day = 0;

	initPres(mem.pres.day);
	if(mem.pres.max[mem.pres.day] == 0 && mem.pres.min[mem.pres.day] == 200000 && mem.pres.avg[mem.pres.day] == 0 && mem.pres.count == 0)
		ret_val+=2;
	/* end update day pressure */

	return ret_val;
}


/*-------------------------------------------------------------------------------------------------------------------*
 *												INTERNAL FUNCTION												     *
 *											clears memory for MEM_test									     		 *
 *-------------------------------------------------------------------------------------------------------------------*/

static void clearMem(){
  while(MEM_remove() < 2);

  if(MEM_remove() == -2){
	MEM_newDay();
	mem.status.MEM_FULL = 0;
  }
  else mem.status.MEM_ERROR = 1;

  for(int i = 0 ; i<7 ; i++)
	initPres(i);
}


/**
 * MEMORY TEST
 */
int MEM_test(void){
    // TESTING TEMP MEM
	int i = 0;
	for(; !mem.status.MEM_FULL; i++){
		MEM_saveTemp(20.4);
		MEM_newDay();
	}
	MEM_saveTemp(20.4);
	mem_temp_t *it = mem.temp;
	int count = 0;
	while(it!= NULL){
		it = it->next;
		count++;
	}
	if(count != i){
	  clearMem();
		return -2;
	}
	MEM_remove();
	it = mem.temp;
	int count2 = 0;
	while(it!= NULL){
		it = it->next;
		count2++;
	}
	if(count2 != count-1){
		clearMem();
	  return -2;
	}
	if((float)mem.temp->max/100.0 != 20.4 || (float)mem.temp->min/100.0 != 20.4 || (float)mem.temp->avg/100.0 != 20.4){
		clearMem();
	  return -4;
	}
	// TESTING AIR PRESSURE MEM
	for(int pres = 0; pres < 10; pres++){
		MEM_savePres(pres);
		if(mem.pres.max[mem.pres.day] != pres){
		  	clearMem();
			return -5;
		}
		MEM_newDay();
	}
	clearMem();
	return 1;
}

int MEM_fill(void){
    // FILLING MEM
	int i = 0;
	while(!mem.status.MEM_FULL){
		MEM_save( rand()%40 , (rand()+95000)%110000 );
		MEM_save( rand()%40 , (rand()+95000)%110000 );
		MEM_save( rand()%40 , (rand()+95000)%110000 );
		MEM_newDay();
		i++;
	}
	return i;
}
