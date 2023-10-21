#include "lexer.h"
#include "svm.h"
#include <stdio.h>
#include <stdlib.h>

TOKEN new_token(TOKEN_TYPE type, char* text, int character, int line){
    TOKEN temp = {.type=type, .text=text, .character=character, .line=line};
    return temp;
}

char* load_file(char* file_path){
    FILE* f = fopen(file_path, "r");
    if (f == NULL) { fprintf(stderr, "FILE_NOT_FOUND: %s", file_path); exit(1); }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    int length = size / sizeof(char);
    rewind(f);
    char* src = (char*) malloc(sizeof(char)*length);
    fread(src, sizeof(char), length, f);
    fclose(f);
    return src;
}

TOKEN_TYPE check_inst(char* keyword){
    if (strcmp(keyword, "push")==0){
        return TYPE_PUSH;
    } else if (strcmp(keyword, "pop")==0){
        return TYPE_POP;
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

TOKEN generate_keyword(SOURCE_CODE* src){
    TOKEN temp;
    char* keyword = (char*) malloc(sizeof(char)*10);
    int current_index = 0;
    while ((isalpha(src->text[src->index])) && (src->index < strlen(src->text))){
        keyword[current_index] = src->text[src->index];
        current_index++;
        (src->index)+=1;
    }
    keyword[current_index] = '\0';
    TOKEN_TYPE type = check_inst(keyword);
    if (type!=TYPE_NULL){
        temp = new_token(type, keyword, src->character, src->line);
        (src->character)+=current_index;
    } else{
        fprintf(stderr, "UNKNOWN: %s\n", keyword);
    }
    return temp;
}

TOKEN generate_number(SOURCE_CODE* src){
    TOKEN temp;
    char* number = (char*) malloc(sizeof(char)*16);
    int current_index = 0;
    while ((isdigit(src->text[src->index])) && (src->index < strlen(src->text))){
        number[current_index] = src->text[src->index];
        current_index++;
        (src->index)+=1;
    }
    number[current_index] = '\0';
    temp = new_token(TYPE_INT, number, src->character, src->line);
    (src->character)+=current_index;
    return temp;
}

void tokenize(SOURCE_CODE src, LEXER* lex){
    for (src.index = 0; src.index < strlen(src.text);){
        if (isalpha(src.text[src.index])){
            lex->tokens[lex->length] = generate_keyword(&src);
            lex->length++;
        } else if (isdigit(src.text[src.index])){
            lex->tokens[lex->length] = generate_number(&src);
            lex->length++;
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

void generate_code(LEXER lex, VM* vm){
    INST instruction;
    int i = 0;
    while(i < lex.length){
        switch (lex.tokens[i].type) {
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
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_PUSH;
                break;
            case TYPE_POP:
                instruction.type = INST_POP;
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
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_JMP;
                break;
            case TYPE_JMPZ:
                i++;
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_JMPZ;
                break;
            case TYPE_JMPNZ:
                i++;
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_JMPNZ;
                break;
            case TYPE_DUP:
                instruction.type = INST_DUP;
                instruction.value = 0;
                break;
            case TYPE_DUPI:
                i++;
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_DUPI;
                break;
            case TYPE_SWP:
                instruction.type = INST_SWP;
                instruction.value = 0;
                break;
            case TYPE_SWPI:
                i++;
                if (lex.tokens[i].type != TYPE_INT) {
                    fprintf(stderr, "NOT_EXPECTED_TYPE\n");
                    exit(1);
                }
                instruction.value = atoi(lex.tokens[i].text);
                instruction.type = INST_SWPI;
                break;
            case TYPE_HLT:
                instruction.type = INST_HLT;
                instruction.value = 0;
                break;
        }
        i++;
        vm->program[vm->program_size] = instruction;
        vm->program_size+=1;
    }
}

#define print_token(x) printf("[%d:%d] %s\n", (x).line, (x).character, (x).text)
#define print_inst(inst, value) printf("TYPE: %d  VALUE: %d\n", inst, value);

void print_tokens(LEXER lex){
    for(int i = 0; i < lex.length; i++){
        print_token(lex.tokens[i]);
    }
}

void print_insts(INST* prog){
    for(int i = 0; i < 3; i++){
        print_inst(prog[i].type, prog[i].value);
    }
}

SOURCE_CODE new_code(){
    SOURCE_CODE temp;
    temp.line = 0;
    temp.character = 0;
    temp.index = 0;
    temp.text = NULL;
    return temp;
}

void new_lexer(LEXER* lex){
    lex->length = 0;
    lex->tokens = (TOKEN*) malloc(sizeof(TOKEN)*MAX_TOKENS);
}
/*
int main(){
    SOURCE_CODE src = new_code();
    src.text = load_file("test.sasm");
    LEXER lex;
    new_lexer(&lex);
    tokenize(src, &lex);
    //printf("%d\n",lex.length);
    //print_tokens(lex);
    VM v;
    new_VM(&v);
    generate_code(lex, &v);
    print_insts(v.program);
    execute_prog(&v);

    return 0;
}*/
