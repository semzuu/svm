#include "svm.h"

INST* load_prog(char* filename){
    FILE* f = fopen(filename, "rb");
    if (f == NULL) { 
        err(ERR_FILE_NOT_FOUND); 
        return NULL;
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

void write_prog(char* filename, INST* prog, int length){
    FILE* f = fopen(filename, "wb");
    fwrite(prog, sizeof(INST), length, f);
    fclose(f);
    return ;
}

INST prog[] = {
    PUSH(2),
    PUSH(9),
    PUSH(10)
};

int pop(VM* vp){
    vp->stack_size--;
    return (vp->stack[vp->stack_size]);
}

void push(VM* vp, int x){
    vp->stack[vp->stack_size] = x;
    vp->stack_size++;
}

//TODO: Replace asserts with custom ERRs
char* err_to_string(ERR type){
    switch(type){
        case ERR_DIV_BY_ZERO: return "ERR_DIV_BY_ZERO";
        case ERR_STACK_EMPTY: return "ERR_STACK_EMPTY";
        case ERR_STACK_FULL: return "ERR_STACK_FULL";
        case ERR_FILE_NOT_FOUND: return "ERR_FILE_NOT_FOUND";
    }
}

void execute_prog(VM* v){
    int step;// step is 0 when the ip wasn't modified in the inst, 1 otherwise
    while (v->ip < v->program_size){
        step = 0;
        int16_t a,b;
        switch(v->program[v->ip].type){
            case INST_PUSH:
                if ( v->stack_size == MAX_STACK_SIZE ) { err(ERR_STACK_FULL); exit(1);}
                push(v, v->program[v->ip].value);
                break;
            case INST_POP:
                if ( v->stack_size == 0 ) { err(ERR_STACK_EMPTY); exit(1);}
                pop(v);
                break;
            case INST_ADD:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a+b);
                break;
            case INST_SUB:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a-b);
                break;
            case INST_MUL:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a*b);
                break;
            case INST_DIV:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                if(a==0) {err(ERR_DIV_BY_ZERO);exit(1);}
                push(v, a/b);
                break;
            case INST_MOD:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                if(a==0) {err(ERR_DIV_BY_ZERO);exit(1);}
                push(v, a%b);
                break;
            case INST_CMPE:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a==b);
                break;
            case INST_CMPNE:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a!=b);
                break;
            case INST_CMPL:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a<b);
                break;
            case INST_CMPLE:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a<=b);
                break;
            case INST_CMPG:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a>b);
                break;
            case INST_CMPGE:
                assert(v->stack_size>=2);
                a = pop(v);
                b = pop(v);
                push(v, a>=b);
                break;
            case INST_JMP:
                v->ip = v->program[v->ip].value;
                step = 1;
                break;
            case INST_JMPZ:
                if(pop(v)==0){
                    v->ip = v->program[v->ip].value;
                    step = 1;
                }
                break;
            case INST_JMPNZ:
                if(pop(v)!=0){
                    v->ip = v->program[v->ip].value;
                    step = 1;
                }
                break;
            case INST_DUP:
                push(v, v->stack[v->stack_size-1]);
                break;
            case INST_DUPI:
                push(v, v->stack[v->program[v->ip].value]);
                break;
            case INST_SWP:
                a = v->stack[v->stack_size-1];
                v->stack[v->stack_size-1] = v->stack[v->stack_size-2];
                v->stack[v->stack_size-2] = a;
                break;
            case INST_SWPI:
                a = v->stack[v->stack_size-1];
                v->stack[v->stack_size-1] = v->stack[v->program[v->ip].value];
                v->stack[v->program[v->ip].value] = a;
                break;
            case INST_HLT:
                v->ip = PROG_SIZE;
                step = 1;
                break;
        }
        printf("INST%d:\n", v->ip);

    for(int i=v->stack_size-1; i>=0; i--) {printf("\t%d--%d\n",i,v->stack[i]);}
    printf("\n");
        if (!step){ v->ip++; }
    }
}

void new_VM(VM* vm){
    vm->stack_size = 0; 
    vm->stack = (int16_t*) malloc(sizeof(int16_t)*MAX_STACK_SIZE);
    vm->ip = 0;
    vm->program_size = 0;
    vm->program = (INST*) malloc(sizeof(INST)*MAX_PROG_SIZE);
}

/*
int main(){
    VM v;
    create_VM(&v);

    write_prog("test.svm", prog, PROG_SIZE);
    v.program = load_prog("test.svm");

    execute_prog(&v);

    for(int i=v.stack_size-1; i>=0; i--) {printf("\t%d--%d\n",i,v.stack[i]);}
    printf("\n");
    return 0;
}
*/
