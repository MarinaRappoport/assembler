#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol_table.h"

/*File contains all methods for handling symbol table.
 Symbol table is a linked list*/

Symbol *head = NULL; /*pointer to beginning of symbol table*/

/*add to symbol table, always - to the head*/
int add_to_symbol_table(char *name, int address, int is_extern, int is_method) {
    Symbol *pnt;
    Symbol *new = (Symbol *) malloc(sizeof(Symbol));
    new->name = (char *) malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->address = address;
    new->is_method = is_method;
    new->is_extern = is_extern;
    new->next = NULL;

    if (head == NULL) {
        head = new;     /*when linked list is empty*/
    } else {
        pnt = head;/*assign head to pnt*/
        while (pnt->next != NULL) {
            if (strcmp(pnt->name, name) == 0) return EXIT_FAILURE;
            pnt = pnt->next;/*traverse the list until the last node*/
        }
        if (strcmp(pnt->name, name) == 0) return EXIT_FAILURE;
        pnt->next = new;/*point the previous last node to the new node created*/
    }
    return EXIT_SUCCESS;
}

/*update DC according to IC offset - for all symbols that are NOT method (and not extern):
    address += IC */
void update_data_address(int offset) {
    Symbol *pnt = head;
    while (pnt) {
        /*Check if the label is data and not external*/
        if (!pnt->is_method && !pnt->is_extern) {
            pnt->address += offset;
        }
        pnt = pnt->next;
    }
}

/*go over symbol table to find the symbol ny label name*/
Symbol *find_symbol(char *name) {
    Symbol *pnt = head;
    while (pnt) {
        if (strcmp(pnt->name, name) == 0) return pnt;
        pnt = pnt->next;
    }
    return NULL;
}

/*delete all data from symbol table and free memory*/
void free_symbol_table() {
    Symbol *pnt = head, *tmp;
    while (pnt) {
        tmp = pnt;
        pnt = pnt->next;
        free(tmp->name);
        free(tmp);
    }
    head=NULL;
}

