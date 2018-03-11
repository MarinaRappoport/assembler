#ifndef ASSEMBLER_COMMANDS_H
#define ASSEMBLER_COMMANDS_H

#define IMMEDIATE_MAX 127
#define IMMEDIATE_MIN -128
#define REGISTER_PREFIX 'r'
#define REGISTER_MAX '7'

typedef struct command {
    const char *name;
    int value;
    int operands_num;
} Command;

/* type of operand address */
typedef enum {
    IMMEDIATE,
    DIRECT, /*variable, label*/
    STRUCTURE,
    REGISTER,
    ERROR = -1
} operand_type;

/* type of encoding*/
typedef enum {
    ABSOLUTE, EXTERNAL, RELOCATABLE
} encoding_type;

/* Represents single machine command*/
typedef struct {
    int command_value;
    operand_type src_type;
    operand_type dst_type;
    int src_reg;
    int dst_reg;
    int type;
} machine_command;

/* Calculate memory (in rows) - IC offset - for the command*/
int calculate_command_offset(char *src_op, char *dst_op, int row_num);

/*function to find command by name,
 if command not found return fake command with value -1*/
Command commands_lookup(const char *name);

#endif //ASSEMBLER_COMMANDS_H
