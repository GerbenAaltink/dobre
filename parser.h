#ifndef PARSE_H
#define PARSE_H
#include "array.h"
#include "ast.h"
#include "lexer.h"
#include <stdarg.h>

typedef struct parser_t {
    lexer_t *lexer;
    ast_t *ast;
    token_t *current_token;
    unsigned int token_index;
    array_t *class_list;
} parser_t;

ast_t *parse_class_definition(parser_t *parser);
parser_t *parser_new(lexer_t *lexer) {
    parser_t *result = (parser_t *)malloc(sizeof(parser_t));
    result->lexer = lexer;
    result->ast = NULL;
    result->token_index = 0;
    result->class_list = array_new();
    result->current_token = lexer->count ? lexer->tokens[0] : NULL;
    return result;
}
void parser_delete(parser_t *parser) {
    array_delete(parser->class_list);
    free(parser);
}
token_t *parser_next(parser_t *parser) {
    if (parser->token_index + 1 < parser->lexer->count) {
        parser->current_token = parser->lexer->tokens[parser->token_index + 1];
        parser->token_index++;
        while (parser->current_token->type < 10 &&
               parser->token_index + 1 < parser->lexer->count) {
            parser->current_token =
                parser->lexer->tokens[parser->token_index + 1];
            parser->token_index++;
        }
        return parser->current_token;
    } else {
        parser->current_token = NULL;
    }
    return NULL;
}
void parser_raise(parser_t *parser, char *message, ...) {
    va_list args;
    va_start(args, parser);
    fprintf(stderr, "\033[31m");
    if (parser->current_token) {
        fprintf(stderr,
                "Error at line %d, column %d: ", parser->current_token->line,
                parser->current_token->col);
    }
    vfprintf(stderr, message, args);
    fprintf(stderr, "\033[0m");
    va_end(args);
}
token_t *parser_expect(parser_t *parser, bool required, ...) {
    unsigned int max = 10;
    va_list args;
    va_start(args, required);
    token_t *token = parser->current_token;
    if (!token) {
        return NULL;
    }
    int token_type = -1;
    for (unsigned int i = 0; i < max; i++) {
        token_type = va_arg(args, int);
        if (token_type == -1) {
            break;
        } else if (token->type == token_type) {
            break;
        }
    }
    va_end(args);
    if (token_type == -1 && required) {
        parser_raise(parser, "Syntax error. Unexpected token: %s\n",
                     token->value);
        exit(2);
    }
    return token_type == -1 ? NULL : token;
}

ast_t *parse_remainder(parser_t *parser) {
    parser_next(parser);
    return NULL;
}

ast_t *parse_assignment(parser_t *parser) {
    token_t *token = parser->current_token;
    if (token->type == TOKEN_IS) {
        __attribute__((unused)) ast_assignment_t *assignment =
            ast_assignment_new(token->value, NULL);
        exit(0);
    }
    return parse_remainder(parser);
}

ast_t *parse_variable_definition(parser_t *parser) {
    if (!parser_expect(parser, false, TOKEN_SYMBOL, -1))
        return NULL;
    char *variable_identifiers = "int|char|bool|float|double|void|";
    token_t *token = parser->current_token;
    bool matched_variable_identifier =
        string_match_option(token->value, variable_identifiers);
    if (!matched_variable_identifier) {
        char *user_defined_classes_string =
            array_to_option_string(parser->class_list);
        printf("User defined classes: %s\n", user_defined_classes_string);
        matched_variable_identifier =
            string_match_option(token->value, user_defined_classes_string);
        free(user_defined_classes_string);
    }
    if (matched_variable_identifier == false) {
        parser_expect(parser, true, -1);
        return NULL;
    }

    char *type = token->value;
    unsigned int stars = 0;
    while ((token = parser_next(parser)) && token->type == TOKEN_STAR) {
        stars++;
    }
    if (token->type != TOKEN_SYMBOL) {
        printf("Syntax error?\n");
    }
    assert(token->type == TOKEN_SYMBOL);
    char *identifier = token->value;
    ast_var_declaration_t *definition =
        ast_var_declaration_new(type, identifier);
    definition->stars = stars;
    token = parser_next(parser);
    if (token->type == TOKEN_IS) {
        token = parser_next(parser);
        char *token_type = NULL;
        if (token->type == TOKEN_NUMBER) {
            token_type = "number";
        } else if (token->type == TOKEN_STRING) {
            token_type = "string";
        } else if (token->type == TOKEN_BOOLEAN) {
            token_type = "boolean";
        } else if (token->type == TOKEN_BOOLEAN) {
            token_type = "symbol";
        }
        ast_value_t *value = ast_value_new(token->value, token_type);
        ast_assignment_t *assignment = ast_assignment_new(identifier, value);
        ast_add_child((ast_t *)definition, (ast_t *)assignment);
        parser_next(parser);
    }
    return (ast_t *)definition;
    // return  parse_remainder(parser);
}

ast_t *parse_closure(parser_t *parser) {

    token_t *token = parser->current_token;
    parser_expect(parser, true, TOKEN_PAREN_OPEN, TOKEN_BRACE_OPEN, -1);
    int open_type = token->type;
    int close_type = 0;
    if (open_type == TOKEN_PAREN_OPEN) {
        close_type = TOKEN_PAREN_CLOSE;
    } else if (open_type == TOKEN_BRACE_OPEN) {
        close_type = TOKEN_BRACE_CLOSED;
    }
    if (token->type == TOKEN_BRACE_OPEN || token->type == TOKEN_PAREN_OPEN) {
        ast_t *closure = ast_closure_new();

        token = parser_next(parser);
        while (token->type != close_type) {
            ast_t *statement = parse_class_definition(parser);
            if (statement) {
                ast_add_child((ast_t *)closure, (ast_t *)statement);
            }
            token = parser->current_token;
        }
        parser_expect(parser, true, close_type, -1);
        parser_next(parser);
        return closure;
    }
    return NULL;
}

array_t *parse_class_extends(parser_t *parser,
                             ast_class_definition_t *definition) {
    parser_expect(parser, true, TOKEN_PAREN_OPEN, -1);
    array_t *extends = array_new();
    parser_next(parser);
    while (parser->current_token->type != TOKEN_PAREN_CLOSE) {
        if (!array_push_string(extends, parser->current_token->value)) {
            parser_raise(parser, "Duplicate extend %s < %s\n", definition->name,
                         parser->current_token->value);
        }
        parser_next(parser);
    }
    parser_expect(parser, true, TOKEN_PAREN_CLOSE, -1);
    parser_next(parser);
    return extends;
}

ast_t *parse_class_definition(parser_t *parser) {
    token_t *token = parser->current_token;
    if (token->type == TOKEN_SYMBOL && !strcmp(token->value, "class")) {
        token = parser_next(parser);
        parser_expect(parser, true, TOKEN_SYMBOL, -1);
        ast_class_definition_t *definition =
            ast_class_definition_new(token->value);
        parser_next(parser);
        token =
            parser_expect(parser, true, TOKEN_PAREN_OPEN, TOKEN_BRACE_OPEN, -1);

        if (!token) {
            parser_raise(parser, "Syntax error. Expected '(' or '{'\n");
            exit(2);
        }
        if (token->type == TOKEN_PAREN_OPEN) {
            definition->extends = parse_class_extends(parser, definition);
            assert(definition->extends);
            parser_expect(parser, true, TOKEN_BRACE_OPEN - 1);
            definition->body = parse_closure(parser);
        } else {
            printf("ZZZ:%s\n", parser->current_token->value);

            definition->body = parse_closure(parser);
        }
        assert(definition->body);
        if (!array_push_string(parser->class_list, definition->name)) {
            parser_raise(parser, "Class already defined: %s", definition->name);
            exit(3);
        }
        return (ast_t *)definition;
        // printf("YEAHH!!\n");
    } else {
        ast_t *variable_definition = parse_variable_definition(parser);
        if (variable_definition)
            return variable_definition;
        // parse_remainder(parser);
    }
    return NULL;
}

ast_t *parse(lexer_t *lexer) {
    parser_t *parser = parser_new(lexer);
    token_t *token_before = NULL;
    token_t *token_after = NULL;
    while (parser->current_token) {
        token_before = parser->current_token;
        ast_t *result = parse_class_definition(parser);
        token_after = parser->current_token;
        if (token_after == token_before) {
            printf("Warning: no token change for: \n");
            token_dump(token_before);
            parser_next(parser);
        }
        if (result) {
            ast_delete(result);
        } else {
            parser_expect(parser, -1);
            exit(3);
            token_dump(parser->current_token);
        }
    }
    parser_delete(parser);
    return NULL;
}

char *read_file_contents(char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Couldn't open file: %s\n", filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    __attribute__((unused)) size_t bytes_read = fread(buffer, length, 1, file);
    buffer[length] = 0;
    fclose(file);
    return buffer;
}

void parse_file(char *filepath) {
    char *content = read_file_contents(filepath);
    lexer_t *lexer = lexer_parse(content);

    parse(lexer);
    // lexer_dump(lexer);
    printf("%d lines and %d tokens\n", lexer->lines, lexer->count);
    lexer_delete(lexer);
    free(content);
}
#endif
