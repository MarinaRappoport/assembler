#include <mem.h>
#include <malloc.h>
#include <stdio.h>

#include "symbol_table.h"

/*File contains all methods for handling symbol table*/

/*One line in the symbol table*/
struct symbol {
    char *name;
    int address;
    int is_method; /*1 - method, 0 - */
    int is_extern; /*1 - extern, 0 - e*/
    struct symbol *next;
};

/*add to symbol table*/
void add_to_symbol_table(Symbol **head, char *name, int address, int is_extern, int is_method) {
    Symbol *new = (Symbol *) malloc(sizeof(Symbol));
    strcpy(new->name, name);
    new->address = address;
    new->is_method = is_method;
    new->is_extern = is_extern;
    /*add first*/
    (*head) = new;
    new->next = (*head)->next;
}

/*update DC according to IC offset - for all symbols that are NOT method (and not extern):
    address += IC */
void update_data_address(Symbol **head, int offset) {
    Symbol *pnt = (*head);
    while (pnt)
    {
        /*Check if the label is data and not external*/
        if(!pnt->is_method && !pnt->is_extern)
        {
            pnt->address += offset;
        }
        pnt=pnt->next;
    }
}

/*go over symbol table to find the symbol ny label name*/
Symbol* find_symbol(Symbol **head, char *name) {
    Symbol *pnt = (*head);
    while (pnt)
    {
        if(strcmp(pnt->name, name) == 0)
        {
            return pnt;
        }
    }
    printf (stderr, "Error: can not find label %s ", name);
    return NULL;
}

