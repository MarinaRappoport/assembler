#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

/*One line in the symbol table*/
typedef struct symbol {
    char *name;
    int address;
    int is_method; /*1 - method, 0 - */
    int is_extern; /*1 - extern, 0 - e*/
    struct symbol *next;
} Symbol;

/*add to symbol table*/
int add_to_symbol_table(char *name, int address, int is_extern, int is_method);

/*update DC according to IC offset - for all symbols that are NOT method (and not extern):
    address += IC */
void update_data_address(int offset);

/*go over symbol table to find the symbol ny label name*/
Symbol *find_symbol(char *name);

/*delete all data from symbol table and free memory*/
void free_symbol_table();

#endif /*ASSEMBLER_SYMBOL_TABLE_H*/
