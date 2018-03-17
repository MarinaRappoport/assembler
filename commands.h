#ifndef ASSEMBLER_COMMANDS_H
#define ASSEMBLER_COMMANDS_H

#define IMMEDIATE_MAX 127
#define IMMEDIATE_MIN (-128)
#define REGISTER_PREFIX 'r'
#define REGISTER_MAX '7'

typedef struct command {
    const char *name;
    int opcode;
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

/* calculate memory (in rows) - IC offset - for the command*/
int calculate_command_offset(char *src_op, char *dst_op, int row_num);

/*function to find command by name,
 if command not found return fake command with value -1*/
Command commands_lookup(const char *name);

/* function adds instruction (command and its operands) to instructions_block (in machine code) */
int add_instruction_to_array(int opcode, int operands_num, char *operands);

/* function writes all the instruction between the start address and the end address to the object file*/
void write_instructions_to_object_file(int IC);

/* set counter to 0*/
void reset_instruction_array_counter();

#endif /*ASSEMBLER_COMMANDS_H*/
