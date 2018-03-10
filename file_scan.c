
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "symbol_table.h"
#include "commands.h"
#include "data_block.h"

#define MAX_ROW_LENGTH 81 /* 80 + 1 for \n*/
char label[MAX_LABEL_SIZE];
int has_label = 0;
#define BUFF_SIZE 80
#define COMMENT_START ';'
#define GUIDELINE_START '.'
char buf[BUFF_SIZE];
int row_number = 1;

int is_label_valid(char *token);

/*check if the string contains only spaces or tabs*/
int is_empty(const char *s);

/*remove all whitespaces from the string*/
void remove_spaces(char *s) {
    char *i = s;
    char *j = s;
    while (*j != 0) {
        *i = *j++;
        if (!isspace((unsigned char) *i)) i++;
    }
    *i = '\0';
}

/*count IC analysing operands of the command*/
int count_IC(int *IC, char *token, int operands_num) {
    int is_error = FALSE, command_offset;
    char *src_op, *dst_op;
    remove_spaces(token); /*TODO fix the method to trim!*/
    if (operands_num == 0 && (!token || is_empty(token))) { (*IC)++; }
    else if (operands_num == 1 && (token && !is_empty(token))) {
        command_offset = calculate_command_offset(NULL, token, row_number);
        if (command_offset == NOT_FOUND) is_error = TRUE;
        else (*IC) += command_offset;
    } else if (operands_num == 2 && (token && !is_empty(token))) {
        /*remove whitespaces and separate by ','*/
        src_op = strtok(token, COMMA_DELIM);
        dst_op = strtok(NULL, COMMA_DELIM);
        token = strtok(NULL, COMMA_DELIM);
        if (!dst_op || is_empty(dst_op) || token) {
            is_error = TRUE;
            fprintf(stderr, "Wrong number of operands, row: %d\n", row_number);
        } else {
            command_offset = calculate_command_offset(src_op, dst_op, row_number);
            if (command_offset == NOT_FOUND) is_error = TRUE;
            else (*IC) += command_offset;
        }
    } else {
        is_error = TRUE;
        fprintf(stderr, "Wrong number of operands, row: %d\n", row_number);
    }
    return is_error;
}

/*first scan of the file
3. if .entry - ignore
4. if .extern - add to symbol table (name, isExtern-true)
5. if label (check if label valid) - ends with : - put to the symbol table (name, address - IC|DC, isExterm, isMethod);
    - if entry/extern - ignore label
    - if data/ struct/ string - add to symbol table + count DC + save binary
6. Without label:
- if data/ struct/ string - count DC + save binary

in case of error return 1, if no errors - 0*/
int first_scan(FILE *fp, int *IC, int *DC) {
    int is_error = FALSE;
    char *row;
    char *token; /*to get part of the row split by space*/
    Command command;
//    char *row_pnt;

    row = (char *) malloc(MAX_ROW_LENGTH);
//    token = (char *) malloc(MAX_ROW_LENGTH);

    if (!row) {
        fprintf(stderr, "Can not allocate memory for row: %d \n", row_number);
        exit(1);
    }

    /*read line by line*/
    while (fgets(row, MAX_ROW_LENGTH, fp) != NULL) {
        /*clear label*/
        memset(label, '\0', sizeof(label));

        /* skip spaces in the beginning*/
        while (*row == SPACE_DELIM[0] || *row == '\t') row++;
        /*if empty string or if comment (start with ;) - ignore*/
        if (*row == COMMENT_START || *row == '\n') continue;

        /*replace end of line*/
        row[strlen(row) - 1] = '\0';

        /*take the first token*/
        token = strtok(row, SPACE_DELIM);
        /*if label*/
        if (token[strlen(token) - 1] == ':') {
            if (is_label_valid(token)) {
                /*remove ':' from the end*/
                strncpy(label, token, strlen(token) - 1);
                label[strlen(token) - 1] = '\0';

                /*take the next element of the row */
                token = strtok(NULL, SPACE_DELIM);
                /*if the token is null or empty - error*/
                if (!token || is_empty(token)) {
                    is_error = TRUE;
                    fprintf(stderr, "Label can't be alone in line, row: %d\n", row_number);
                    continue;
                }
            }
        }

        /*TODO - separate method fot guideline*/
        /*if guideline - starts with "."*/
        if (*token == GUIDELINE_START) {
            token++; /*skip '.'*/
            if (strcmp(token, ENTRY) == 0) { /*ignore*/
                if (strlen(label)) fprintf(stderr, "Warning: ignore label before ENTRY, row %d", row_number);
            } else if (strcmp(token, EXTERN) == 0) {
                if (strlen(label)) fprintf(stderr, "Warning: ignore label before EXTERN, row %d", row_number);
                token = strtok(NULL, SPACE_DELIM);
                if (is_label_valid(token)) {
                    add_to_symbol_table(token, 0, TRUE, FALSE);
                } else {
                    printf("Invalid EXTERN label, row %d\n", row_number);
                    is_error = TRUE;
                }
                token = strtok(NULL, SPACE_DELIM);
                if (token && !is_empty(token)) {
                    printf("Additional data after EXTERN label, row %d\n", row_number);
                    is_error = TRUE;
                }
            } else if (strcmp(token, STRING) == 0) {
                /*take rest*/
                token = strtok(NULL, "");
                if (add_string_to_memory(token, DC)) {
                    is_error = TRUE;
                    fprintf(stderr, "Failed to insert string to memory - %s, row number - %d \n", token, row_number);
                } else {
                    /*if label not empty - add to symbol table*/
                    if (strlen(label)) add_to_symbol_table(label, *DC, FALSE, FALSE);
                }
            } else if (strcmp(token, DATA) == 0) {
                /*if label not empty - add to symbol table*/
                if (strlen(label)) add_to_symbol_table(label, *DC, FALSE, FALSE);
                /* split all the numbers by ','*/
                token = strtok(NULL, COMMA_DELIM);
                /* go over the numbers and add them to the data memory block */
                while (token && !is_empty(token)) {
                    /* add them to the memory data section */
                    if (add_int_to_data_memory(token, DC)) {
                        /* Get the next number */
                        token = strtok(NULL, COMMA_DELIM);
                    } else {
                        /* In case there is an error in the memory insert */
                        is_error = TRUE;
                        fprintf(stderr, "Invalid data, row %d  \n", row_number);
                        break;
                    }
                }
            } else if (strcmp(token, STRUCT) == 0) {
                /*if label not empty - add to symbol table*/
                if (strlen(label)) add_to_symbol_table(label, *DC, FALSE, FALSE);
                /*TODO structure*/
            } else {
                printf("Invalid guideline, row %d\n", row_number);
                is_error = TRUE;
            }
        }
            /*if command*/
        else {
            command = commands_lookup(token);
            if (command.value == NOT_FOUND) {
                is_error = TRUE;
                fprintf(stderr, "Unknown command, row: %d\n", row_number);
                continue;
            }
            /*if label not empty - add to symbol table*/
            if (strlen(label)) add_to_symbol_table(label, *IC, FALSE, TRUE);
            /*take the rest of the row - operands*/
            token = strtok(NULL, "");

            is_error = count_IC(IC, token, command.operands_num);
        }
        row_number++;
    }
    return is_error;
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
//                    } else if (strcmp(buf, STRUCTURE) == 0) {
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


/*check is the label valid: start with letter, contains only number and letters*/
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

/*check if the string contains only spaces or tabs*/
int is_empty(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char) *s))
            return 0;
        s++;
    }
    return 1;
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
