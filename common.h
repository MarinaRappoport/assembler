#ifndef _COMMON_H_
#define _COMMON_H_

#define AS_FILE_EXT ".as"
#define OBJECT_FILE_EXT ".ob"
#define ENTRY_FILE_EXT ".ent"
#define EXTERN_FILE_EXT ".ext"

#define CODE_ADDRESS_BASE 100
#define MEMORY_SIZE 256            /*Size of instruction/data block (max number of lines)*/
#define MAX_LABEL_SIZE 30
#define SPACE_DELIM " "
#define COMMA_DELIM ","
#define EMPTY_DELIM ""
#define END_OF_STRING '\0'
enum boolean {FALSE, TRUE};
#define NOT_FOUND (-1)

/* type of encoding*/
typedef enum {
    ABSOLUTE, EXTERNAL, RELOCATABLE
} encoding_type;

/*guidelines*/
#define EXTERN ".extern"
#define ENTRY ".entry"
#define DATA ".data"
#define STRING ".string"
#define STRUCT ".struct"

#endif

