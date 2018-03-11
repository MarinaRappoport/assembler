

#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include "common.h"
#include "commands.h"

Command command_table[] = {
        {"mov",  0,  2},
        {"cmp",  1,  2},
        {"add",  2,  2},
        {"sub",  3,  2},
        {"not",  4,  1},
        {"clr",  5,  1},
        {"lea",  6,  2},
        {"inc",  7,  1},
        {"dec",  8,  1},
        {"jmp",  9,  1},
        {"bne",  10, 1},
        {"red",  11, 1},
        {"prn",  12, 1},
        {"jsr",  13, 1},
        {"rts",  14},
        {"stop", 15}
};

/* function to check what type of operand is that*/
operand_type parse_operand_type(char *operand) {
    int num;
    int index; /*for struct*/
    char label[MAX_LABEL_SIZE];
    /* immediate value - '#' and number */
    if (sscanf(operand, "#%d", &num) == 1) return IMMEDIATE;
        /*register r0-r7*/
    else if (strlen(operand) == 2 && *operand == REGISTER_PREFIX
             && isdigit(operand[1]) && (operand[1]) >= '0' && (operand[1]) <= REGISTER_MAX)
        return REGISTER;
        /* struct - label.1 or label.2*/
    else if (sscanf(operand, "%30[a-zA-Z0-9].%d", label, &index) == 2 && (index == 1 || index == 2)) return STRUCTURE;
        /* direct(variable) - label name */
    else if (sscanf(operand, "%30[a-zA-Z0-9]", label) == 1) return DIRECT;
        /*not found*/
    else return ERROR;
}

/* Calculate memory (in rows) - IC offset - for the command*/
int calculate_command_offset(char *src_op, char *dst_op, int row_num) {
    int src_offset = 0, dst_offset = 0;
    operand_type src_type = ERROR, dst_type;

    /*source operand*/
    if (src_op != NULL) {
        src_type = parse_operand_type(src_op);
        if (src_type == ERROR) {
            fprintf(stderr, "Invalid source operand %s,  row: %d \n", src_op, row_num);
            return ERROR;
        }
        if (src_type == STRUCTURE) src_offset = 2;
        else src_offset = 1; /*for all other*/
    }

    /*destination operand*/
    dst_type = parse_operand_type(dst_op);
    if (dst_type == ERROR) {
        fprintf(stderr, "Invalid destination operand %s, row: %d \n", dst_op, row_num);
        return ERROR;
    }
    if (dst_type == STRUCTURE) dst_offset = 2;
    else dst_offset = 1; /*for all other*/

    /* if both registers - no need to extra row for destination */
    if (src_type == REGISTER && dst_type == REGISTER) dst_offset = 0;

    /* + 1 for the command itself */
    return src_offset + dst_offset + 1;
}


/*function to find command by name,
 if command not found return fake command with value -1*/
Command commands_lookup(const char *name) {
    Command command = {"", NOT_FOUND, NOT_FOUND};
    Command *p;
    for (p = command_table; p->name != NULL; ++p) {
        if (strcmp(p->name, name) == 0) {
            return *p;
        }
    }
    return command;
}


