/* author: Marina Rappoport
 Program represents assemler for assembly language,
 it translates assembly commands to machine code*/

#include <stdio.h>
#include <stdlib.h> /*for malloc*/
#include "common.h"
#include "symbol_table.h"
#include "file_scan.h"

#define AS_EXT ".as"

/*structures and global variables*/
//enum position {
//    NEW_LINE, GUIDELINE, COMMAND, EXTERN
//};

int IC = CODE_ADDRESS_BASE; /*instruction counter*/
int DC = 0; /*data counter*/

/*function prototypes*/

int main(int argc, char *argv[]) {
    /*read arguments from command line*/

    /*if not enough arguments - error message and not compile*/
    /*if(argc < 2){
        printf("Usage: assembler as_file1 ...");
    }

    /*for each input file
    while (--argc > 0){
        /*add extension "as" to file name
        char *filename;
        ++argv;
        filename = (char*)malloc(strlen(*argv) + strlen(AS_EXT) + 1);
		strcpy(filename,*argv);
		strcat(filename, AS_EXT);
         /*if no file - error message and not compile
        FILE *fp = fopen(filename, "r");
        if(!fp) {
            printf("File %s is not found", filename);
            /*error - move to next file
        }
         printf("Compiling file: %s\n", filename);
    }*/

    int is_error = FALSE;

    int c;
    int i = 0; /*to count current buffer size*/
//    enum position pos = NEW_LINE;

    /*TODO add ext to file name */
    FILE *fp = fopen("C:\\Users\\Marina\\Desktop\\uni\\C\\_maman14\\ps.as", "r");
    if (!fp) {
        printf("File not found!");
        exit(0);
    }

    /*if first scan finished without error*/
    if (first_scan(fp, &IC, &DC) == 0) {

        /*update address for all data labels - add IC offset*/
        update_data_address(IC);

        /*the file position to the beginning*/
        rewind(fp);

        /*if second scan finished without error*/
        if(second_scan(fp) == 0){
            /*write object file*/
            /*write extern file*/
            /*write entry file*/
        }else
            is_error = TRUE;
    } else
        is_error = TRUE;

    fclose(fp);

    return 0;
}

/*object file:
1) size of code =(IC-100) size of data=(DC-IC) - in base 32
2) all methods: address - value - in base 32
3) all data: address - value - in base 32
}



