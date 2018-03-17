
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "symbol_table.h"
#include "commands.h"
#include "data_block.h"
#include "utils.h"
#include "file_scan.h"

/*File contains methods to scan the input as file: first scan and second scan*/

#define MAX_ROW_LENGTH 81 /* 80 + 1 for \n*/
#define COMMENT_START ';'
#define GUIDELINE_START '.'
#define LABEL_END ':'
char label[MAX_LABEL_SIZE];
int row_number = 0;
char *row; /*to read one row*/
char *token; /*to get part of the row split by space*/
Command command;

/*function prototype*/
int is_label_valid(char *token);

/* helper method: replace tabs by spaces*/
void replace_tabs_by_spaces(char *s) {
    char *i = s;
    while (*i != 0) {
        if (*i == '\t') *i = ' ';
        i++;
    }
}

/*helper method: parse and handle guideline - entry, extern, string, data, struct
 in case of error - return 1, if no errors - 0*/
int handle_guideline(int *DC, char *token) {
    int is_error = FALSE;
    if (strcmp(token, ENTRY) == 0) { /*ignore*/
        if (strlen(label)) fprintf(stderr, "Warning: ignore label before ENTRY, row %d", row_number);
    } else if (strcmp(token, EXTERN) == 0) {
        if (strlen(label)) fprintf(stderr, "Warning: ignore label before EXTERN, row %d", row_number);
        token = strtok(NULL, SPACE_DELIM);
        if (is_label_valid(token)) {
            if (add_to_symbol_table(token, 0, TRUE, FALSE)) {
                fprintf(stderr, "Duplicate label \"%s\", row %d\n", token, row_number);
                is_error = TRUE;
            };
        } else {
            fprintf(stderr, "Invalid EXTERN label, row %d\n", row_number);
            is_error = TRUE;
        }
        token = strtok(NULL, SPACE_DELIM);
        if (token && !is_empty(token)) {
            fprintf(stderr, "Additional data after EXTERN label, row %d\n", row_number);
            is_error = TRUE;
        }
    } else if (strcmp(token, STRING) == 0) {
        /*take rest*/
        token = strtok(NULL, EMPTY_DELIM);
        /*if label not empty - add to symbol table*/
        if (strlen(label)) {
            if (add_to_symbol_table(label, *DC, FALSE, FALSE)) {
                fprintf(stderr, "Duplicate label \"%s\", row %d\n", label, row_number);
                is_error = TRUE;
            }
        }
        if (!token || add_string_to_memory(token, DC)) {
            is_error = TRUE;
            fprintf(stderr, "Failed to insert string to memory - %s, row %d\n", token, row_number);
        }
    } else if (strcmp(token, DATA) == 0) {
        /*if label not empty - add to symbol table*/
        if (strlen(label)) {
            if (add_to_symbol_table(label, *DC, FALSE, FALSE)) {
                fprintf(stderr, "Duplicate label \"%s\", row %d\n", label, row_number);
                is_error = TRUE;
            }
        }
        /* split all the numbers by ','*/
        token = strtok(NULL, COMMA_DELIM);
        /* go over the numbers and add them to the data memory block */
        while (token && !is_empty(token)) {
            /* add them to the memory data section */
            if (!add_int_to_data_memory(token, DC)) {
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
        if (strlen(label)) {
            if (add_to_symbol_table(label, *DC, FALSE, FALSE)) {
                fprintf(stderr, "Duplicate label \"%s\", row %d\n", label, row_number);
                is_error = TRUE;
            }
        }
        /* get the number part*/
        token = strtok(NULL, COMMA_DELIM);
        if (!token || add_int_to_data_memory(token, DC)) {
            /* In case there is an error in the memory insert */
            is_error = TRUE;
            fprintf(stderr, "Invalid number part of struct, row %d \n", row_number);
        } else {
            /* get the string part*/
            token = strtok(NULL, COMMA_DELIM);
            if (!token || add_string_to_memory(token, DC)) {
                is_error = TRUE;
                fprintf(stderr, "Invalid string part of struct, row %d \n", row_number);
            }
        }
    } else {
        fprintf(stderr, "Invalid guideline, row %d\n", row_number);
        is_error = TRUE;
    }
    return is_error;
}

/*helper method: count IC analysing operands of the command*/
int count_IC(int *IC, char *token, int operands_num) {
    int is_error = FALSE, command_offset;
    char *src_op, *dst_op;
    if (operands_num == 0 && (!token || is_empty(token))) { (*IC)++; }
    else if (operands_num == 1 && (token && !is_empty(token))) {
        token = strtok(token, SPACE_DELIM); /*trim spaces*/
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
            src_op = strtok(src_op, SPACE_DELIM); /*trim spaces*/
            dst_op = strtok(dst_op, SPACE_DELIM);
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

/* ------first scan of the file------------
handle labels, count DC for guidelines and IC for commands
in case of error return 1, if no errors - 0*/
int first_scan(FILE *fp, int *IC, int *DC) {
    int is_error = FALSE;
    row = new_string(MAX_ROW_LENGTH);

    /*read line by line*/
    while (fgets(row, MAX_ROW_LENGTH, fp) != NULL) {
        row_number++;
        /*clear label*/
        memset(label, '\0', sizeof(label));

        replace_tabs_by_spaces(row);

        /* skip spaces in the beginning*/
        while (*row == SPACE_DELIM[0]) row++;
        /*if empty string or if comment (start with ;) - ignore*/
        if (*row == COMMENT_START || *row == '\n') continue;

        /*replace end of line*/
        if (row[strlen(row) - 1] == '\n') row[strlen(row) - 1] = END_OF_STRING;

        /*take the first token*/
        token = strtok(row, SPACE_DELIM);
        /*if label*/
        if (token[strlen(token) - 1] == LABEL_END) {
            if (strlen(token) > (MAX_LABEL_SIZE + 1)) {
                fprintf(stderr, "Length of label should be less or equal %d, row: %d\n", MAX_LABEL_SIZE, row_number);
                return FALSE;
            } else {
                /*remove ':' from the end*/
                strncpy(label, token, strlen(token) - 1);
                label[strlen(token) - 1] = END_OF_STRING;
                if (is_label_valid(label)) {
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
        }
        /*if guideline - starts with "."*/
        if (*token == GUIDELINE_START) {
            if (handle_guideline(DC, token)) is_error = TRUE;
        }
            /*if command*/
        else {
            command = commands_lookup(token);
            if (command.opcode == NOT_FOUND) {
                is_error = TRUE;
                fprintf(stderr, "Unknown command, row: %d\n", row_number);
                continue;
            }
            /*if label not empty - add to symbol table*/
            if (strlen(label)) {
                if (add_to_symbol_table(label, *IC, FALSE, TRUE)) {
                    fprintf(stderr, "Duplicate label \"%s\", row %d\n", label, row_number);
                    is_error = TRUE;
                }
            }
            /*take the rest of the row - operands*/
            token = strtok(NULL, EMPTY_DELIM);

            if (count_IC(IC, token, command.operands_num)) is_error = TRUE;
        }
    }
    return is_error;
}

/*------second scan of the file------------
write entry and external file,
prepare instructions array in machine code view
in case of error return 1, if no errors - 0*/
int second_scan(FILE *fp) {
    int is_error = FALSE;
    Symbol *sym;
    row_number = 0;
    row = new_string(MAX_ROW_LENGTH);
    reset_instruction_array_counter();

    /*read line by line*/
    while (fgets(row, MAX_ROW_LENGTH, fp) != NULL) {
        row_number++;
        replace_tabs_by_spaces(row);

        /* skip spaces in the beginning*/
        while (*row == SPACE_DELIM[0]) row++;
        /*if empty string or if comment (start with ;) - ignore*/
        if (*row == COMMENT_START || *row == '\n') continue;

        /*replace end of line*/
        if (row[strlen(row) - 1] == '\n') row[strlen(row) - 1] = END_OF_STRING;

        /*take the first token*/
        token = strtok(row, SPACE_DELIM);

        /* nothing to do with label - get next element*/
        if (token[strlen(token) - 1] == LABEL_END) token = strtok(NULL, SPACE_DELIM);
        /*if guideline - starts with "." - need to handle only ENTRY*/
        if (*token == GUIDELINE_START) {
            if (strcmp(token, ENTRY) == 0) {
                /*find in symbol table*/
                token = strtok(NULL, SPACE_DELIM);
                sym = find_symbol(token);
                if (!sym) {
                    fprintf(stderr, "The label \"%s\" was not declared, row %d \n", token, row_number);
                    is_error = TRUE;
                    continue;
                } else {
                    if (sym->is_extern) {
                        fprintf(stderr, "Error label name \"%s\" is external and can't be entry, row %d\n", token,
                                row_number);
                    } else {
                        write_to_entry_file(sym->name, sym->address);
                    }
                }
            }
            /*if it is command*/
        } else {
            command = commands_lookup(token);
            /*take the rest of the row - operands*/
            token = strtok(NULL, EMPTY_DELIM);
            if (add_instruction_to_array(command.opcode, command.operands_num, token)) is_error = TRUE;
        }
    }
    return is_error;
}

/*helper method: check is the label valid: start with letter, contains only number and letters*/
int is_label_valid(char *token) {
    Command com;

    /*should start with letter, contains only letters(65-90,97-122) or digits(48-57), length not more that 30*/
    char *c = token;
    if (!strlen(token)) {
        fprintf(stderr, "Label can't be empty, row: %d\n", row_number);
        return FALSE;
    }

    if (strlen(token) > (MAX_LABEL_SIZE + 1)) {
        fprintf(stderr, "Length of label should be less or equal %d, row: %d\n", MAX_LABEL_SIZE, row_number);
        return FALSE;
    }

    if (!(*c >= 65 && *c <= 90) && !(*c >= 97 && *c <= 122)) {
        fprintf(stderr, "Label should start with letter, row: %d\n", row_number);
        return FALSE;
    }
    while (*c) {
        if (!(*c >= 65 && *c <= 90) && !(*c >= 97 && *c <= 122) && !(*c >= 48 && *c <= 57)) {
            fprintf(stderr, "Label should contain only letters and numbers, row: %d\n", row_number);
            return FALSE;
        }
        c++;
    }
    /* check reserved words */
    com = commands_lookup(token);
    if (com.opcode != NOT_FOUND) {
        fprintf(stderr, "Can not use reserved word for label, row: %d\n", row_number);
        return FALSE;
    }
    return TRUE;
}