#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

/*allocate memory for new string*/
char *new_string(int length);

/*check if the string contains only spaces or tabs*/
int is_empty(const char *s);

/* save filename for all output files */
void save_file_name(char *fileName);

/* write label and address (in 32-encoding) to Entry file*/
void write_to_entry_file(char *label, int address);

/* write label and address (in 32-encoding) to External file*/
void write_to_external_file(char *label, int address);

/* write first row to Object file - size of IC and DC in 32-encoding*/
void write_first_line_to_object_file(int IC_size, int DC_size);

/* write row to Object file - address and machine code in 32-encoding*/
void write_to_object_file(int address, int machineCode);

/* close all output files*/
void close_output_files();

/* delete all output files*/
void delete_output_files();

#endif /*ASSEMBLER_UTILS_H*/
