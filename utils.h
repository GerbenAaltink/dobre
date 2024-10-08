#ifndef UTILS_H
#define UTILS_H
#include "parser.h"

size_t get_line_count(char * filepath){
    char * content =read_file_contents(filepath);
    lexer_t * lexer = lexer_parse(content);
    size_t result = lexer->lines;
    lexer_delete(lexer);
    free(content);
    return result;
}









#endif 