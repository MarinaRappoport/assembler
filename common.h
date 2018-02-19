#ifndef _COMMON_H_
#define _COMMON_H_

#define CODE_ADDRESS_BASE 100
enum boolean {FALSE, TRUE};
enum result {SUCCESS, ERROR};

/*function to check what value has the command,
 if command not found return -1*/
int commands_lookup(const char *name);

/*register r1-8*/

/*max size of the line - 80 - not include \n*/

/*guidelines*/
#define EXTERNAL "external"
#define ENTRY "entry"
#define DATA "data"
#define STRING "string"
#define STRUCT "struct"

#endif

