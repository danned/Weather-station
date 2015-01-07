/************************************************************************/
/* Memory module														*/
/* This is the implementation of the memory module for the project		*/
/* This module will perform all memory specific code					*/
/* Uses a single liked list data structure and requires only calls to	*/
/* add and get data. Will set flag and remove oldest entry when			*/
/* memory is full														*/
/************************************************************************/

#include "mem.h"
#include <stdlib.h>
node_t *mem_root_pr;

/************************************************************************/
/* Internal functions                                                    */
/************************************************************************/
int addNode(short int, datestamp_t);
datestamp_t getTime();

/************************************************************************/
/* NOT USED														        */
/* Will initialize the data structure. Call this method only once       */
/* return value -1. Unable to allocate memory							*/
/************************************************************************/
int MEM_init( void ){
	mem_root_pr = NULL;
	/**prMem = malloc(sizeof(node));

	if(prMem != NULL){
		return 1;
	}else{ // Address of root to mem at null, error
		/* need a error message*/
		/*return -1;
	}*/
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
/* -1 fail. no value TODO: implement									*/
/************************************************************************/
int MEM_save(float new_val_f){
	node_t *cur_node = mem_root_pr;
	short int new_val_s = (short int) new_val_f*100;// saves value of 2 decimals. truncate rest
	cur_node->temp.avg+= new_val_s;
	cur_node->temp.count++;
	if(new_val_s < cur_node->temp.min){
		cur_node->temp.min = new_val_s;
		return 1;
	}
	else if( new_val_s > cur_node->temp.max){
		cur_node->temp.max = new_val_s;
		return 2;
	}	
	return 0;
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
	if(mem_root_pr != NULL){
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
/* -1 = unable to remove node											*/
/* -2 = no nodes to remove, empty list									*/
/************************************************************************/
int MEM_remove(){
	node_t *it_pr = mem_root_pr;
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
			free(mem_root_pr);
			mem_root_pr = NULL;
			return 1;
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
/*int addNode(short int new_temp, datestamp_t time){
	node_t *tmp_node_pr;
	tmp_node_pr = malloc(sizeof(node_t));

	if(tmp_node_pr!= NULL){
		tmp_node_pr->temp = new_temp;
		tmp_node_pr->date = time;
		tmp_node_pr->next = mem_root_pr;
		mem_root_pr = tmp_node_pr;
		return 1;
	}else{ //Unable to allocate memory. Handle error
		return -1;
	}
}*/


/************************************************************************/
/* Returns current time                                                 */
/************************************************************************/
datestamp_t getTime(){
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
