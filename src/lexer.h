#ifndef LEXER_H
#define LEXER_H

#include "buffer.h"
#include "string.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct lexer_t {
    token_t **tokens;
    size_t count;
    char *source;
    buffer_t *buffer;
    unsigned int lines;
} lexer_t;

lexer_t *lexer_new() {
    lexer_t *result = (lexer_t *)calloc(1, sizeof(lexer_t));
    result->count = 0;
    result->buffer = NULL;
    result->source = NULL;
    result->lines = 0;
    // result->length = 0;
    return result;
}

bool issymbolchar(char c) { return c == '_' || isalnum(c); }
bool isnumberchar(char c) { return c == '.' || isdigit(c); }

char *cdup(char c) {
    char *result = (char *)malloc(2);
    result[0] = c;
    result[1] = 0;
    return result;
}

token_t *lexer_next(lexer_t *lexer) {
    char c = buffer_pop(lexer->buffer);
    if (c == 0)
        return NULL;
    token_t *token = token_new();
    token->line = lexer->buffer->line;
    token->col = lexer->buffer->col;
    token->index = lexer->buffer->pos;
    if (c == '{') {
        token->type = TOKEN_BRACE_OPEN;
        token->value = cdup(c);
    } else if (c == '}') {
        token->type = TOKEN_BRACE_CLOSE;
        token->value = cdup(c);
    } else if (isdigit(c) ||
               (c == '-' && isdigit(buffer_peek(lexer->buffer)))) {
        token->type = TOKEN_NUMBER;
        buffer_t *buffer = buffer_new(NULL, 0);
        buffer_push(buffer, c);
        unsigned int dot_count = 0;
        while (isnumberchar((c = buffer_peek(lexer->buffer)))) {
            c = buffer_pop(lexer->buffer);
            if (c == '.') {
                dot_count++;
                if (dot_count > 1)
                    continue;
            }
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
    } else if (issymbolchar(c)) {
        buffer_t *buffer = buffer_new(NULL, 0);
        buffer_push(buffer, c);
        while (issymbolchar((c = buffer_peek(lexer->buffer)))) {
            c = buffer_pop(lexer->buffer);
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
        if (!strcmp(token->value, "while")) {
            token->type = TOKEN_WHILE;
        } else if (!strcmp(token->value, "for")) {
            token->type = TOKEN_FOR;
        } else if (!strcmp(token->value, "class")) {
            token->type = TOKEN_CLASS;
        } else {
            token->type = TOKEN_SYMBOL;
        }
    } else if (c == '"') {
        token->type = TOKEN_STRING;
        buffer_t *buffer = buffer_new(NULL, 0);
        while ((c = buffer_pop(lexer->buffer)) != '"') {
            if (c == '\\') {
                c = buffer_pop(lexer->buffer);
                if (c == 'n')
                    c = '\n';
                else if (c == 't')
                    c = '\t';
                else if (c == 'r')
                    c = '\r';
                else if (c == 'b')
                    c = '\b';
                else if (c == 'f')
                    c = '\f';
                else if (c == 'v')
                    c = '\v';
                else if (c == 'a')
                    c = '\a';
            }
            buffer_push(buffer, c);
        }
        buffer_pop(lexer->buffer);
        token->value = buffer_to_str(buffer);
    } else if(c == ','){
        token->value = cdup(c);
        token->type = TOKEN_COMMA;
    } else if (c == '#') {
        buffer_pop(lexer->buffer);
        buffer_t *buffer = buffer_new(NULL, 0);
        while ((c = buffer_peek(lexer->buffer)) != '\n') {
            c = buffer_pop(lexer->buffer);
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_MACRO;
    } else if (c == '/' && buffer_peek(lexer->buffer) == '/') {
        buffer_pop(lexer->buffer);
        buffer_t *buffer = buffer_new(NULL, 0);
        while ((c = buffer_peek(lexer->buffer)) != '\n') {
            c = buffer_pop(lexer->buffer);
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_COMMENT;
    } else if (c == '/' && buffer_peek(lexer->buffer) == '*') {
        buffer_pop(lexer->buffer);
        buffer_pop(lexer->buffer);
        buffer_t *buffer = buffer_new(NULL, 0);
        while (true) {
            c = buffer_pop(lexer->buffer);
            if (c == '*') {
                if (buffer_peek(lexer->buffer) == '/') {
                    buffer_pop(lexer->buffer);
                    break;
                }
            }
            buffer_push(buffer, c);
        }
        token->value = buffer_to_str(buffer);
        token->type = TOKEN_MULTILINE_COMMENT;
    } else if (c == '<') {
        if (buffer_peek(lexer->buffer) == '=') {
            buffer_pop(lexer->buffer);
            token->type = TOKEN_LTE;
            token->value = strdup("<=");
        } else {
            token->type = TOKEN_LT;
            token->value = cdup(c);
        }
    } else if (c == '!') {
        if (buffer_peek(lexer->buffer) == '=') {
            buffer_pop(lexer->buffer);
            token->type = TOKEN_NOT_EQUAL;
            token->value = strdup("!=");
        } else {
            token->type = TOKEN_NOT;
            token->value = cdup(c);
        }
    } else if (c == '>') {
        if (buffer_peek(lexer->buffer) == '=') {
            buffer_pop(lexer->buffer);
            token->type = TOKEN_GTE;
            token->value = strdup(">=");
        } else {
            token->type = TOKEN_GT;
            token->value = cdup(c);
        }
    } else if (c == '=') {
        if (buffer_peek(lexer->buffer) == '=') {
            buffer_pop(lexer->buffer);
            token->type = TOKEN_EQUAL;
            token->value = strdup("==");
        } else {
            token->type = TOKEN_IS;
            token->value = cdup(c);
        }
    } else if (c == '{') {
        token->type = TOKEN_BRACE_OPEN;
        token->value = cdup(c);
    } else if (c == '}') {
        token->type = TOKEN_BRACE_CLOSE;
        token->value = cdup(c);
    } else if (c == '.') {
        token->type = TOKEN_DOT;
        token->value = cdup(c);
    } else if (c == '\n' || c == '\t' || c == ' ') {
        token->type = TOKEN_WHITESPACE;
        token->value = cdup(c);
    } else if (c == '*') {
        token->type = TOKEN_STAR;
        token->value = cdup(c);
    } else if (c == '(') {
        token->type = TOKEN_PAREN_OPEN;
        token->value = cdup(c);
    } else if (c == ')') {
        token->type = TOKEN_PAREN_CLOSE;
        token->value = cdup(c);
    } else if (c == '{') {
        token->type = TOKEN_BRACE_OPEN;
        token->value = cdup(c);
    } else if (c == '}') {
        token->type = TOKEN_BRACE_CLOSE;
        token->value = cdup(c);
    } else {
        token->type = TOKEN_UNKNOWN;
        token->value = cdup(c);
    }
    if (token->value)
        token->length = strlen(token->value);
    return token;
}

void lexer_dump(lexer_t *lexer) {
    printf("Token count: %zu\n", lexer->count);
    for (unsigned int i = 0; i < lexer->count; i++) {
        token_dump(lexer->tokens[i]);
    }
}

void lexer_delete(lexer_t *lexer) {
    if (lexer->buffer)
        buffer_delete(lexer->buffer);
    if (lexer->source)
        free(lexer->source);
    if (lexer->count) {
        for (unsigned int i = 0; i < lexer->count; i++) {
            token_delete(lexer->tokens[i]);
        }
        free(lexer->tokens);
    }
    free(lexer);
}

void lexer_add_token(lexer_t *lexer, token_t *token) {
    lexer->tokens =
        realloc(lexer->tokens, (lexer->count + 1) * sizeof(token_t *));
    lexer->tokens[lexer->count] = token;
    lexer->count++;
}

lexer_t *lexer_parse(lexer_t *lexer, char *source) {
    lexer->source = strdup(source);
    lexer->buffer = buffer_new(source, strlen(source));
    token_t *token;
    while (lexer->buffer->eof == false && (token = lexer_next(lexer))) {
        lexer_add_token(lexer, token);
    }
    if (lexer->count)
        lexer->lines = lexer->tokens[lexer->count - 1]->line + 1;
    return lexer;
}

#endif
