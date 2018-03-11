#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "utils.h"

#define MAX_NUM 511        /* maximum number that can represented by 10 bits */
#define MIN_NUM (-512)    /* minimum number that can represented by 10 bits */
#define QUOTE '\"'
#define MEMORY_SIZE 256            /*Size of data table (number of lines)*/
int memory_block[MEMORY_SIZE];

/*function adds numbers(data) to data block, checks if number valid,
 * in case of error return 1, if no errors - 0*/
int add_int_to_data_memory(char *str_num, int *DC) {
    int is_error = FALSE;
    int num;
    char *end_ptr;    /*what rests after num*/
    if (*DC == MEMORY_SIZE) {
        fprintf(stderr, "No more space in memory block");
        is_error = TRUE;
    } else {
        num = (int) strtol(str_num, &end_ptr, 10);
        /*check if the number format valid*/
        if (end_ptr == NULL || (end_ptr != NULL && (strcmp(str_num, end_ptr) == 0 || !is_empty(end_ptr)))) {
            is_error = TRUE;
            fprintf(stderr, "Invalid number format %s\n", str_num);
        }
            /*check range of number*/
        else if (num < MIN_NUM || num > MAX_NUM) {
            is_error = TRUE;
            fprintf(stderr, "Can't insert to memory block number %s, it's out or range %d - %d\n", str_num, MIN_NUM,
                    MAX_NUM);
        } else {
            memory_block[(*DC)++] = num;
        }
    }
    return is_error;
}

/*function adds string to data block (each letter to its own cell), checks if the string valid,
 * in case of error return 1, if no errors - 0*/
int add_string_to_memory(char *str, int *DC) {
    int is_error = FALSE;
    /* skip spaces in the beginning*/
    while (*str == ' ' || *str == '\t') str++;

    /* string should starts with quotation marks */
    if (*str == QUOTE) {
        /* add letters one by one - until " or no memory space*/
        /*str++ - to skip first " */
        for (str++; *str != QUOTE && *DC < MEMORY_SIZE; str++, (*DC)++) {
            if (*str == '\0') {
                is_error = TRUE;
                fprintf(stderr, "Wrong format of string %s\n", str);
                return is_error;
            }
            memory_block[*DC] = *str;
        }
        memory_block[(*DC)++] = '\0';

        /* If no more space in "data encoding array" print the error and return 0(false) */
        if (*DC == MEMORY_SIZE) {
            fprintf(stderr, "No more space in memory block");
            is_error = TRUE;
        }

        /*if something left after second " - error */
        str++; /*skip second " */
        /* skip spaces in the end*/
        while (*str == ' ' || *str == '\t') str++;
        if (strlen(str) > 0) {
            is_error = TRUE;
            fprintf(stderr, "Wrong format of string %s\n", str);
        }
    } else {
        is_error = TRUE;
        fprintf(stderr, "Wrong format of string %s\n", str);
    }
    return is_error;
}



