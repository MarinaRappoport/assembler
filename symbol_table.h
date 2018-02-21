#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

/*One line in the symbol table*/
typedef struct symbol Symbol;

/*add to symbol table*/
void add_to_symbol_table(Symbol **head, char *name, int address, int is_extern, int is_method);

/*update DC according to IC offset - for all symbols that are NOT method (and not extern):
    address += IC */
void update_data_address(Symbol **head, int offset);

/*go over symbol table to find the symbol ny label name*/
Symbol* find_symbol(Symbol **head, char *name);

#endif //ASSEMBLER_SYMBOL_TABLE_H
