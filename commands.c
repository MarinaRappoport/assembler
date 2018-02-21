

#include <stdio.h>
#include <mem.h>
#include <ctype.h>
#include "common.h"

#define IMMEDIATE_MAX 127
#define IMMEDIATE_MIN -128
#define REGISTER_PREFIX 'r'
#define REGISTER_MAX '7'


/* type of operand address */
typedef enum {
    IMMEDIATE,
    DIRECT,
    STRUCT,
    REGISTER,
    ERROR = -1
} operand_type;

/* type of encoding*/
typedef enum {
    ABSOLUTE, EXTERNAL, RELOCATABLE
} encoding_type;

/* function to check what type of operand is that*/
operand_type parse_operand_type(char *operand) {
    operand_type result;
    int num;
    char label[MAX_LABEL_SIZE], index[1];
    /* immediate value - '#' and number */
    if (sscanf(operand, "#%d", &num) == 1) return IMMEDIATE;
        /* direct(variable) - label name */
    else if (sscanf(operand, "%30[a-zA-Z0-9]", label) == 1) return DIRECT;
        /*TODO check!
        /* struct - label.1 or label.2*/
    else if (sscanf(operand, "%30[a-zA-Z0-9].%1[1-2]", label, index) == 1) return STRUCT;
        /*register r0-r7*/
    else if (strlen(operand) == 2 && *operand == REGISTER_PREFIX
             && isdigit(operand[1]) && (operand[1]) >= '0' && (operand[1]) <= REGISTER_MAX)
        return REGISTER;
        /*not found*/
    else return ERROR;
}


/*function to check what value has the command,
 if command not found return -1*/
int commands_lookup(const char *name) {
    typedef struct command {
        const char *name;
        int value;
    } Command_value;
    Command_value *p;
    Command_value table[] = {
            {"mov",  0},
            {"cmp",  1},
            {"add",  2},
            {"sub",  3},
            {"not",  4},
            {"clr",  5},
            {"lea",  6},
            {"inc",  7},
            {"dec",  8},
            {"jmp",  9},
            {"bne",  10},
            {"red",  11},
            {"prn",  12},
            {"jsr",  13},
            {"rts",  14},
            {"stop", 15}
    };
    for (p = table; p->name != NULL; ++p) {
        if (strcmp(p->name, name) == 0) {
            return p->value;
        }
    }
    return -1;
}
