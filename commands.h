#ifndef ASSEMBLER_COMMANDS_H
#define ASSEMBLER_COMMANDS_H

/* Calculate memory (in rows) - IC offset - for the command*/
int calculate_command_offset(char *src_op, char *dst_op, int row_num);

typedef struct command {
    const char *name;
    int value;
    int operands_num;
} Command;

/*function to find command by name,
 if command not found return fake command with value -1*/
Command commands_lookup(const char *name);

#endif //ASSEMBLER_COMMANDS_H
