#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

int isalpha(int ch){ return ((ch>=65)&&(ch<=90)||(ch>=97)&&(ch<=122)); }
int isdigit(int ch){ return ((ch>=48)&&(ch<=57)); }

#define MAX_STACK_SIZE 1024
#define MAX_PROG_SIZE 1024
#define PROG_SIZE (sizeof(prog)/sizeof(INST))
#define MAX_TOKENS 1000
int DEBUG = 0;

typedef enum {
    INST_PUSH,
    INST_POP,
    INST_READ,
    INST_READC,
    INST_WRITE,
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
#define READ()   {.type=INST_READ}
#define READC()  {.type=INST_READC}
#define WRITE()  {.type=INST_WRITE}
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

//TODO: Replace asserts with custom ERRs
char* err_to_string(ERR type){
    switch(type){
        case ERR_DIV_BY_ZERO: return "ERR_DIV_BY_ZERO";
        case ERR_STACK_EMPTY: return "ERR_STACK_EMPTY";
        case ERR_STACK_FULL: return "ERR_STACK_FULL";
        case ERR_FILE_NOT_FOUND: return "ERR_FILE_NOT_FOUND";
    }
}

#define err(type) fprintf(stderr, "%s\n", err_to_string(type))

typedef struct {
    INST_TYPE type;
    int16_t value;
}INST;

class STACK{
    public:
        int16_t* bp;
        int16_t sp;
        STACK(){
            this->bp = (int16_t*) malloc(sizeof(int16_t)*MAX_STACK_SIZE);
            this->sp = 0;
        }
        ~STACK() = default;
        void push(int16_t value){
            bp[(sp)++] = value;
        }
        int16_t pop(){
            sp--;
            return (bp[sp]);
        }
};

class PROGRAM{
    public:
        int16_t ip;
        int16_t size;
        INST* code;
        PROGRAM(){
            this->ip = 0;
            this->size = 0;
            this->code = (INST*) malloc(sizeof(INST)*MAX_PROG_SIZE);
        }
        ~PROGRAM() = default;
        void load(char* filepath){
            FILE* file = fopen(filepath, "rb");
            if (file == NULL) {
                err(ERR_FILE_NOT_FOUND);
            }
            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            rewind(file);
            size = file_size / sizeof(INST);
            fread(code, sizeof(INST), size, file);
            fclose(file);
        }
        void write(char* filepath){
            FILE* file = fopen(filepath, "wb");
            fwrite(code, sizeof(INST), size, file);
            fclose(file);
        }
};

typedef enum {
    TYPE_NULL,
    TYPE_INT,
    TYPE_PUSH,
    TYPE_POP,
    TYPE_READ,
    TYPE_READC,
    TYPE_WRITE,
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

class TOKEN{
    public:
        TOKEN_TYPE type;
        char* text;
        int character;
        int line;
        TOKEN(){
            this->type = TYPE_NULL;
            this->text = NULL;
            this->character = 0;
            this->line = 0;
        }
        TOKEN(TOKEN_TYPE type, char* text, int character, int line){
            this->type = type;
            this->text = text;
            this->character = character;
            this->line = line;
        }
};

class SOURCE_CODE{
    public:
        int index;
        char* text;
        int character;
        int line;
        SOURCE_CODE(){
            this->index = 0;
            this->text = NULL;
            this->character = 0;
            this->line = 0;
        }
        void load_file(char* filepath){
            FILE* file = fopen(filepath, "r");
            if (file == NULL) { fprintf(stderr, "FILE_NOT_FOUND: %s", filepath); exit(1); }
            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            int length = file_size / sizeof(char);
            rewind(file);
            text = (char*) malloc(sizeof(char)*length);
            fread(text, sizeof(char), length, file);
            fclose(file);
        }
        TOKEN_TYPE check_inst(char* keyword){
            if (strcmp(keyword, "push")==0){
                return TYPE_PUSH;
            } else if (strcmp(keyword, "pop")==0){
                return TYPE_POP;
            } else if (strcmp(keyword, "read")==0){
                return TYPE_READ;
            } else if (strcmp(keyword, "readc")==0){
                return TYPE_READC;
            } else if (strcmp(keyword, "write")==0){
                return TYPE_WRITE;
            } else if (strcmp(keyword, "add")==0){
                return TYPE_ADD;
            } else if (strcmp(keyword, "sub")==0){
                return TYPE_SUB;
            } else if (strcmp(keyword, "mul")==0){
                return TYPE_MUL;
            } else if (strcmp(keyword, "div")==0){
                return TYPE_DIV;
            } else if (strcmp(keyword, "mod")==0){
                return TYPE_MOD;
            } else if (strcmp(keyword, "not")==0){
                return TYPE_NOT;
            } else if (strcmp(keyword, "and")==0){
                return TYPE_AND;
            } else if (strcmp(keyword, "or")==0){
                return TYPE_OR;
            } else if (strcmp(keyword, "xor")==0){
                return TYPE_XOR;
            } else if (strcmp(keyword, "shl")==0){
                return TYPE_SHL;
            } else if (strcmp(keyword, "shr")==0){
                return TYPE_SHR;
            } else if (strcmp(keyword, "rol")==0){
                return TYPE_ROL;
            } else if (strcmp(keyword, "ror")==0){
                return TYPE_ROR;
            } else if (strcmp(keyword, "cmpe")==0){
                return TYPE_CMPE;
            } else if (strcmp(keyword, "cmpne")==0){
                return TYPE_CMPNE;
            } else if (strcmp(keyword, "cmpl")==0){
                return TYPE_CMPL;
            } else if (strcmp(keyword, "cmple")==0){
                return TYPE_CMPLE;
            } else if (strcmp(keyword, "cmpg")==0){
                return TYPE_CMPG;
            } else if (strcmp(keyword, "cmpge")==0){
                return TYPE_CMPGE;
            } else if (strcmp(keyword, "jmp")==0){
                return TYPE_JMP;
            } else if (strcmp(keyword, "jmpz")==0){
                return TYPE_JMPZ;
            } else if (strcmp(keyword, "jmpnz")==0){
                return TYPE_JMPNZ;
            } else if (strcmp(keyword, "dup")==0){
                return TYPE_DUP;
            } else if (strcmp(keyword, "dupi")==0){
                return TYPE_DUPI;
            } else if (strcmp(keyword, "swp")==0){
                return TYPE_SWP;
            } else if (strcmp(keyword, "swpi")==0){
                return TYPE_SWPI;
            } else if (strcmp(keyword, "hlt")==0){
                return TYPE_HLT;
            } else {
                return TYPE_NULL;
            }
        }
        TOKEN generate_keyword(){
            TOKEN temp;
            char* keyword = (char*) malloc(sizeof(char)*10);
            int current_index = 0;
            int code_length = strlen(text);
            while ((isalpha(text[index])) && (index < code_length)){
                keyword[current_index] = text[index];
                current_index++;
                index++;
            }
            keyword[current_index] = '\0';
            TOKEN_TYPE type = check_inst(keyword);
            if (type!=TYPE_NULL){
                temp = TOKEN(type, keyword, character, line);
                (character)+=current_index;
            } else{
                fprintf(stderr, "UNKNOWN: %s\n", keyword);
            }
            return temp;
        }
        TOKEN generate_number(){
            TOKEN temp;
            char* number = (char*) malloc(sizeof(char)*16);
            int current_index = 0;
            int code_length = strlen(text);
            while ((isdigit(text[index])) && (index < code_length)){
                number[current_index] = text[index];
                current_index++;
                (index)++;
            }
            number[current_index] = '\0';
            temp = TOKEN(TYPE_INT, number, character, line);
            (character)+=current_index;
            return temp;
        }
};

class LEXER{
    public:
        TOKEN* tokens;
        int numberOfTokens;
        LEXER(){
            this->numberOfTokens = 0;
            this->tokens = (TOKEN*) malloc(sizeof(TOKEN)*MAX_TOKENS);
        }
        void tokenize(SOURCE_CODE src){
            int code_length = strlen(src.text);
            for (src.index = 0; src.index < code_length;){
                if (isalpha(src.text[src.index])){
                    tokens[numberOfTokens] = src.generate_keyword();
                    numberOfTokens++;
                } else if (isdigit(src.text[src.index])){
                    tokens[numberOfTokens] = src.generate_number();
                    numberOfTokens++;
                } else if (src.text[src.index] == ' '){
                    src.character++;
                    src.index++;
                } else if (src.text[src.index] == '\n'){
                    src.line++;
                    src.character = 0;
                    src.index++;
                } else{
                    fprintf(stderr, "UNKNOWN_TYPE: %c\n", src.text[src.index]);
                }
            }
        }
        PROGRAM generate_code(){
            PROGRAM program = PROGRAM();
            INST instruction;
            int i = 0;
            while(i < numberOfTokens){
                switch (tokens[i].type) {
                    case TYPE_NULL:
                        fprintf(stderr, "SYNTAX_ERR\n");
                        exit(1);
                        break;
                    case TYPE_INT:
                        fprintf(stderr, "SYNTAX_ERR\n");
                        exit(1);
                        break;
                    case TYPE_PUSH:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_PUSH;
                        break;
                    case TYPE_POP:
                        instruction.type = INST_POP;
                        instruction.value = 0;
                        break;
                    case TYPE_READ:
                        instruction.type = INST_READ;
                        instruction.value = 0;
                        break;
                    case TYPE_READC:
                        instruction.type = INST_READC;
                        instruction.value = 0;
                        break;
                    case TYPE_WRITE:
                        instruction.type = INST_WRITE;
                        instruction.value = 0;
                        break;
                    case TYPE_ADD:
                        instruction.type = INST_ADD;
                        instruction.value = 0;
                        break;
                    case TYPE_SUB:
                        instruction.type = INST_SUB;
                        instruction.value = 0;
                        break;
                    case TYPE_MUL:
                        instruction.type = INST_MUL;
                        instruction.value = 0;
                        break;
                    case TYPE_DIV:
                        instruction.type = INST_DIV;
                        instruction.value = 0;
                        break;
                    case TYPE_MOD:
                        instruction.type = INST_MOD;
                        instruction.value = 0;
                        break;
                    case TYPE_NOT:
                        instruction.type = INST_NOT;
                        instruction.value = 0;
                        break;
                    case TYPE_AND:
                        instruction.type = INST_AND;
                        instruction.value = 0;
                        break;
                    case TYPE_OR:
                        instruction.type = INST_OR;
                        instruction.value = 0;
                        break;
                    case TYPE_XOR:
                        instruction.type = INST_XOR;
                        instruction.value = 0;
                        break;
                    case TYPE_SHL:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_SHL;
                        break;
                    case TYPE_SHR:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_SHR;
                        break;
                    case TYPE_ROL:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_ROL;
                        break;
                    case TYPE_ROR:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_ROR;
                        break;
                    case TYPE_CMPE:
                        instruction.type = INST_CMPE;
                        instruction.value = 0;
                        break;
                    case TYPE_CMPNE:
                        instruction.type = INST_CMPNE;
                        instruction.value = 0;
                        break;
                    case TYPE_CMPL:
                        instruction.type = INST_CMPL;
                        instruction.value = 0;
                        break;
                    case TYPE_CMPLE:
                        instruction.type = INST_CMPLE;
                        instruction.value = 0;
                        break;
                    case TYPE_CMPG:
                        instruction.type = INST_CMPG;
                        instruction.value = 0;
                        break;
                    case TYPE_CMPGE:
                        instruction.type = INST_CMPGE;
                        instruction.value = 0;
                        break;
                    case TYPE_JMP:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_JMP;
                        break;
                    case TYPE_JMPZ:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_JMPZ;
                        break;
                    case TYPE_JMPNZ:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_JMPNZ;
                        break;
                    case TYPE_DUP:
                        instruction.type = INST_DUP;
                        instruction.value = 0;
                        break;
                    case TYPE_DUPI:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_DUPI;
                        break;
                    case TYPE_SWP:
                        instruction.type = INST_SWP;
                        instruction.value = 0;
                        break;
                    case TYPE_SWPI:
                        i++;
                        if (tokens[i].type != TYPE_INT) {
                            fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                            exit(1);
                        }
                        instruction.value = atoi(tokens[i].text);
                        instruction.type = INST_SWPI;
                        break;
                    case TYPE_HLT:
                        instruction.type = INST_HLT;
                        instruction.value = 0;
                        break;
                }
                i++;
                program.code[program.size] = instruction;
                program.size++;
            }
            return program;
        }
};

class VM{
    private:
        STACK stack;
    public:
        PROGRAM program;
        LEXER lexer;
        VM(){
            stack = STACK();
            program = PROGRAM();
            lexer = LEXER();
        }
        ~VM() = default;
        void execute(){
            int step;
            // step is 0 when the ip wasn't modified in the inst, 1 otherwise
            while (program.ip < program.size){
                step = 0;
                int16_t a,b;
                switch(program.code[program.ip].type){
                    case INST_PUSH:
                        if ( stack.sp == MAX_STACK_SIZE ) { err(ERR_STACK_FULL); exit(1);}
                        stack.push(program.code[program.ip].value);
                        break;
                    case INST_POP:
                        if ( stack.sp == 0 ) { err(ERR_STACK_EMPTY); exit(1);}
                        stack.pop();
                        break;
                    case INST_READ:
                        if ( stack.sp == 0 ) { err(ERR_STACK_EMPTY); exit(1);}
                        a = stack.pop();
                        printf("%d\n",a);
                        break;
                    case INST_READC:
                        if ( stack.sp == 0 ) { err(ERR_STACK_EMPTY); exit(1);}
                        a = stack.pop();
                        printf("%c",a);
                        break;
                    case INST_WRITE:
                        if ( stack.sp == MAX_STACK_SIZE ) { err(ERR_STACK_FULL); exit(1);}
                        scanf("%hd", &a);
                        stack.push(a);
                        break;
                    case INST_ADD:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a+b);
                        break;
                    case INST_SUB:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a-b);
                        break;
                    case INST_MUL:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a*b);
                        break;
                    case INST_DIV:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        if(a==0) {err(ERR_DIV_BY_ZERO);exit(1);}
                        stack.push(a/b);
                        break;
                    case INST_MOD:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        if(a==0) {err(ERR_DIV_BY_ZERO);exit(1);}
                        stack.push(a%b);
                        break;
                    case INST_NOT:
                        assert(stack.sp>=1);
                        a = stack.pop();
                        stack.push(~a);
                        break;
                    case INST_AND:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a&b);
                        break;
                    case INST_OR:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a|b);
                        break;
                    case INST_XOR:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a^b);
                        break;
                    case INST_SHL:
                        assert(stack.sp>=1);
                        a = stack.pop();
                        stack.push(a<<program.code[program.ip].value);
                        break;
                    case INST_SHR:
                        assert(stack.sp>=1);
                        a = stack.pop();
                        stack.push(a>>program.code[program.ip].value);
                        break;
                    case INST_ROL:
                        assert(stack.sp>=1);
                        a = stack.pop();
                        stack.push((a<<program.code[program.ip].value)|(a>>(sizeof(a)-program.code[program.ip].value)));
                        break;
                    case INST_ROR:
                        assert(stack.sp>=1);
                        a = stack.pop();
                        stack.push((a>>program.code[program.ip].value)|(a<<(sizeof(a)-program.code[program.ip].value)));
                        break;
                    case INST_CMPE:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a==b);
                        break;
                    case INST_CMPNE:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a!=b);
                        break;
                    case INST_CMPL:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a<b);
                        break;
                    case INST_CMPLE:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a<=b);
                        break;
                    case INST_CMPG:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a>b);
                        break;
                    case INST_CMPGE:
                        assert(stack.sp>=2);
                        a = stack.pop();
                        b = stack.pop();
                        stack.push(a>=b);
                        break;
                    case INST_JMP:
                        program.ip = program.code[program.ip].value;
                        step = 1;
                        break;
                    case INST_JMPZ:
                        if(stack.pop()==0){
                            program.ip = program.code[program.ip].value;
                            step = 1;
                        }
                        break;
                    case INST_JMPNZ:
                        if(stack.pop()!=0){
                            program.ip = program.code[program.ip].value;
                            step = 1;
                        }
                        break;
                    case INST_DUP:
                        stack.push(stack.bp[stack.sp-1]);
                        break;
                    case INST_DUPI:
                        stack.push(stack.bp[program.code[program.ip].value]);
                        break;
                    case INST_SWP:
                        a = stack.bp[stack.sp-1];
                        stack.bp[stack.sp-1] = stack.bp[stack.sp-2];
                        stack.bp[stack.sp-2] = a;
                        break;
                    case INST_SWPI:
                        a = stack.bp[stack.sp-1];
                        stack.bp[stack.sp-1] = stack.bp[program.code[program.ip].value];
                        stack.bp[program.code[program.ip].value] = a;
                        break;
                    case INST_HLT:
                        program.ip = program.size;
                        step = 1;
                        break;
                }
                if(DEBUG){
                    debug_print();
                }

                if (!step){ (program.ip)++; }
            }
        }
        void debug_print(){
            printf("\n");
            printf("INST%d:\n", program.ip);
            for(int i=stack.sp-1; i>=0; i--) {printf("\t%d/ %d\n",i,stack.bp[i]);}
            printf("\n");
        }
};

char* pop_arg(int* argc, char*** argv){
    char* temp = **argv;
    (*argv)++;
    (*argc)--;
    return temp;
}

int main(int argc, char** argv){

    char* program = pop_arg(&argc, &argv);
    char* filepath;
    char* arg;
    while(argc>0){
        arg = pop_arg(&argc, &argv);
        if(strcmp(arg, "--debug")==0){ DEBUG = 1; }
        else{ filepath = arg; }
    }

    VM vm = VM(); printf("VM created\n");
    SOURCE_CODE src;
    src.load_file(filepath);
    vm.lexer.tokenize(src);
    vm.program = vm.lexer.generate_code();
    vm.program.write(strcat(filepath, ".svm"));
    vm.program.load(filepath);
    vm.execute();

    return 0;
}
