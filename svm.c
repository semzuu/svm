#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_STACK_SIZE 1024
#define PROG_SIZE (sizeof(program)/sizeof(program[0]))

typedef enum {
    INST_PUSH,
    INST_POP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV
} INST_TYPE;

typedef enum {
    ERR_DIV_BY_ZERO,
    ERR_STACK_EMPTY,
    ERR_STACK_FULL
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

typedef struct {
    int stack_size;
    int* stack;
} VM;

/*
INST* load_file(char* filename){
    FILE* f;
    f = fopen(filename, "rb");
    if (f == NULL) { err(ERR_FILE_NOT_FOUND); exit(1);}
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(f);
    int n = file_size/sizeof(INST);
    INST* prog = (INST*) malloc(sizeof(INST)*n);
    for (int i=0; i< n; i++){
        fread( &(prog[i]), sizeof(INST), 1, f);
    }
}

void write_file(char* filename){
    FILE* f;
    f = fopen(filename, "wb");
    if (f == NULL) { exit(1);}

}
*/

INST program[] = {
    PUSH(10),
    PUSH(5),
    ADD(),
    PUSH(8),
    SUB(),
    PUSH(2),
    MUL(),
    PUSH(2),
    DIV(),
    POP()
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
    }
}

#define err(type) fprintf(stderr, "%s\n", err_to_string(type))

int main(){
    VM v = {.stack_size=0, .stack=(int*) malloc(sizeof(int)*MAX_STACK_SIZE) };
    for(int i=0; i<PROG_SIZE; i++){
        int a,b;
        switch(program[i].type){
            case INST_PUSH:
                if ( v.stack_size == MAX_STACK_SIZE ) { err(ERR_STACK_FULL); exit(1);}
                push(&v, program[i].value);
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
        }

        for(int i=0; i<v.stack_size; i++) {printf("%d--%d\n",i,v.stack[i]);}
        printf("\n");
    }
    return 0;
}
