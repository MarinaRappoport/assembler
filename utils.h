#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

/*check if the string contains only spaces or tabs*/
int is_empty(const char *s);

/*function encode decimal to base 32*/
char *dec_to_base32(int n);

#endif //ASSEMBLER_UTILS_H
