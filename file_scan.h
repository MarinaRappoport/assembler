#ifndef ASSEMBLER_SCAN_H
#define ASSEMBLER_SCAN_H

/*first scan of the file
1. if empty string - ignore
2. if comment (start with ;) - ignore
3. if .entry - ignore
4. if .extern - add to symbol table (name, isExtern-true)
5. if label (check if label valid) - ends with : - put to the symbol table (name, address - IC|DC, isExterm, isMethod);
    - if entry/extern - ignore label
    - if data/ struct/ string - add to symbol table + count DC + save binary
    - if command - add to symbol table + count IC
6. Without label:
- if data/ struct/ string - count DC + save binary
- if command - count IC
in case of error return 1, if no errors - 0*/
int first_scan(FILE *fp, int *IC, int *DC);

/*second scan of the file
1. if empty string - ignore
2. if comment (start with ;) - ignore
3. if .entry - to file .ent: name(label) + address(in base 32)
4. if .extern - ignore
5. NOTHING to do with labels!
6. if command - build code in binary:
    0000   -  10 - 01 - 00
    opcode - op1 - op2 - AER
    ?operators: 00 - number
                01 - variable(label)
                10 - struct
                11 - register
    ?AER:	 00 - absolute
            01 - external
            10 - relocated

    for struct: 1) address(of label)->8 bits - 10 (relocated) - because it depends on IC
                2) number inside struct: 1 - 00000001-00, 2 - 00000010-00

    for variables - check in symbol table:
                1) if extern: 00000000-01
                + to file .ext: name(label) + address=IC - of method it uses(in base 32)
in case of error return 1, if no errors - 0*/
int second_scan(FILE *fp);

#endif /*ASSEMBLER_SCAN_H*/
