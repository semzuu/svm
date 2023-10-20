#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
    } else if (strcmp(keyword, "jmp")==0){
        return TYPE_JMP;
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

#define print_token(x) printf("[%d:%d] %s\n", (x).line, (x).character, (x).text)

void print_tokens(LEXER lex){
    for(int i = 0; i < lex.length; i++){
        print_token(lex.tokens[i]);
    }
}

int main(){
    SOURCE_CODE src = {.index = 0, .character = 0, .line = 0};
    src.text = load_file("test.sasm");
    LEXER lex;
    lex.tokens = (TOKEN*) malloc(sizeof(TOKEN)*MAX_TOKENS);
    lex.length = 0;
    tokenize(src, &lex);
    printf("%d\n",lex.length);
    print_tokens(lex);
    return 0;
}
