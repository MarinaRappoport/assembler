#include <stdio.h>
#include <stdlib.h> /*for malloc*/
#include <mem.h>
#include "common.h"
#include "utils.h"
#include "symbol_table.h"

/*structures and global variables*/
enum position {
    NEW_LINE, GUIDELINE, COMMAND, EXTERN
};

int IC = 100; /*instruction counter*/
int DC = 0; /*data counter*/

#define MAX_LABEL_SIZE 30
char label[MAX_LABEL_SIZE];
int has_label = 0;
#define BUFF_SIZE 80
char buf[BUFF_SIZE];

/*function prototypes*/

void first_run();

void second_run();

void is_label_valid(char *buf, int size);


int main1(int argc, char *argv[]) {
    printf("Res: %s", dec_to_base32(100));
}

int main(int argc, char *argv[]) {
    /*if(argc < 2){
        printf("Usage: assembler as_file1 ...");
    }

    while (--argc > 0){
        printf("Compiling file: %s\n", *++argv);
        /*compile file
    }*/

    int c;
    int i = 0; /*to count current buffer size*/
    enum position pos = NEW_LINE;
    Symbol *head = NULL;
    /*TODO add ext to file name */
    FILE *fp = fopen("C:\\Users\\Marina\\Desktop\\uni\\C\\_maman14\\ps.as", "r");
    if (!fp) {
        printf("File not found!");
        exit(0);
    }

    while ((c = getc(fp)) != EOF) {
        if (c == ' ' || c == '\t') {
            switch (pos) {
                case NEW_LINE:
                    /*skip empty line*/
                    if (i > 0) {
                        pos = COMMAND;
                        buf[i] = '\0'; /*end of string*/
                        if (commans_lookup(buf) == -1){
                            /*error - unknown command*/
                        } else{
                            /*TODO count the place for the command*/
                            IC++;
                            if(has_label){
                                add_to_symbol_table(&head,buf,IC, FALSE, TRUE);
                            }
                        }
                    }
                    break;
                case GUIDELINE:
                    buf[i] = '\0'; /*end of string*/
                    /*ignore label for extern and entry*/
                    if (strcmp(buf, ENTRY) == 0) {
                        /*skip for first run*/
                    } else if (strcmp(buf, EXTERNAL) == 0) {
                        pos = EXTERN;
                    } else if (strcmp(buf, DATA) == 0) {

                    } else if (strcmp(buf, STRING) == 0) {

                    } else if (strcmp(buf, STRUCT) == 0) {

                    } else {
                        /*error*/
                    }
                case EXTERN:
                    is_label_valid(buf, i);
                    add_to_symbol_table(&head, buf, 0, TRUE, FALSE);
                default:
                    break;
            }
        } else if (c == '\n') {
            pos = NEW_LINE;
            has_label = 0;
            /*clear buffer*/
            i = 0;
            memset(&buf[0], 0, sizeof(buf));
        } else if (c == ';') {
            if (pos == NEW_LINE) {
                while ((c = getc(fp)) != EOF || c != '\n');
                /*skip comment*/
            }
        } else if (c == '.') {
            /*extern, entry, string, data or struct*/
            switch (pos) {
                case NEW_LINE:
                    pos = GUIDELINE;
                    break;
                default:
                    /*error*/
                    break;
            }
        } else if (c == ':') {
            switch (pos) {
                case NEW_LINE:
                    /*it's label*/
                    if (has_label) {
                        /*error - second label in the line*/
                    }
                    has_label = 1;
                    is_label_valid(buf, i);
                    strcpy(label, buf);
                    /*clear buffer*/
                    i = 0;
                    memset(&buf[0], 0, sizeof(buf));
                    break;
                default:
                    /*error*/
                    break;
            }
        } else {
            /*collect buffer*/
            if (i >= BUFF_SIZE) {
                printf("Too many characters in line");
            } else {
                buf[i++] = c;
            }
        }

    }

    fputc(c, fp);

    fclose(fp);
    return 0;
}

void is_label_valid(char *buf, int size) {
    /*should start with letter, contains only letters(65-90,97-122) or digits(48-57), length not more that 30*/
    int i = 0;
    int c = buf[i];
    if (size > 30) printf("Length of label should be less or equal 30");
    if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122)) {
        printf("Label should start with letter");
    }
    while (i++ < size) {
        if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122) && !(c >= 48 && c <= 57)) {
            printf("Label should contain only letters and numbers");
        }
    }
    //TODO check reserved words
    if (0) {
        printf("Can not use reserved word for label");
    }
    buf[size] = '\0'; /*end of string*/
}