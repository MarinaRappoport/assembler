/* author: Marina Rappoport
 Program represents assembler for assembly language,
 it translates assembly commands to machine code*/

#include <stdio.h>
#include <stdlib.h> /*for malloc*/
#include <string.h>
#include "common.h"
#include "symbol_table.h"
#include "file_scan.h"
#include "utils.h"
#include "commands.h"
#include "data_block.h"

int IC; /*instruction counter*/
int DC; /*data counter*/

int main(int argc, char *argv[]) {
    char *filename;
    FILE *fp;
    int is_error;

    /*if not enough arguments - error message and not compile*/
    if (argc < 2) {
        printf("Usage: assembler as_file1 ...");
        exit(1);
    }

    /*for each input file*/
    while (--argc > 0) {
        is_error = FALSE;
        ++argv;
        filename = (char *) malloc(strlen(*argv) + strlen(AS_FILE_EXT) + 1);
        strcpy(filename, *argv);

        /*save filename without extension*/
        save_file_name(filename);

        /*add extension "as" to file name*/
        fp = fopen(strcat(filename, AS_FILE_EXT), "r");
        /*if no file - error message and not compile*/
        if (!fp) {
            fprintf(stderr, "File \"%s\" not found!", filename);
            continue;
        }
        printf("Compiling file: %s\n", filename);

        /*reset counters*/
        IC = CODE_ADDRESS_BASE;
        DC = 0;

        /*if first scan finished without error*/
        if (first_scan(fp, &IC, &DC) == EXIT_SUCCESS) {
            /*update address for all data labels - add IC offset*/
            update_data_address(IC);

            /*the file position to the beginning*/
            rewind(fp);

            /*if second scan finished without error*/
            if (second_scan(fp) == EXIT_SUCCESS) {
                /*write to object file size of IC and DC*/
                write_first_line_to_object_file(IC - CODE_ADDRESS_BASE, DC);
                write_instructions_to_object_file(IC);
                write_data_to_object_file(IC, DC);
            } else {
                fprintf(stderr, "Error found during the Second scan \n");
                is_error = TRUE;
            }
        } else {
            fprintf(stderr, "Error found during the First scan\n");
            is_error = TRUE;
        }

        fclose(fp);
        close_output_files();
        free_symbol_table();

        /* in case of error delete all the output files*/
        if (is_error) {
            fprintf(stderr, "No output files should be created because of errors in assembly file %s\n", filename);
            delete_output_files();
        } else {
            printf("Compiling is finished successfully: %s\n", filename);
        }
    }
    return EXIT_SUCCESS;
}



