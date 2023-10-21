#include "svm.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    
    VM v;
    new_VM(&v);
    
    char file_path[255];
    printf("File to load: ");
    scanf("%s", file_path);
    SOURCE_CODE src = new_code();
    src.text = load_file(file_path);

    LEXER lex;
    new_lexer(&lex);
    tokenize(src, &lex);
    generate_code(lex, &v);
    execute_prog(&v);
    

    return 0;
}
