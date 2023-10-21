#ifndef COMPILER
#define COMPILER

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "svm.h"

#define MAX_TOKENS 1000

typedef enum {
    TYPE_NULL,
    TYPE_INT,
    TYPE_PUSH,
    TYPE_POP,
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_MOD,
    TYPE_NOT,
    TYPE_AND,
    TYPE_OR,
    TYPE_XOR,
    TYPE_SHL,
    TYPE_SHR,
    TYPE_ROL,
    TYPE_ROR,
    TYPE_CMPE,
    TYPE_CMPNE,
    TYPE_CMPL,
    TYPE_CMPLE,
    TYPE_CMPG,
    TYPE_CMPGE,
    TYPE_JMP,
    TYPE_JMPZ,
    TYPE_JMPNZ,
    TYPE_DUP,
    TYPE_DUPI,
    TYPE_SWP,
    TYPE_SWPI,
    TYPE_HLT
} TOKEN_TYPE;

typedef struct {
    char* text;
    int index;
    int character;
    int line;
} SOURCE_CODE;

typedef struct {
    TOKEN_TYPE type;
    char* text;
    int character;
    int line;
} TOKEN;

typedef struct {
    TOKEN* tokens;
    int length;
} LEXER;


TOKEN new_token(TOKEN_TYPE type, char* text, int character, int line);
char* load_file(char* file_path);
TOKEN_TYPE check_inst(char* keyword);
TOKEN generate_keyword(SOURCE_CODE* src);
TOKEN generate_number(SOURCE_CODE* src);
void tokenize(SOURCE_CODE src, LEXER* lex);
void generate_code(LEXER lex, VM* vm);
SOURCE_CODE new_code();
void new_lexer(LEXER* lex);

#endif
