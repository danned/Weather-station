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

typedef struct datestamp_t{
	int date	: 5;
	int month	: 4;
	int year	: 7;
}datestamp;

typedef struct node_t{
	//float fTemp;
	signed short int sTemp;
	datestamp tDate;
	/*time_t tTime;*/
	struct node_t *next;
}node;

extern node *prMem;

/************************************************************************/
/* Will initialize the datastructure. Call this method only once        */
/************************************************************************/
int Memory_Init( void );

/************************************************************************/
/* Saves new value at first place in data structure                     */
/* If there are no more memory, it will remove oldest entry		 		*/
/* try again.															*/
/* Return value:														*/
/*  1 on success, no overwrite											*/
/*  1 on success, old value overwritten									*/
/* -1 on error															*/
/************************************************************************/
int Memory_Save(float fNew_Value);


/************************************************************************/
/* Removes oldest entry of list											*/
/* Returns 1 if success.												*/
/* -1 = unable to remove entry											*/
/************************************************************************/
int Memory_Remove();




