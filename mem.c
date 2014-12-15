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
node *prMem;

/************************************************************************/
/* Internal functions                                                    */
/************************************************************************/
int addNode(short int, datestamp);
datestamp getTime();

/************************************************************************/
/* NOT USED														        */
/* Will initialize the data structure. Call this method only once       */
/* return value -1. Unable to allocate memory							*/
/************************************************************************/
int Memory_Init( void ){
	prMem = NULL;
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
/* Saves new value at first place in data structure                     */
/* If there are no more memory, it will remove oldest entry		 		*/
/* try again.															*/
/* Return value:														*/
/*  1 on success, no overwrite											*/
/*  2 on success, old value overwritten									*/
/* -1 Memory_remove was unable to clear space							*/
/* -2 Memory_Remove tried to clear space but list was empty				*/
/************************************************************************/
int Memory_Save(float fNew_Value){
	short int sNew_Value = (short int)fNew_Value*100;// saves value of 2 decimals. truncate rest
	if(prMem != NULL){
		if( addNode( sNew_Value, getTime() ) < 0){  
			/* out of memory, remove oldest entry and try again */
			int resp = Memory_Remove();
			if(resp > 0){
				if(addNode( sNew_Value, getTime() ) > 0){ 
					return 2;
				}
			}else{	
				return resp;
			}
		}else{
			/* node was successfully added to list. returns 1 on success */
			return 1;
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

/************************************************************************/
/* Removes oldest entry of list											*/
/* Returns 1 if success.												*/
/* -1 = unable to remove node											*/
/* -2 = no nodes to remove, empty list									*/
/************************************************************************/
int Memory_Remove(){
	node *prIt = prMem;
	if(prIt != NULL){
		if(prIt->next != NULL){
			
			while(prIt->next->next != NULL){
				prIt = prIt->next;
			}
			// prIt->next is the last node in the list. will clear up memory at adress and set the pointer value to null
			free(prIt->next); //removes last entry in list.
			prIt->next = NULL;
			return 1;		
		}else{//list has only 1 node
			free(prMem);
			prMem = NULL;
			return 1;
		}
	}else{//trying to remove item of empty list
		return -2;
	}
}

/**
 * Returns float value of temp stored at node
 */
float Memory_Get( node *prNode ){
	if(prNode != NULL){
		return (float)prNode->sTemp/100;
	}else{
		return 600; //TODO: this return should indicate error
	}
}

/*-------------------------------------------------------------------------------------------------------------------*
 *												INTERNAL FUNCTIONS												     *
 *												  implementations												     *
 *-------------------------------------------------------------------------------------------------------------------*/



/************************************************************************/
/* Adds new node first in list											*/
/* returns																*/
/* 1 if node successfully added.										*/
/* -1 if unable to allocate mem (out of memory	)						*/
/************************************************************************/
int addNode(short int sNew_Temp, datestamp tTime){
	node *prTemp_node;
	prTemp_node = malloc(sizeof(node));
	
	if(prTemp_node!= NULL){
		prTemp_node->sTemp = sNew_Temp;
		prTemp_node->tDate = tTime;
		prTemp_node->next = prMem;
		prMem = prTemp_node;
		return 1;
	}else{ //Unable to allocate memory. Handle error
		return -1;
	}
}


/************************************************************************/
/* Returns current time                                                 */
/************************************************************************/
datestamp getTime(){
	/*currTime = RTC_Get_Date();
	datestamp newStamp;
	newStamp.date =  currTime.date;	TODO: remove comments
	newStamp.month = currTime.month;
	newStamp.year =  currTime.year;*/
	datestamp newStamp;
	newStamp.date =  1;
	newStamp.month = 2;
	newStamp.year =  3;
	return newStamp;
}