#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "commands.h"
#include "utils.h"
#include "symbol_table.h"

/*File contains methods to handle  with commands(instruction) block of code*/

int instructions_block[MEMORY_SIZE];        /* array to hold machine commands of instructions block*/
int counter = 0; /*to count instructions added to array*/

/* table of allowed commands:
    command name, opcode, number of operands*/
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
    int num; /*for immediate and struct*/
    char label[MAX_LABEL_SIZE];
    /* immediate value - '#' and number */
    if (sscanf(operand, "#%d", &num) == 1) {
        if (num < IMMEDIATE_MIN || num > IMMEDIATE_MAX) {
            fprintf(stderr, "Can't use as operand number %d, it's out or range %d - %d\n", num, IMMEDIATE_MIN,
                    IMMEDIATE_MAX);
            return ERROR;
        } else return IMMEDIATE;
    }
        /*register r0-r7*/
    else if (strlen(operand) == 2 && *operand == REGISTER_PREFIX
             && isdigit(operand[1]) && (operand[1]) >= '0' && (operand[1]) <= REGISTER_MAX)
        return REGISTER;
        /* struct - label.1 or label.2*/
    else if (sscanf(operand, "%30[a-zA-Z0-9].%d", label, &num) == 2 && (num == 1 || num == 2)) return STRUCTURE;
        /* direct(variable) - label name */
    else if (sscanf(operand, "%30[a-zA-Z0-9]", label) == 1) return DIRECT;
        /*not found*/
    else return ERROR;
}

/* calculate memory (in rows) - IC offset - for the command*/
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


/* function to find command by name,
 if command not found return fake command with value -1*/
Command commands_lookup(const char *name) {
    Command command = {"", NOT_FOUND, NOT_FOUND};
    Command *p;
    int i = 0;
    for (p = command_table; i < sizeof(command_table) / sizeof(Command); i++, ++p) {
        if (strcmp(p->name, name) == 0) {
            return *p;
        }
    }
    return command;
}

/* function builds machine command for instruction
0000   -  10     - 01    - 00
opcode - src op - dst op - AER*/
int build_machine_command(int opcode, operand_type src_type, operand_type dst_type) {
    int machine_command;
    /*opcode - 4 bits*/
    machine_command = opcode;

    /*enum types are unsigned int by default*/
    /* destination address type - 2 bits */
    machine_command = (machine_command << 2) + src_type;
    /*destination address type - 2 bits */
    machine_command = (machine_command << 2) + dst_type;

    /*AER - 2 bits - for first instruction row AER always '00' */
    machine_command = (machine_command << 2);
    return machine_command;
}

/*  function adds operands value to machine code instruction array
    if label: find in symbol table - check if exist? (error)
    if external - write to external file, AER = external
    else if not method - AER = relocatable*/
int add_operand_to_instruction_array(char *oper, operand_type type, int is_src, int is_src_reg) {
    int num;
    char label[MAX_LABEL_SIZE];
    Symbol *sym;
    if (oper) {
        switch (type) {
            case IMMEDIATE:
                sscanf(oper, "#%d", &num);
                instructions_block[counter++] = (num << 2) + ABSOLUTE;
                break;
            case DIRECT:
                sym = find_symbol(oper);
                if (!sym) {
                    fprintf(stderr, "Unknown symbol %s\n", oper);
                    return EXIT_FAILURE;
                } else if (sym->is_extern) {
                    /*write to external file*/
                    write_to_external_file(oper, CODE_ADDRESS_BASE + counter);
                    instructions_block[counter++] = EXTERNAL;
                } else {
                    instructions_block[counter++] = (sym->address << 2) + RELOCATABLE;
                }
                break;
            case REGISTER:
                sscanf(oper, "r%d", &num);
                /*source register */
                if (is_src) instructions_block[counter++] = (num << (4 + 2)) + ABSOLUTE;
                    /*destination register*/
                else {
                    if (is_src_reg) {
                        /*will write in the same line*/
                        counter--;
                        instructions_block[counter] = instructions_block[counter] + (num << 2) + ABSOLUTE;
                        counter++;
                    } else {
                        instructions_block[counter++] = (num << 2) + ABSOLUTE;
                    }
                }
                break;
            case STRUCTURE:
                sscanf(oper, "%30[a-zA-Z0-9].%d", label, &num);
                sym = find_symbol(label);
                if (!sym) {
                    fprintf(stderr, "Unknown symbol %s\n", label);
                    return EXIT_FAILURE;
                } else if (sym->is_extern) {
                    /*write to external file*/
                    write_to_external_file(label, CODE_ADDRESS_BASE + counter);
                    instructions_block[counter++] = EXTERNAL;
                } else {
                    instructions_block[counter++] = (sym->address << 2) + RELOCATABLE;
                }
                instructions_block[counter++] = (num << 2) + ABSOLUTE;
                break;
            default:
                break;
        }
    }
    return EXIT_SUCCESS;
}

/* function adds instruction (command and its operands) to instructions_block (in machine code) */
int add_instruction_to_array(int opcode, int operands_num, char *operands) {
    int is_error = FALSE;
    char *src_op = NULL;
    char *dst_op = NULL;
    operand_type src_type = IMMEDIATE;
    operand_type dst_type = IMMEDIATE;
    if (operands_num == 1) {
        dst_op = strtok(operands, SPACE_DELIM); /*trim spaces*/
    } else if (operands_num == 2) {
        /*remove whitespaces and separate by ','*/
        src_op = strtok(operands, COMMA_DELIM);
        dst_op = strtok(NULL, COMMA_DELIM);
        src_op = strtok(src_op, SPACE_DELIM); /*trim spaces*/
        dst_op = strtok(dst_op, SPACE_DELIM);
    }
    if (src_op != NULL) src_type = parse_operand_type(src_op);
    if (dst_op != NULL) dst_type = parse_operand_type(dst_op);

    /*add instruction in machine code and operands*/
    instructions_block[counter++] = build_machine_command(opcode, src_type, dst_type);
    if (add_operand_to_instruction_array(src_op, src_type, TRUE, FALSE)) is_error = TRUE;
    if (add_operand_to_instruction_array(dst_op, dst_type, FALSE, src_type == REGISTER)) is_error = TRUE;
    return is_error;
}

/* function writes all the instruction to the object file */
void write_instructions_to_object_file(int IC) {
    int i;/*index in the instructions_block*/
    int start_address = CODE_ADDRESS_BASE;
    for (i = 0; i < IC - start_address; i++)
        write_to_object_file(start_address + i, instructions_block[i]);
}

/* set counter to 0*/
void reset_instruction_array_counter() {
    counter = 0;
}


