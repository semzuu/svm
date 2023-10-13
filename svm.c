#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_STACK_SIZE 1024
#define PROG_SIZE (sizeof(prog)/sizeof(INST))

typedef enum {
    INST_PUSH,
    INST_POP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_CMPE,
    INST_CMPNE,
    INST_JMP,
    INST_JMPZ,
    INST_JMPNZ,
    INST_HLT
} INST_TYPE;

typedef enum {
    ERR_DIV_BY_ZERO,
    ERR_STACK_EMPTY,
    ERR_STACK_FULL,
    ERR_FILE_NOT_FOUND
} ERR;

typedef struct {
    INST_TYPE type;
    int value;
}INST;

#define PUSH(x) {.type=INST_PUSH, .value=(x)}
#define POP() {.type=INST_POP}
#define ADD() {.type=INST_ADD}
#define SUB() {.type=INST_SUB}
#define MUL() {.type=INST_MUL}
#define DIV() {.type=INST_DIV}
#define CMPE() {.type=INST_CMPE}
#define CMPNE() {.type=INST_CMPNE}
#define JMP(x) {.type=INST_JMP, .value=(x)}
#define JMPZ(x) {.type=INST_JMPZ, .value=(x)}
#define JMPNZ(x) {.type=INST_JMPNZ, .value=(x)}
#define HLT() {.type=INST_HLT}

typedef struct {
    int stack_size;
    int* stack;
} VM;

INST* load_file(char* filename){
    FILE* f = fopen(filename, "rb");
    if (f == NULL) { 
        err(ERR_FILE_NOT_FOUND); 
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    rewind(f);
    int n = file_size/sizeof(INST);
    INST* prog = (INST*) malloc(sizeof(INST)*n);
    fread( prog, sizeof(INST), n, f);
    fclose(f);
    return prog;
}

void write_file(char* filename, INST* prog, int length){
    FILE* f = fopen(filename, "wb");
    fwrite(prog, sizeof(INST), length, f);
    fclose(f);
    return ;
}


INST prog[] = {
    PUSH(1),
    PUSH(2),
    PUSH(1),
    CMPE(),
    JMPZ(1)
};


int pop(VM* vp){
    vp->stack_size--;
    return (vp->stack[vp->stack_size]);
}

void push(VM* vp, int x){
    vp->stack[vp->stack_size] = x;
    vp->stack_size++;
}

char* err_to_string(ERR type){
    switch(type){
        case ERR_DIV_BY_ZERO: return "ERR_DIV_BY_ZERO";
        case ERR_STACK_EMPTY: return "ERR_STACK_EMPTY";
        case ERR_STACK_FULL: return "ERR_STACK_FULL";
        case ERR_FILE_NOT_FOUND: return "ERR_FILE_NOT_FOUND";
    }
}

#define err(type) fprintf(stderr, "%s\n", err_to_string(type))

int main(){
    VM v = {.stack_size=0, .stack=(int*) malloc(sizeof(int)*MAX_STACK_SIZE) };

    write_file("test.svm", prog, PROG_SIZE);
    INST* program = load_file("test.svm");

    int ip = 0, step;// step is 0 when the ip wasn't modified in the inst, 1 otherwise
    while (ip < PROG_SIZE){
        step = 0;
        int a,b;
        switch(program[ip].type){
            case INST_PUSH:
                if ( v.stack_size == MAX_STACK_SIZE ) { err(ERR_STACK_FULL); exit(1);}
                push(&v, program[ip].value);
                break;
            case INST_POP:
                if ( v.stack_size == 0 ) { err(ERR_STACK_EMPTY); exit(1);}
                pop(&v);
                break;
            case INST_ADD:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                push(&v, a+b);
                break;
            case INST_SUB:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                push(&v, b-a);
                break;
            case INST_MUL:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                push(&v, a*b);
                break;
            case INST_DIV:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                if(a==0) {err(ERR_DIV_BY_ZERO);exit(1);}
                push(&v, b/a);
                break;
            case INST_CMPE:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                push(&v, a==b);
                break;
            case INST_CMPNE:
                assert(v.stack_size>=2);
                a = pop(&v);
                b = pop(&v);
                push(&v, a!=b);
                break;
            case INST_JMP:
                ip = program[ip].value;
                step = 1;
                break;
            case INST_JMPZ:
                if(pop(&v)==0){
                    ip = program[ip].value;
                    step = 1;
                }
                break;
            case INST_JMPNZ:
                if(pop(&v)!=0){
                    ip = program[ip].value;
                    step = 1;
                }
                break;
            case INST_HLT:
                ip = PROG_SIZE;
                step = 1;
                break;
        }
        printf("INST%d:\n", ip);

        if (!step){ ip++; }
        
        for(int i=v.stack_size-1; i>=0; i--) {printf("\t%d--%d\n",i,v.stack[i]);}
        printf("\n");
        sleep(1);
    }
    return 0;
}
