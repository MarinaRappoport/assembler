
#include <stdio.h>
#include <dxtmpl.h>
#include "common.h"

#define MAX_ROW_LENGTH 81 /* 80 + 1 for \n*/
char label[MAX_LABEL_SIZE];
int has_label = 0;
#define BUFF_SIZE 80
char buf[BUFF_SIZE];
int row_number = 1;

int is_label_valid(char *token);

/*first scan of the file
3. if .entry - ignore
4. if .extern - add to symbol table (name, isExtern-true)
5. if label (check if vabel valid) - ends with : - put to the symbol table (name, address - IC|DC, isExterm, isMethod);
    - if entry/extern - ignore label
    - if data/ struct/ string - add to symbol table + count DC + save binary
    - if command - add to symbol table + count IC
6. Without label:
- if data/ struct/ string - add to symbol table + count DC + save binary
- if command - add to symbol table + count IC

in case of error return 1, if no errors - 0*/
int first_scan(FILE *fp, int *IC) {
    char *row;
    char *token; /*to get part of the row split by space*/
//    char *row_pnt;

    row = (char *) malloc(MAX_ROW_LENGTH);
    token = (char *) malloc(MAX_ROW_LENGTH);

    if (!token || !row) {
        fprintf(stderr, "Can not allocate memory for row: %d \n", row_number);
        exit(1);
    }
    /*read line by line*/
    while (fgets(row, MAX_ROW_LENGTH, fp) != NULL) {
        /* skip spaces in the beginning*/
        while (*row == SPACE_DELIM[0] || *row == '\t') row++;
        /*if empty string or if comment (start with ;) - ignore*/
        if (*row == ';' || *row == '\n') continue;

        token = strtok(row, SPACE_DELIM);
        /*if label*/
        if (token[strlen(token) - 1] == ':') {
            if (is_label_valid(token)) {
                /*remove : from the end*/
                strncpy(label, token, strlen(token) - 1);
                label[strlen(token) - 1] = '\0';
            }
        }


    }

//    /*char c;
//    while ((c = getc(fp)) != EOF) {
//        if (c == ' ' || c == '\t') {
//            switch (pos) {
//                case NEW_LINE:
//                    /*skip empty line*/
//                    if (i > 0) {
//                        pos = COMMAND;
//                        buf[i] = '\0'; /*end of string*/
//                        if (commands_lookup(buf) == -1) {
//                            /*error - unknown command*/
//                        } else {
//                            /*TODO count the place for the command*/
//                            IC++;
//                            if (has_label) {
//                                add_to_symbol_table(&head, buf, IC, FALSE, TRUE);
//                            }
//                        }
//                    }
//                    break;
//                case GUIDELINE:
//                    buf[i] = '\0'; /*end of string*/
//                    /*ignore label for extern and entry*/
//                    if (strcmp(buf, ENTRY) == 0) {
//                        /*skip for first run*/
//                    } else if (strcmp(buf, EXTERNAL) == 0) {
//                        pos = EXTERN;
//                    } else if (strcmp(buf, DATA) == 0) {
//
//                    } else if (strcmp(buf, STRING) == 0) {
//
//                    } else if (strcmp(buf, STRUCT) == 0) {
//
//                    } else {
//                        /*error*/
//                    }
//                case EXTERN:
//                    is_label_valid(buf, i);
//                    add_to_symbol_table(&head, buf, 0, TRUE, FALSE);
//                default:
//                    break;
//            }
//        } else if (c == '\n') {
//            pos = NEW_LINE;
//            has_label = 0;
//            /*clear buffer*/
//            i = 0;
//            memset(&buf[0], 0, sizeof(buf));
//        } else if (c == ';') {
//            if (pos == NEW_LINE) {
//                while ((c = getc(fp)) != EOF || c != '\n');
//                /*skip comment*/
//            }
//        } else if (c == '.') {
//            /*extern, entry, string, data or struct*/
//            switch (pos) {
//                case NEW_LINE:
//                    pos = GUIDELINE;
//                    break;
//                default:
//                    /*error*/
//                    break;
//            }
//        } else if (c == ':') {
//            switch (pos) {
//                case NEW_LINE:
//                    /*it's label*/
//                    if (has_label) {
//                        /*error - second label in the line*/
//                    }
//                    has_label = 1;
//                    is_label_valid(buf, i);
//                    strcpy(label, buf);
//                    /*clear buffer*/
//                    i = 0;
//                    memset(&buf[0], 0, sizeof(buf));
//                    break;
//                default:
//                    /*error*/
//                    break;
//            }
//        } else {
//            /*collect buffer*/
//            if (i >= BUFF_SIZE) {
//                printf("Too many characters in line");
//            } else {
//                buf[i++] = c;
//            }
//        }*/

}

}

/*second scan of the file
1. if empty string - ignore
2. if comment (start with ;) - ignore
3. if .entry - to file .ent: name(label) + address(in base 32)
4. if .extern - ignore
5. NOTHING to do with labels!
6. if command - build code in binary:
    0000   -  10 - 01 - 00
    opcode - op1 - op2 - AER
    ?operators: 00 - number
                01 - variable(label)
                10 - struct
                11 - register
    ?AER:	 00 - absolute
            01 - external
            10 - relocated

    for struct: 1) address(of label)->8 bits - 10 (relocated) - because it depends on IC
                2) number inside struct: 1 - 00000001-00, 2 - 00000010-00

    for variables - check in symbol table:
                1) if extern: 00000000-01
                + to file .ext: name(label) + address=IC - of method it uses(in base 32)

in case of error return 1, if no errors - 0*/
int second_scan(FILE *fp) {

}


int is_label_valid(char *token) {
    /*should start with letter, contains only letters(65-90,97-122) or digits(48-57), length not more that 30*/
    int i = 0;
    int c = buf[i];
    if (strlen(token) > (MAX_LABEL_SIZE + 1))
        printf("Length of label should be less or equal %d, row: %d", MAX_LABEL_SIZE, row_number);
    if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122)) {
        printf(stderr, "Label should start with letter, row: %d", row_number);
    }
    while (i++ < MAX_LABEL_SIZE) {
        if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122) && !(c >= 48 && c <= 57)) {
            printf(stderr, "Label should contain only letters and numbers, row: %d", row_number);
        }
    }
    //TODO check reserved words
    if (0) {
        printf("Can not use reserved word for label, row: %d", row_number);
    }
}

int is_label_valid_old(char *buf, int size) {
    /*should start with letter, contains only letters(65-90,97-122) or digits(48-57), length not more that 30*/
    int i = 0;
    int c = buf[i];
    if (size > 30) printf("Length of label should be less or equal 30");
    if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122)) {
        printf(stderr, "Label should start with letter");
    }
    while (i++ < size) {
        if (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122) && !(c >= 48 && c <= 57)) {
            printf(stderr, "Label should contain only letters and numbers");
        }
    }
    //TODO check reserved words
    if (0) {
        printf("Can not use reserved word for label");
    }
    buf[size] = '\0'; /*end of string*/
}
