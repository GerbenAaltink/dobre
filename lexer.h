#ifndef LEXER_H
#define LEXER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "buffer.h"
#include <sys/types.h>
#include <ctype.h>

typedef enum token_type_t {
    TOKEN_STRING = 20,
    TOKEN_INT = 21,
    TOKEN_DECIMAL = 22,
    TOKEN_NUMBER =23,
    TOKEN_BOOLEAN = 24,
    TOKEN_SYMBOL = 30,
    TOKEN_PAREN_OPEN =  31,
    TOKEN_PAREN_CLOSE = 32,
    TOKEN_BRACE_OPEN = 33,
    TOKEN_BRACE_CLOSED = 44,
    TOKEN_COMMENT = 50,
    TOKEN_MULTILINE_COMMENT = 51,
    TOKEN_IS = 60,
    TOKEN_DOT = 61,
    TOKEN_WHITESPACE = 1,
    TOKEN_UNKNOWN = 0,
    TOKEN_STAR = 62
} token_type_t;

typedef struct token_t {
    char * value;
    size_t length;
    token_type_t type;
    unsigned int line;
    unsigned int col;
} token_t;

void token_delete(token_t * token){
    if(token->value)
        free(token->value);
    free(token);
}

typedef struct lexer_t {
    token_t ** tokens;
    size_t count;
    char * source;
    buffer_t * buffer;
    unsigned int lines;
} lexer_t;



lexer_t * lexer_new(){
    lexer_t * result = (lexer_t *)calloc(1, sizeof(lexer_t));
    result->count = 0;
    result->buffer = NULL;
    result->source = NULL;
    result->lines = 0;
    //result->length = 0;
    return result;
}

token_t * token_new(){
    token_t * result = (token_t *)calloc(1, sizeof(token_t));
    result->value = NULL;
    return result;
}

bool issymbolchar(char c){
    return c == '_' || isalnum(c);
}
bool isnumberchar(char c){
    return c == '.' || isdigit(c);
}
char * cdup(char c){
    char * result = (char *)malloc(2);
    result[0] = c;
    result[1] = 0;
    return result;
}

token_t * token_next(lexer_t * lexer){
    char c = buffer_pop(lexer->buffer);
    if(c == 0)
    return NULL;
    token_t * token = token_new();
    token->line = lexer->buffer->line;
    token->col = lexer->buffer->col;
    if(c == '{'){
        token->type = TOKEN_PAREN_OPEN;
        token->value = cdup(c);
    }else if(c == '}'){
        token->type = TOKEN_PAREN_CLOSE;
        token->value = cdup(c);
    }else if(isdigit(c) || (c =='-' && isdigit(buffer_peek(lexer->buffer)))){ 
        token->type = TOKEN_NUMBER;
        buffer_t * buffer = buffer_new(NULL,0);
        buffer_push(buffer, c);
        unsigned int dot_count = 0;
        while(isnumberchar((c = buffer_pop(lexer->buffer)))){
            if(c == '.')
            {   
                dot_count++;
                if(dot_count > 1)
                continue;
            }   
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
    }else if(issymbolchar(c)){
        buffer_t * buffer = buffer_new(NULL,0);
        buffer_push(buffer,c);
        while(issymbolchar((c = buffer_pop(lexer->buffer)))){
            buffer_push(buffer,c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_SYMBOL;
    }else if(c == '"'){
        token->type = TOKEN_STRING;
        buffer_t * buffer = buffer_new(NULL,0);
        while((c = buffer_pop(lexer->buffer)) != '"'){
            if(c == '\\'){
                c = buffer_pop(lexer->buffer);
                if(c == 'n')
                    c = '\n';
                else if(c == 't')
                    c = '\t';
                else if(c == 'r')
                    c = '\r';
                else if(c == 'b')
                    c = '\b';
                else if(c == 'f')
                    c = '\f';
                else if(c == 'v')
                    c = '\v';
                else if(c == 'a')
                    c = '\a';
            }
            buffer_push(buffer,c);

        }
        buffer_pop(lexer->buffer);
        token->value = buffer_to_str(buffer);
    }else if(c == '/' && buffer_peek(lexer->buffer) == '/'){
        buffer_pop(lexer->buffer);
        buffer_t * buffer = buffer_new(NULL,0);
        while((c = buffer_pop(lexer->buffer)) != '\n'){
            buffer_push(buffer,c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_COMMENT;
    }else if(c == '/' && buffer_peek(lexer->buffer) == '*'){
        buffer_pop(lexer->buffer);
        buffer_pop(lexer->buffer);
        buffer_t * buffer = buffer_new(NULL,0);
        while((c = buffer_pop(lexer->buffer)) != '\n'){
            buffer_push(buffer,c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_MULTILINE_COMMENT;
        token->value = cdup(c);
    }else if(c == '='){
        token->type = TOKEN_IS;
        token->value = cdup(c);
    }else if(c == '{'){
        token->type = TOKEN_BRACE_OPEN;
        token->value = cdup(c);
    }else if(c == '}'){
        token->type = TOKEN_BRACE_CLOSED;
        token->value = cdup(c);
    }else if(c == '.'){
        token->type = TOKEN_DOT;
        token->value = cdup(c);
    }else if(isspace(c)){
        token->type = TOKEN_WHITESPACE;
        token->value = cdup(c);
    }else if(c == '*'){
        token->type = TOKEN_STAR;
        token->value = cdup(c);   
    }else{
        token->type = TOKEN_UNKNOWN;
        token->value = cdup(c);
    }
    return token;
    
}

void token_dump(token_t * token){
    printf("Type: %d\n",token->type);
    printf("Value: <%s>\n",token->value);
    printf("Position: %d:%d\n",token->line,token->col);
}

void lexer_dump(lexer_t * lexer){
    printf("Token count: %zu\n",lexer->count);
    for(unsigned int i = 0; i < lexer->count;i++){
        token_dump(lexer->tokens[i]);
    }
}

void lexer_delete(lexer_t * lexer){
    if(lexer->buffer)
    buffer_delete(lexer->buffer);
    if(lexer->source)
    free(lexer->source);
    if(lexer->count){
        for(unsigned int i = 0; i < lexer->count;i++){
            token_delete(lexer->tokens[i]);
        }
        free(lexer->tokens);
    }
    free(lexer);
}

void lexer_add_token(lexer_t * lexer, token_t * token){
    lexer->tokens = realloc(lexer->tokens,(lexer->count+1)*sizeof(token_t *));
    lexer->tokens[lexer->count] = token; 
    lexer->count++;
}


lexer_t * lexer_parse(char * source){
    lexer_t * lexer = lexer_new();
    lexer->source = strdup(source);
    lexer->buffer = buffer_new(source,strlen(source));
    while(lexer->buffer->eof == false){
        token_t * token = token_next(lexer);
        if(!token)
            break;
        lexer->lines = token->line;
        lexer_add_token(lexer,token);
    }
    return lexer;
}

#endif 