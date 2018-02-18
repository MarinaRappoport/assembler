#include<stdio.h>
#include <mem.h>
#include <malloc.h>
#include "common.h"

/*File contains different utils for conversion*/

#define BASE_32 32;
const char encoding_strange_32[]= "!@#$%^&*<>abcdefghijklmnopqrstuv";

/*function to convert decimal to binary*/
long dec_to_binary(int n) {
    int remainder;
    long binary = 0, i = 1;

    while (n != 0) {
        remainder = n % 2;
        n = n / 2;
        binary = binary + (remainder * i);
        i = i * 10;
    }
    return binary;
}

/*function to check what value has the command,
 if command not found return -1*/
int commans_lookup(const char *name) {
    typedef struct command {
        const char *name;
        int value;
    } Command;
    Command *p;
    Command table[] = {
            {"mov",  0},
            {"cmp",  1},
            {"add",  2},
            {"sub",  3},
            {"not",  4},
            {"clr",  5},
            {"lea",  6},
            {"inc",  7},
            {"dec",  8},
            {"jmp",  9},
            {"bne",  10},
            {"red",  11},
            {"prn",  12},
            {"jsr",  13},
            {"rts",  14},
            {"stop", 15}
    };
    for (p = table; p->name != NULL; ++p) {
        if (strcmp(p->name, name) == 0) {
            return p->value;
        }
    }
    return -1;
}

/*function encode decimal to base 32*/
char *dec_to_base32(int n) {
    /*array to store base_32 numbers in reverse order*/
    int a[10];
    int i, j = 0; /*counter for array*/
    char *encoded;
    for (i = 0; n > 0; i++) {
        /* storing remainder in binary array*/
        a[i] = n % BASE_32;
        n = n / BASE_32;
    }
    /* +1 - to keep end of string*/
    encoded = (char *) malloc((i + 1) * 8);
    while (i--) {
        encoded[j++] = encoding_strange_32[a[i]];
    }
    encoded[j] = '\0';
    return encoded;
}

