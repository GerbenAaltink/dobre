#ifndef TOKEN_H
#define TOKEN_H
#include <stdlib.h>

typedef enum token_type_t {
    TOKEN_STRING = 20,
    TOKEN_INT = 21,
    TOKEN_DECIMAL = 22,
    TOKEN_NUMBER = 23,
    TOKEN_BOOLEAN = 24,
    TOKEN_SYMBOL = 30,
    TOKEN_PAREN_OPEN = 31,
    TOKEN_PAREN_CLOSE = 32,
    TOKEN_BRACE_OPEN = 33,
    TOKEN_BRACE_CLOSE = 34,
    TOKEN_COMMENT = 2,
    TOKEN_MULTILINE_COMMENT = 3,
    TOKEN_IS = 60,
    TOKEN_DOT = 61,
    TOKEN_WHITESPACE = 1,
    TOKEN_UNKNOWN = 0,
    TOKEN_STAR = 62,
    TOKEN_EQUAL = 63,
    TOKEN_LT = 64,
    TOKEN_LTE = 65,
    TOKEN_GT = 66,
    TOKEN_GTE = 67,
    TOKEN_NOT_EQUAL = 68,
    TOKEN_NOT = 69,
    TOKEN_MACRO = 4,
    TOKEN_WHILE = 50,
    TOKEN_FOR = 51,
    TOKEN_CLASS = 52,
    TOKEN_COMMA = 70
} token_type_t;

typedef struct token_t {
    char *value;
    size_t length;
    token_type_t type;
    unsigned int line;
    unsigned int col;
    unsigned int index;
} token_t;

token_t *token_new() {
    token_t *result = (token_t *)calloc(1, sizeof(token_t));
    result->value = NULL;
    return result;
}

void token_delete(token_t *token) {
    if (token->value)
        free(token->value);
    free(token);
}

void token_dump(token_t *token) {
    printf("Type: %d\n", token->type);
    printf("Value: <%s>\n", token->value);
    printf("Position: %d:%d\n", token->line, token->col);
}

#endif
