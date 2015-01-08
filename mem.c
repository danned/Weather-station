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
#include <stdlib.h>
#include "includes/common.h"
mem_t mem;

/************************************************************************/
/* Internal functions                                                    */
/************************************************************************/
int addNode(short int, datestamp_t);
datestamp_t getDate();

/************************************************************************/
/* Will initialize the data structure. Call this method only once       */
/* 																        */
/* return value -1. Unable to allocate memory							*/
/************************************************************************/
int MEM_init( void ){
	//mem.temp = NULL;
	mem.temp = malloc(sizeof(node_t) );
	if(mem.temp == NULL){ // Address of root to mem at null, error
		/* need a error message*/
		return -1;
	}
		
	mem.temp->next = NULL;
	mem.temp->temp.min = 30000;    // If min value is very high, it will be overwritten at first MEM_save
	mem.temp->temp.max = -30000;   // If max value is very low, it will be overwritten at first MEM_save
	mem.temp->temp.avg = 0; 		 // initialize avg to 0
	mem.temp->date = getDate();
	return 1;
}


/************************************************************************/
/* Updates values of current day						                */
/* if new meas is less then saved min. Min val is updated				*/
/* same for max. Avg will hold sum of values until new day event		*/
/*	count holds number of measurements									*/
/*  0 Success. No new min, max saved									*/
/* 	1 Success. new min saved											*/
/*  2 Success. New Max saved											*/
/*  3 Success. New Max and Min saved									*/
/* -1 fail. no value TODO: implement									*/
/************************************************************************/
int MEM_save(float new_val_f){
	node_t *cur_node = mem.temp;
	short int new_val_s = (short int) (new_val_f*100);// saves value of 2 decimals. truncate rest
	char ret_val = 0;
	cur_node->temp.avg+= new_val_s;
	cur_node->temp.count++;
	if(new_val_s < cur_node->temp.min){
		cur_node->temp.min = new_val_s;
		ret_val += 1;
	}
	
	if( new_val_s > cur_node->temp.max){
		cur_node->temp.max = new_val_s;
		ret_val += 2;
	}	
	return ret_val;
}

/************************************************************************/
/* Saves new value at first place in data structure                     */
/* If there are no more memory, it will remove oldest entry		 		*/
/* try again.															*/
/* Return value:														*/
/*  1 on success, no overwrite											*/
/*  2 on success, old value overwritten									*/
/* -1 Memory_remove was unable to clear space							*/
/* -2 Memory_Remove tried to clear space but list was empty				*/
/************************************************************************/
/*int MEM_save(float fNew_Value){
	short int sNew_Value = (short int)fNew_Value*100;// saves value of 2 decimals. truncate rest
	if(mem.temp != NULL){
		if( addNode( sNew_Value, getTime() ) < 0){
*/			/* out of memory, remove oldest entry and try again */
/*			int resp = MEM_remove();
			if(resp > 0){
				if(addNode( sNew_Value, getTime() ) > 0){
					return 2;
				}
			}else{
				return resp;
			}
		}else{
*/			/* node was successfully added to list. returns 1 on success */
/*			return 1;
		}
	}else{// first value saved
		if( addNode( sNew_Value, getTime() ) == -1 ){ //this really shouldn't happen. Unable to allocate when memory is empty<-------------------TODO: get time
			return -1;
		}else{
			return 1;
		}

	}
	return -5;
}
*/
/************************************************************************/
/* Removes oldest entry of list											*/
/* Returns 1 if success.												*/
/* Returns 2 if removed node is only node								*/
/* -1 = unable to remove node											*/
/* -2 = no nodes to remove, empty list									*/
/************************************************************************/
int MEM_remove(){
	node_t *it_pr = mem.temp;
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

/**
 * Returns float value of temp stored at node
 */
temp_t MEM_get( node_t *node_pr ){
	//if(node_pr != NULL){
		return node_pr->temp;
	//}else{
		//return NULL; //TODO: this return should indicate error
	//}
}



/************************************************************************/
/* Adds new day to linked list											*/
/* Returns 1 if success.												*/
/* Returns -1 if fail													*/
/************************************************************************/
int MEM_newDay(){
	node_t *new_node_pr = malloc(sizeof(node_t));
	if(new_node_pr == NULL){// memory is full. Remove oldest entry
		mem.status.MEM_FULL = TRUE;
		signed char resp = MEM_remove();
		if(resp < 0){ // unable to clear space in memory
			//mem.status.MEM_ERROR = TRUE;
		}else{
			new_node_pr = malloc(sizeof(node_t));
		}
	}

	if(new_node_pr != NULL){
		new_node_pr->next = mem.temp;
		mem.temp = new_node_pr;
		mem.temp->temp.min = 30000;    // If min value is very high, it will be overwritten at first MEM_save
		mem.temp->temp.max = -30000;   // If max value is very low, it will be overwritten at first MEM_save
		mem.temp->temp.avg = 0; 		 // initialize avg to 0
		mem.temp->date = getDate();
		return 1;
	}else{
		mem.status.MEM_ERROR = TRUE;
		return -1;
	}
}


/*-------------------------------------------------------------------------------------------------------------------*
 *												INTERNAL FUNCTIONS												     *
 *												  implementations												     *
 *-------------------------------------------------------------------------------------------------------------------*/



/************************************************************************/
/* Adds new node first in list	DEPRECATED								*/
/* returns																*/
/* 1 if node successfully added.										*/
/* -1 if unable to allocate mem (out of memory	)						*/
/************************************************************************/
int addNode(short int new_temp, datestamp_t time){
	node_t *tmp_node_pr;
	tmp_node_pr = malloc(sizeof(node_t));

	if(tmp_node_pr!= NULL){
		//tmp_node_pr->temp = new_temp;
		tmp_node_pr->date = time;
		tmp_node_pr->next = mem.temp;
		mem.temp = tmp_node_pr;
		return 1;
	}else{ //Unable to allocate memory. Handle error
		return -1;
	}
}


/************************************************************************/
/* Returns current time                                                 */
/************************************************************************/
datestamp_t getDate(){
	/*currTime = RTC_Get_Date();
	datestamp newStamp;
	newStamp.date =  currTime.date;	TODO: remove comments
	newStamp.month = currTime.month;
	newStamp.year =  currTime.year;*/
	datestamp_t newStamp;
	newStamp.date =  1;
	newStamp.month = 2;
	newStamp.year =  3;
	return newStamp;
}

int MEM_test(void){
    MEM_init();
    MEM_save(25.5);
    MEM_save(23.3);
    MEM_newDay();
    MEM_save(10);
    MEM_save(12);
    MEM_newDay();
    MEM_save(-2);
    MEM_save(-5);

    node_t *temp = mem.temp;
    char count = 0;
    //Get last 7 days worth of data from database
    while(temp != NULL && count <7){
        //Display_Draw_Graph(&temp->temp, count);
        printf("max: %f. min: %f. avg: %f\n",((float)MEM_get(temp).max)/100,((float)MEM_get(temp).min)/100,((float)MEM_get(temp).avg)/100);
        count++;
        temp = temp->next;
    }

}