#ifndef ASSEMBLER_DATA_BLOCK_H
#define ASSEMBLER_DATA_BLOCK_H

/*function adds integer to data block, checks if number valid,
 * in case of error return 1, if no errors - 0*/
int add_int_to_data_memory(char *str_number, int *DC);

/*function adds string to data block (each letter to its own cell), checks if the string valid,
 * in case of error return 1, if no errors - 0*/
int add_string_to_memory(char *str, int *DC);

/*function writes DC address and machine code of data block to object file */
void write_data_to_object_file(int IC, int DC);

#endif /*ASSEMBLER_DATA_BLOCK_H*/
