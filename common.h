#ifndef _COMMON_H_
#define _COMMON_H_

enum boolean {FALSE, TRUE};

/*function to check what value has the command,
 if command not found return -1*/
int commans_lookup(const char *name);

/*register r1-8*/

/*max size of the line - 80 - not include \n*/

/*guidelines*/
#define EXTERNAL "external"
#define ENTRY "entry"
#define DATA "data"
#define STRING "string"
#define STRUCT "struct"

#endif
