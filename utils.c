#include<stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include <string.h>
#include "common.h"

/*File contains parse, conversion and output utils*/
#define BASE_32 32;
#define BASE_10_BITS 1024; /* 2^5 */
const char encoding_strange_32[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";

FILE *object_fp;    /*pointer to an object file*/
FILE *entry_fp;     /*pointer to an entry file*/
FILE *external_fp;  /*pointer to an external file*/
char *output_filename;     /* filename of output files*/

/*allocate memory for new string*/
char *new_string(int length) {
    /* +1 - to keep end of string*/
    char *str = (char *) malloc(sizeof(char) * (length + 1));
    if (!str) {
        fprintf(stderr, "Failed to allocate memory for a new string");
        exit(EXIT_FAILURE);
    }
    return str;
}

/*check if the string contains only spaces or tabs*/
int is_empty(const char *s) {
    while (*s != END_OF_STRING) {
        if (!isspace((unsigned char) *s))
            return 0;
        s++;
    }
    return 1;
}

/* open file with certain extension for writing */
FILE *open_file_for_write(char const *ext) {
    char *full_filename = new_string(strlen(output_filename) + strlen(ext));
    strcpy(full_filename, output_filename);
    strcat(full_filename, ext);

    /*Open the file for writing*/
    return fopen(full_filename, "w+");
}

/*function encodes decimal to base 32*/
char *dec_to_base32(int n, int num_sign) {
    /*array to store base_32 numbers in reverse order*/
    int a[10];
    int i, j = 0; /*counter for array*/
    char *encoded;
    /*for negative numbers*/
    if (n < 0) n = n + BASE_10_BITS;
    for (i = 0; n > 0; i++) {
        /* storing remainder in binary array*/
        a[i] = n % BASE_32;
        n = n / BASE_32;
    }
    if (num_sign > 0) {
        while (i < num_sign) a[i++] = 0;
    }
    encoded = new_string(i);
    while (i--) {
        encoded[j++] = encoding_strange_32[a[i]];
    }
    encoded[j] = END_OF_STRING;
    return encoded;
}

/* save filename for all output files */
void save_file_name(char *fileName) {
    if (output_filename) output_filename = realloc(output_filename, strlen(fileName) + 1);
    else output_filename = malloc(strlen(fileName) + 1);
    strcpy(output_filename, fileName);
}

/* write label to an Entry file*/
void write_to_entry_file(char *label, int address) {
    if (!entry_fp)  /*open first time*/
        entry_fp = open_file_for_write(ENTRY_FILE_EXT);

    if (!entry_fp) fprintf(stderr, "Can't create Entry file\n");
    else fprintf(entry_fp, "%s\t%s\n", label, dec_to_base32(address, -1));
}

/* write label and address (in 32-encoding) to External file*/
void write_to_external_file(char *label, int address) {
    if (!external_fp) /*open first time*/
        external_fp = open_file_for_write(EXTERN_FILE_EXT);

    if (!external_fp) fprintf(stderr, "Can't create External file\n");
    else fprintf(external_fp, "%s\t%s\n", label, dec_to_base32(address, -1));
}

/* write first row to Object file - size of IC and DC in 32-encoding*/
void write_first_line_to_object_file(int IC_size, int DC_size) {
    if (!object_fp) object_fp = open_file_for_write(OBJECT_FILE_EXT);
    if (!object_fp) fprintf(stderr, "Can't create Object file\n");
    else fprintf(object_fp, "%s\t%s\n", dec_to_base32(IC_size, -1), dec_to_base32(DC_size, -1));
}

/* write row to Object file - address and machine code in 32-encoding*/
void write_to_object_file(int address, int machineCode) {
    fprintf(object_fp, "%s\t%s\n", dec_to_base32(address, -1), dec_to_base32(machineCode, 2));
}

/* close all output files*/
void close_output_files() {
    if (entry_fp) fclose(entry_fp);
    entry_fp = NULL;
    if (external_fp)fclose(external_fp);
    external_fp = NULL;
    if (object_fp)fclose(object_fp);
    object_fp = NULL;
}

/* delete all output files*/
void delete_output_files(){
    remove(strcat(output_filename,ENTRY_FILE_EXT));
    remove(strcat(output_filename,EXTERN_FILE_EXT));
    remove(strcat(output_filename,OBJECT_FILE_EXT));
}



