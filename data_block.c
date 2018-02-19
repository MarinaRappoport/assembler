#include <stdio.h>
#include "common.h"

#define MAX_NAM 511		/* maximum number that can represented by 10 bits */
#define MIN_NAM -512	/* minimum number that can represented by 10 bits */
#define DATA_MEMORY_SIZE 256			/*Size of data table (number of lines)*/
int data_block[DATA_MEMORY_SIZE];

int DC = 0; /*Data counter*/


/*function adds number to data block, checks if number valid,
 * in case of error return 1, if no errors - 0*/
int add_int_to_data(char *str_number){
    int is_error = FALSE;

    return is_error;
}

/*function adds string to data block (each letter to its own cell), checks if the string valid,
 * in case of error return 1, if no errors - 0*/
int add_string_to_data(char *string){
    int is_error = FALSE;

    return is_error;
}



