#ifndef SVM
#define SVM

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define MAX_STACK_SIZE 1024
#define MAX_PROG_SIZE 1024
#define PROG_SIZE (sizeof(prog)/sizeof(INST))

typedef enum {
    INST_PUSH,
    INST_POP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_MOD,
    INST_NOT,
    INST_AND,
    INST_OR,
    INST_XOR,
    INST_SHL,
    INST_SHR,
    INST_ROL,
    INST_ROR,
    INST_CMPE,
    INST_CMPNE,
    INST_CMPL,
    INST_CMPLE,
    INST_CMPG,
    INST_CMPGE,
    INST_JMP,
    INST_JMPZ,
    INST_JMPNZ,
    INST_DUP,
    INST_DUPI,
    INST_SWP,
    INST_SWPI,
    INST_HLT
} INST_TYPE;

#define PUSH(x)  {.type=INST_PUSH, .value=(x)}
#define POP()    {.type=INST_POP}
#define ADD()    {.type=INST_ADD}
#define SUB()    {.type=INST_SUB}
#define MUL()    {.type=INST_MUL}
#define DIV()    {.type=INST_DIV}
#define MOD()    {.type=INST_MOD}
#define NOT()    {.type=INST_NOT}
#define AND()    {.type=INST_AND}
#define OR()     {.type=INST_OR}
#define XOR()    {.type=INST_XOR}
#define SHL(x)   {.type=INST_SHL, .value=(x)}
#define SHR(x)   {.type=INST_SHR, .value=(x)}
#define ROL(x)   {.type=INST_ROL, .value=(x)}
#define ROR(x)   {.type=INST_ROR, .value=(x)}
#define CMPE()   {.type=INST_CMPE}
#define CMPNE()  {.type=INST_CMPNE}
#define CMPL()   {.type=INST_CMPL}
#define CMPLE()  {.type=INST_CMPLE}
#define CMPG()   {.type=INST_CMPG}
#define CMPGE()  {.type=INST_CMPGE}
#define JMP(x)   {.type=INST_JMP, .value=(x)}
#define JMPZ(x)  {.type=INST_JMPZ, .value=(x)}
#define JMPNZ(x) {.type=INST_JMPNZ, .value=(x)}
#define DUP()    {.type=INST_DUP}
#define DUPI(x)  {.type=INST_DUPI, .value=(x)}
#define SWP()    {.type=INST_SWP}
#define SWPI(x)  {.type=INST_SWPI, .value=(x)}
#define HLT()    {.type=INST_HLT}

typedef enum {
    ERR_DIV_BY_ZERO,
    ERR_STACK_EMPTY,
    ERR_STACK_FULL,
    ERR_FILE_NOT_FOUND
} ERR;

typedef struct {
    INST_TYPE type;
    int16_t value;
}INST;

typedef struct {
    int16_t stack_size;
    int16_t* stack;
    int16_t program_size;
    INST* program;
    int16_t ip;
} VM;

void new_VM(VM* vm);
INST* load_prog(char* filename);
void write_prog(char* filename, INST* prog, int length);
int pop(VM* vp);
void push(VM* vp, int x);
void execute_prog(VM* v);
char* err_to_string(ERR type);
    
#define err(type) fprintf(stderr, "%s\n", err_to_string(type))

#endif
