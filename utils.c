#include<stdio.h>
#include <mem.h>
#include <malloc.h>
#include "common.h"

/*File contains different utils for conversion*/

#define BASE_32 32;
const char encoding_strange_32[]= "!@#$%^&*<>abcdefghijklmnopqrstuv";

/*function to convert positive and negative decimal to binary:
 * n - number to convert
 * bits_number - how many bits we could use (first bit needs for sign)
 */
int dec_to_binary(int n, int bits_number) {
    int mask = (1 << (bits_number - 1));
    int result = 0;
    while (mask != 0) {
        result = result * 10 + ((n & mask) != 0);
        mask >>= 1;
    }
    return result;
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

