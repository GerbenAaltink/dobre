#ifndef PARSE_H
#define PARSE_H
#include "lexer.h"
#include "ast.h"

typedef struct parser_t {
    lexer_t * lexer;
    ast_t * ast;
    token_t * current_token;
    unsigned int token_index;
} parser_t;


parser_t * parser_new(lexer_t * lexer){
    parser_t * result = (parser_t *)malloc(sizeof(parser_t));
    result->lexer = lexer;
    result->ast = NULL;
    result->token_index = 0;
    result->current_token = lexer->count ? lexer->tokens[0] : NULL;
    return result;
}
token_t * parser_next(parser_t * parser){
    if(parser->token_index + 1 < parser->lexer->count){
        parser->current_token = parser->lexer->tokens[parser->token_index+1];
        parser->token_index++;
        if(parser->current_token->type  < 3){
            return parser_next(parser);
        }
        return parser->current_token;
    }else{
        parser->current_token = NULL;
    }
    return NULL;
}

ast_t * parse_remainder(parser_t * parser){
    parser_next(parser);
    return NULL; 
}


ast_t * parse_assignment(parser_t * parser){
    token_t * token = parser->current_token;
    if(token->type == TOKEN_IS){
        __attribute__((unused)) ast_assignment_t * assignment = ast_assignment_new(token->value,NULL);
        exit(0);
    }
    return parse_remainder(parser);
}

ast_t * parse_variable_definition(parser_t * parser){
    char * variable_identifiers = "int|char|bool|float|double|void|";
    token_t * token = parser->current_token;
    if(token->type == TOKEN_SYMBOL && string_match_option(token->value,variable_identifiers)){
        char * type = token->value;
        unsigned int stars = 0;
        while((token = parser_next(parser)) && token->type == TOKEN_STAR){
            stars++;
        }
        if(token->type != TOKEN_SYMBOL){
            printf("Syntax error?\n");
        }
        assert(token->type == TOKEN_SYMBOL);
        char * identifier = token->value;
        ast_var_declaration_t * definition = ast_var_declaration_new(type,identifier);
        definition->stars = stars;
        token = parser_next(parser);
        if(token->type == TOKEN_IS){
            token = parser_next(parser);
            char * token_type = NULL;
            if(token->type == TOKEN_NUMBER){
                token_type = "number";
            }else if(token->type == TOKEN_STRING){
                token_type = "string";
            }else if(token->type == TOKEN_BOOLEAN){
                token_type = "boolean";
            }else if(token->type == TOKEN_BOOLEAN){
                token_type = "symbol";
            }
            ast_value_t * value = ast_value_new(token->value,token_type);
            ast_assignment_t * assignment = ast_assignment_new(identifier,value);
            ast_add_child((ast_t *)definition,(ast_t *)assignment);
            printf("%s with %d * %s = %s\n",type,stars,identifier,assignment->value->value);
            parser_next(parser);
        }
        return (ast_t *)definition;
    }else{
        return  parse_remainder(parser);
    }
}

ast_t * parse_class_definition(parser_t * parser){
    token_t * token = parser->current_token;
    if(token->type == TOKEN_SYMBOL && !strcmp(token->value,"class")){
    token = parser_next(parser);
    assert(token->type == TOKEN_SYMBOL);
        ast_class_definition_t * definition = ast_class_definition_new(token->value);
        parser_next(parser);
        return (ast_t *)definition;
        //printf("YEAHH!!\n");
    }else{
        ast_t * variable_definition = parse_variable_definition(parser);
        if(variable_definition)
            return variable_definition;
            //parse_remainder(parser);
        
    }
    return NULL;
}


ast_t * parse(lexer_t * lexer){
    parser_t  * parser = parser_new(lexer);
      
    while(parser->current_token){
    ast_t * result = parse_class_definition(parser);
    if(result){
       // ast_class_definition_t * definition = (ast_class_definition_t *)result;
       // printf("Will delete %d :%p %s\n",definition->node.type, &result,definition->name);
        ast_delete(result);
        printf("DONE\n");
    }
    }
    return NULL;
}




char * read_file_contents(char * filename){

    FILE * file = fopen(filename, "r");
    if(file == NULL){
        printf("Couldn't open file: %s\n",filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char * buffer = (char *)malloc(length + 1);
    __attribute__((unused)) size_t bytes_read = fread(buffer, length, 1, file);
    buffer[length] = 0;
    fclose(file);
    return buffer;

}

void parse_file(char *filepath){
    char * content = read_file_contents(filepath);
    lexer_t * lexer = lexer_parse(content);
    
    parse(lexer);
    printf("HIERSS\n");
    //lexer_dump(lexer);
    printf("%d lines and %d tokens\n",lexer->lines,lexer->count);
    lexer_delete(lexer);
    free(content);
}
#endif
