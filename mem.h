/************************************************************************/
/* Memory module														*/
/* This is the declaration  of the memory module for the project		*/
/* This module will perform all memory specific code					*/
/* Uses a single liked list data structure and requires only calls to   */
/* add and get data. Will set flag and remove oldest entry when		    */
/* memory is full														*/
/************************************************************************/

/************************************************************************/
/* Type definitions crucial for memory module                           */
/************************************************************************/
#ifndef _MEM_H_
#define _MEM_H_
typedef struct {
	int date	: 5;
	int month	: 4;
	int year	: 7;
}datestamp_t;


/* datastructures for temperature start */
/*typedef struct {
	short min;
	short avg;
	short max;
	short count; // this might be removed
}temp_t;
*/
typedef struct node{
	short min;
	short avg;
	short max;
	short count; // this might be removed
	datestamp_t date;
	struct node *next;
}mem_temp_t;
/* temperature end */

/* datastructures for air pressure start */

/*typedef struct{
	
}pres_value_t;
*/

typedef struct {
	char day;
	char count;
	unsigned int max[7];
	unsigned int avg[7];
	unsigned int min[7];
}mem_air_pres_t;

/* air pressure end */
typedef struct{
	char MEM_FULL  : 1;
	char MEM_ERROR : 1;
}mem_status_t;

typedef struct{
	mem_temp_t *temp;
	
	mem_air_pres_t pres;
	mem_status_t status;
}mem_t;

//extern temp_t *mem_root_pr;
extern mem_t mem;

/************************************************************************/
/* Will initialize the datastructure. Call this method only once        */
/************************************************************************/
int MEM_init( void );

/************************************************************************/
/* Saves new value at first place in data structure                     */
/* If there are no more memory, it will remove oldest entry		 		*/
/* try again.															*/
/* Return value:														*/
/*  1 on success, no overwrite											*/
/*  1 on success, old value overwritten									*/
/* -1 on error															*/
/************************************************************************/
int MEM_saveTemp(float new_value_f);


int MEM_savePres(unsigned int new_value_u32);

int MEM_save(float new_temp_f, unsigned int new_pres_u32);

/************************************************************************/
/* Removes oldest entry of list											*/
/* Returns 1 if success.												*/
/* -1 = unable to remove entry											*/
/************************************************************************/
int MEM_remove();


/**
 * Returns float value of temp stored at node
 */
mem_temp_t MEM_get( mem_temp_t *node_pr );

/************************************************************************/
/* Adds new day to linked list											*/
/* Returns 1 if success.												*/
/* Returns -1 if fail													*/
/************************************************************************/
int MEM_newDay();


int MEM_test(void);


#endif // _MEM_H_

