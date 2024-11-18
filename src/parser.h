#ifndef PARSE_H
#define PARSE_H
#include "dict.h"
#include "array.h"
#include "ast.h"
#include "lexer.h"
#include <stdarg.h>

typedef struct parser_t {
    lexer_t *lexer;
    ast_t *ast;
    token_t *current_token;
    token_t *previous_token;
    unsigned int token_index;
    array_t *class_list;
} parser_t;

ast_t *parse_closure(parser_t *parser, token_type_t type);
token_t *parser_next(parser_t *parser);
token_t *parser_peek(parser_t *parser);
ast_t *parse_class_definition(parser_t *parser);
ast_t *parse_variable_definition(parser_t *parser);
ast_t *parse_statement(parser_t *parser);
ast_t *parse_expression(parser_t *parser);
ast_t *parse_assignment(parser_t *parser);
ast_value_t *parse_value(parser_t *parser);
dict_t * parse_params(parser_t *parser);

unsigned int get_token_index(parser_t *parser, token_t *token) {
    for (unsigned i = 0; i < parser->lexer->count; i++) {
        if (parser->lexer->tokens[i] == token) {
            return i;
        }
    }
    return 0;
}

char *parser_get_line_by_number(parser_t *parser, unsigned int line_number) {
    buffer_t *buffer = buffer_new(NULL, 0);
    unsigned int first_char_match = 0;
    unsigned int last_char_match = 0;
    for (unsigned int i = 0; i < parser->lexer->count; i++) {
        if (parser->lexer->tokens[i]->line == line_number) {
            if (first_char_match == 0) {
                first_char_match = parser->lexer->tokens[i]->index;
            }
            last_char_match = parser->lexer->tokens[i]->index +
                              parser->lexer->tokens[i]->length;
        } else if (parser->lexer->tokens[i]->line > line_number) {
            break;
        }
    }
    for (unsigned int i = first_char_match; i < last_char_match; i++) {
        buffer_push(buffer, parser->lexer->source[i]);
    }
    return buffer_to_str(buffer);
}

char *parser_get_source_lines(char *source, unsigned int start_line,
                              unsigned int end_line) {
    buffer_t *buffer = buffer_new(NULL, 0);
    size_t source_length = strlen(source);
    unsigned int current_line = 0;
    for (unsigned int i = 0; i < source_length; i++) {
        if (current_line >= start_line) {
            buffer_push(buffer, source[i]);
        }
        if (source[i] == '\n') {
            current_line++;
        }
        if (current_line > end_line && source[i] == '\n') {
            break;
        }
    }

    return buffer_to_str(buffer);
}

char *parser_get_source_context(parser_t *parser, token_t *token) {
    // int preferred_line_count = 3;
    int line_number_start =
        token->line; //(((int)token->line - preferred_line_count) > 0) ?
                     // token->line - preferred_line_count: 0;

    char *source = parser_get_source_lines(parser->lexer->source,
                                           line_number_start, token->line);
    return source;
}

parser_t *parser_new(lexer_t *lexer) {
    parser_t *result = (parser_t *)malloc(sizeof(parser_t));
    result->lexer = lexer;
    result->ast = NULL;
    result->token_index = 0;
    result->class_list = array_new();
    result->current_token = lexer->count ? lexer->tokens[0] : NULL;
    result->previous_token = result->current_token;
    while (result->current_token && result->current_token->type < 10) {
        parser_next(result);
    }
    return result;
}
void parser_delete(parser_t *parser) {
    array_delete(parser->class_list);
    free(parser);
}

token_t *parser_peek(parser_t *parser) {
    token_t *token = parser->current_token;
    unsigned int token_index = parser->token_index;
    if (token_index + 1 < parser->lexer->count) {
        token = parser->lexer->tokens[token_index + 1];
        token_index++;
        while (token->type < 10 && token_index + 1 < parser->lexer->count) {
            token = parser->lexer->tokens[token_index + 1];
            token_index++;
        }
        return token;
    }
    return NULL;
}
token_t *parser_next(parser_t *parser) {
    parser->previous_token = parser->current_token;
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
    va_start(args, message);
    fprintf(stderr, "\033[31m");
    if (parser->current_token) {
        fprintf(stderr, "\033[31mError at line %d, column %d: ",
                parser->current_token->line + 1,
                parser->current_token->col + 1);

        vfprintf(stderr, message, args);
        fprintf(stderr, "\033[0m");
        char *parser_source_context =
            parser_get_source_context(parser, parser->current_token);
        fprintf(stderr, "%s", parser_source_context);
        if(parser_source_context[-1] != '\n'){
            fprintf(stderr, "\n");
        }
        for (unsigned int i = 0; i < parser->current_token->col; i++) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "\033[31m");
        for (unsigned int i = parser->current_token->col;
             i < parser->current_token->col + parser->current_token->length;
             i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\033[0m\n");
        free(parser_source_context);
    }
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
    bool token_found = false;
    for (unsigned int i = 0; i < max; i++) {
        token_type = va_arg(args, int);
        if (token_type == -1) {
            break;
        } else if ((int)token->type == token_type) {
            token_found = true;
            break;
        } else if (token_type == TOKEN_UNKNOWN) {
            // token_found = true;
            break;
        }
    }
    va_end(args);
    if (token_type == -1 && required) {
        parser_raise(parser, "syntax error. unexpected token: %s\n",
                     token->value);
        exit(2);
    }
    if (!token_found)
        token = NULL;
    return token_type == -1 ? NULL : token;
}

token_t *parser_advance(parser_t *parser, bool required, ...) {
    va_list args;
    va_start(args, required);
    token_t *token = NULL;
    token = parser_expect(parser, required, args);
    if (token)
        token = parser_next(parser);
    va_end(args);
    return token;
}

ast_t *parse_remainder(parser_t *parser) {
    parser_next(parser);
    return NULL;
}

token_t * _parser_current_token = NULL;
token_t * _parser_previous_token = NULL;
unsigned int _parser_token_index = 0;
void parser_push_state(parser_t * parser){
    _parser_current_token = parser->current_token;
 _parser_previous_token = parser->previous_token;
 _parser_token_index = parser->token_index;
}
void parser_pop_state(parser_t * parser){
    parser->current_token = _parser_current_token;
    parser->previous_token = _parser_previous_token;
    parser->token_index = _parser_token_index;
}

ast_t *parse_operator(parser_t *parser) {
    parser_push_state(parser);
    parser_next(parser);
    if (!parser_expect(parser, false, TOKEN_EQUAL, TOKEN_GT, TOKEN_GTE,
                        TOKEN_LT, TOKEN_LTE, TOKEN_NOT_EQUAL, -1)) {
        parser_pop_state(parser);
        return parse_remainder(parser);
    }
    ast_t *ast = NULL;
    token_t *token_identifier = parser->previous_token;
    token_t *token_operator = parser->current_token;
    parser_next(parser);
    ast_value_t *value = parse_value(parser);
    if (token_operator->type == TOKEN_EQUAL) {
        ast = (ast_t *)ast_equal_new(token_identifier->value, value);
    } else if (token_operator->type == TOKEN_GT) {
        ast = (ast_t *)ast_gt_new(token_identifier->value, value);
    } else if (token_operator->type == TOKEN_GTE) {
        ast = (ast_t *)ast_gte_new(token_identifier->value, value);
    } else if (token_operator->type == TOKEN_LT) {
        ast = (ast_t *)ast_lt_new(token_identifier->value, value);
    } else if (token_operator->type == TOKEN_LTE) {
        ast = (ast_t *)ast_lte_new(token_identifier->value, value);
    } else if (token_operator->type == TOKEN_NOT_EQUAL) {
        ast = (ast_t *)ast_not_equal_new(token_identifier->value, value);
    }
    return (ast_t *)ast;
}

ast_t *parse_assignment(parser_t *parser) {
    token_t *token = parser->current_token;

    token_t *token_next = parser_peek(parser);
    if (token_next->type == TOKEN_IS) {
        token = parser_next(parser);
    }

    if (token->type == TOKEN_IS) {

        token_t *token_identifier = parser->previous_token;
        parser_next(parser);
        ast_value_t *value = parse_value(parser);
        ast_assignment_t *assignment =
            ast_assignment_new(token_identifier->value, value);
        return (ast_t *)assignment;
    }
    return parse_operator(parser);
}

ast_value_t *parse_value(parser_t *parser) {
    token_t *token =
        parser_expect(parser, true, TOKEN_STRING, TOKEN_NUMBER, TOKEN_BOOLEAN,
                      TOKEN_SYMBOL, TOKEN_PAREN_OPEN, -1);
    if (token->type == TOKEN_PAREN_OPEN) {
        return (ast_value_t *)parse_closure(parser, TOKEN_PAREN_OPEN);
    }
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
    parser_next(parser);
    return ast_value_new(token->value, token_type);
}

ast_t *parse_for(parser_t *parser) {
    if (!parser_advance(parser, false, TOKEN_FOR, -1)) {
        return (ast_t *)parse_expression(parser); // parse_value(parser);
    }
    parser_advance(parser, true, TOKEN_PAREN_OPEN, -1);
    ast_t *start = parse_expression(parser);
    ast_t *end = parse_expression(parser);
    ast_t *expr = parse_expression(parser);
    parser_advance(parser, true, TOKEN_PAREN_CLOSE, -1);
    ast_t *closure = parse_closure(parser, TOKEN_BRACE_OPEN);
    parser_advance(parser, true, TOKEN_BRACE_CLOSE);
    ast_for_t *ast_for = ast_for_new(start, end, expr, closure);
    return (ast_t *)ast_for;
}

ast_t *parse_while(parser_t *parser) {
    token_t *token = NULL;
    if (!(token = parser_advance(parser, false, TOKEN_WHILE, -1))) {
        return parse_for(parser);
    }
    printf("%d:%s\n", token->type, token->value);
    ast_t *statement = parse_closure(parser, TOKEN_PAREN_OPEN);
    ast_t *closure = parse_closure(parser, TOKEN_BRACE_OPEN);
    ast_while_t *ast_while = ast_while_new(statement, closure);
    return (ast_t *)ast_while;
}

void parser_dump(parser_t *parser){
    unsigned int prev_line = 1337;
    unsigned int i = 0;
    while(i < parser->lexer->count){
        token_t * token = parser->lexer->tokens[i];
        if(token->line != prev_line){
            prev_line = token->line;
            printf("%d:\t", token->line + 1);
        }
        printf(parser->lexer->tokens[i]->value);
        i++;
        if(parser->lexer->tokens[i] == parser->current_token){
            break;
        }
    }
    printf("\n");
}

ast_t * parse_function_call(parser_t *parser) {
    token_t *token = NULL;
    
    if (!(token = parser_expect(parser, false, TOKEN_SYMBOL, -1))) {
        return parse_assignment(parser);
    }
    char * name = token->value; 
    token = parser_peek(parser);

    if(token->type == TOKEN_PAREN_OPEN) {
        parser_next(parser);
        printf("XXX\n");
        dict_t * params = parse_params(parser);
        
        parser_dump(parser);
        printf("XXX\n");
    
        ast_function_call_t * call = ast_function_call_new(name, params);
        return (ast_t *)call;
    }
    return parse_assignment(parser);
}

dict_t * parse_params(parser_t *parser) {
    parser_expect(parser,true,TOKEN_PAREN_OPEN, -1);
    dict_t *params = dict_new();
    token_t *token;

    while ((token = parser_next(parser))) {
    
        if(token->type == TOKEN_PAREN_CLOSE){
            break;
        }
    parser_dump(parser);
        
        parser_expect(parser, true, TOKEN_SYMBOL, -1);
        ast_t * declaration = (ast_t *)parse_statement(parser);
        char * name = token->value;
        if(!declaration){
            printf("NOTTAA\n");
        }
        dict_set(params,name,(void *)declaration);
        token = parser_expect(parser, true, TOKEN_PAREN_CLOSE, TOKEN_COMMA, -1);
        if(token->type == TOKEN_PAREN_CLOSE){
            break;
        }
        //if(parser->current_token->type == TOKEN_COMMA){
        //    parser_next(parser);   
        //}

    }
    
    parser_expect(parser, true, TOKEN_PAREN_CLOSE, -1);
    parser_next(parser);
    return params;
}

ast_t *parse_variable_definition(parser_t *parser) {
    if (!parser_expect(parser, false, TOKEN_SYMBOL, -1)) {
        return parse_function_call(parser);
    }
    char *variable_identifiers = "int|char|bool|float|double|void";
    token_t *token = parser->current_token;
    char *matched_variable_identifier =
        string_match_option(token->value, variable_identifiers);
    if (!matched_variable_identifier && parser->class_list->count) {
        char *user_defined_classes_string =
            array_to_option_string(parser->class_list);
        matched_variable_identifier =
            string_match_option(token->value, user_defined_classes_string);
        free(user_defined_classes_string);
    }

    if (!matched_variable_identifier) {
        return parse_function_call(parser);
    }
    token = parser_peek(parser);
    if(token->type != TOKEN_SYMBOL) {
        return parse_function_call(parser);
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
    
    token = parser_next(parser);
    if(parser_expect(parser, false, TOKEN_PAREN_OPEN, -1)){
        dict_t * params = parse_params(parser);
        if(parser_expect(parser, true, TOKEN_BRACE_OPEN, -1)){
            ast_t * body = parse_closure(parser, TOKEN_BRACE_OPEN);

            ast_function_declaration_t * declaration = ast_function_declaration_new(type, identifier,params,body);

         return (ast_t *)declaration;
        }
    } else if (parser_expect(parser, false, TOKEN_IS, -1)) {
        ast_t *assignment = parse_assignment(parser);
        ast_var_declaration_t * definition = ast_var_declaration_new(type, identifier);
        definition->stars = stars;
        ast_add_child((ast_t *)definition, (ast_t *)assignment);
        return (ast_t *)definition;
    }
    return NULL;
}

ast_t *parse_expression(parser_t *parser) {
    ast_t *return_value = parse_variable_definition(parser);
    return return_value;
}

ast_t *parse_closure(parser_t *parser, token_type_t open_type) {

    token_t *token = parser->current_token;
    parser_expect(parser, true, open_type, -1);
    int close_type = 0;
    if (open_type == TOKEN_PAREN_OPEN) {
        close_type = TOKEN_PAREN_CLOSE;
    } else if (open_type == TOKEN_BRACE_OPEN) {
        close_type = TOKEN_BRACE_CLOSE;
    } else if (open_type == TOKEN_UNKNOWN) {
        close_type = TOKEN_UNKNOWN;
    }

    ast_t *closure = ast_closure_new();
    if (open_type != TOKEN_UNKNOWN)
        token = parser_next(parser);
    while (token && (int)token->type != close_type && (int)token->type > 10) {
        token_t *token_start = parser->current_token;
        ast_t *statement = parse_statement(parser);
        if (statement) {
            ast_add_child((ast_t *)closure, (ast_t *)statement);
        }
        token = parser->current_token;
        if (token == token_start)
            break;
    }
    parser_expect(parser, true, close_type, -1);
    parser_next(parser);
    return closure;
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
    token_t *token;
    if (!(token = parser_expect(parser, false, TOKEN_CLASS, -1))) {
        return parse_while(parser);
    }
    token = parser_next(parser);
    if (!array_push_string(parser->class_list, token->value)) {
        parser_raise(parser, "Class already defined: %s\n", token->value);
        exit(3);
    }
    ast_class_definition_t *definition = ast_class_definition_new(token->value);
    parser_next(parser);
    token = parser_expect(parser, true, TOKEN_PAREN_OPEN, TOKEN_BRACE_OPEN, -1);

    if (!token) {
        parser_raise(parser, "Syntax error. Expected '(' or '{'\n");
        exit(2);
    }
    if (token->type == TOKEN_PAREN_OPEN) {
        definition->extends = parse_class_extends(parser, definition);
        assert(definition->extends);
        parser_expect(parser, true, TOKEN_BRACE_OPEN, -1);
        definition->body = parse_closure(parser, TOKEN_BRACE_OPEN);
    } else {
        definition->body = parse_closure(parser, TOKEN_BRACE_OPEN);
    }
    assert(definition->body);

    return (ast_t *)definition;
}

ast_t *parse_macro(parser_t *parser) {
    if (!parser_expect(parser, false, TOKEN_MACRO, -1)) {
        return parse_class_definition(parser);
    }
    char *macro_text = parser->current_token->value;
    printf("COMES HERER!!\n");
    printf("###%s\n",macro_text);
    if (!strcmp(macro_text, "debug")) {
        token_t *token_next = parser_peek(parser);
        printf("t.line: %d t.col %d t.type: %d t.value:\"%s\"\n",
               token_next->line, token_next->col, token_next->value);
    }
    return NULL;
}

ast_t *parse_statement(parser_t *parser) { return parse_macro(parser); }

ast_t *parse(lexer_t *lexer) {
    parser_t *parser = parser_new(lexer);
    token_t *token_before = NULL;
    token_t *token_after = NULL;
    parser->previous_token = parser->current_token;
    while (parser->current_token) {
        token_before = parser->current_token;
        ast_t *result = parse_closure(parser, TOKEN_UNKNOWN);
        token_after = parser->current_token;
        if (token_before && token_after == token_before) {
            parser_raise(parser, "Syntax error. Unexpected '%s'\n",
                         token_before->value);
            printf("debug: in main parser function.\n");
            exit(1);
            // return NULL;

            // exit(0);
            // token_dump(token_before);
            // parser_next(parser);
        }
        if (result) {
            ast_delete(result);
        } else if (parser->current_token) {
            // parser_expect(parser, 0, -1);
            // exit(3);
            // token_dump(parser->current_token);
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

void parse_string(char *script) {
    lexer_t *lexer = lexer_new();
    lexer = lexer_parse(lexer, script);
    parse(lexer);
    lexer_delete(lexer);
}

void parse_file(char *filepath) {
    char *content = read_file_contents(filepath);
    lexer_t *lexer = lexer_new();
    lexer = lexer_parse(lexer, content);
    parse(lexer);
    // lexer_dump(lexer);
    print("%d lines and %d tokens\n", lexer->lines, lexer->count);
    lexer_delete(lexer);
    free(content);
}
#endif
