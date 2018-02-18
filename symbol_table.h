#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

/*One line in the symbol table*/
typedef struct symbol Symbol;

/*add to symbol table*/
void add_to_symbol_table(Symbol **head, char *name, int address, int is_extern, int is_method);

#endif //ASSEMBLER_SYMBOL_TABLE_H
