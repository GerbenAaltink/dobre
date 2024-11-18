#ifndef AST_H
#define AST_H
#include "array.h"
#include "dict.h"
#include "dobre.h"
#include "string.h"
#include <stdbool.h>


typedef enum ast_type_t {
    AST_NODE,
    AST_VALUE,
    AST_CLASS_DECLARATION,
    AST_VAR_DECLARATION,
    AST_ASSIGNMENT,
    AST_ARRAY,
    AST_WHILE,
    AST_FOR,
    AST_EQUAL,
    AST_LT,
    AST_LTE,
    AST_GT,
    AST_GTE,
    AST_NOT,
    AST_NOT_EQUAL,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL
} ast_type_t;

typedef struct ast_t {
    ast_type_t type;
    struct ast_t **children;
    unsigned int children_count;
} ast_t;

typedef struct ast_value_t {
    ast_t node;
    char *value;
    char *type;
} ast_value_t;

typedef struct ast_equal_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_equal_t;

typedef struct ast_lt_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_lt_t;

typedef struct ast_lte_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_lte_t;

typedef struct ast_gt_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_gt_t;

typedef struct ast_gte_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_gte_t;

typedef struct ast_not_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_not_t;

typedef struct ast_not_equal_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_not_equal_t;

typedef struct ast_for_t {
    ast_t node;
    ast_t *start;
    ast_t *end;
    ast_t *statement;
    ast_t *closure;
} ast_for_t;

typedef struct ast_class_definition_t {
    ast_t node;
    char *name;
    ast_t *body;
    array_t *extends;
} ast_class_definition_t;

typedef struct ast_function_declaration_t {
    ast_t node;
    char *name;
    char * type;
    ast_t *body;
    dict_t *params;
    unsigned int stars;
} ast_function_declaration_t;

typedef struct ast_function_call_t {
    ast_t node;
    char *name;
    dict_t *params;
    unsigned int stars;
} ast_function_call_t;

typedef struct ast_var_declaration_t {
    ast_t node;
    char *type;
    char *name;
    unsigned int stars;
} ast_var_declaration_t;

typedef struct ast_assignment_t {
    ast_t node;
    char *name;
    ast_value_t *value;
} ast_assignment_t;

typedef struct ast_while_t {
    ast_t node;
    ast_t *statement;
    ast_t *closure;
} ast_while_t;

ast_t *ast_new(char *type);
void ast_delete(ast_t *node);
ast_value_t *ast_value_new(char *value, char *type);
ast_class_definition_t *ast_class_definition_new(char *name);
void ast_class_definition_delete(ast_class_definition_t *definition);
ast_var_declaration_t *ast_var_declaration_new(char *type, char *name);
void ast_var_declaration_delete(ast_var_declaration_t *declaration);
ast_assignment_t *ast_assignment_new(char *name, ast_value_t *value);
void ast_assignment_delete(ast_assignment_t *assignment);
void ast_dump(ast_t *node);
void ast_init(ast_t *ast) {
    ast->children = NULL;
    ast->children_count = 0;
    ast->type = AST_NODE;
}

void ast_value_delete(ast_value_t *value);
void ast_class_definition_delete(ast_class_definition_t *definition);
void ast_var_declaration_delete(ast_var_declaration_t *declaration);
void ast_assignment_delete(ast_assignment_t *assignment);
void ast_while_delete(ast_while_t *while_fn);
void ast_for_delete(ast_for_t *for_fn);
void ast_equal_delete(ast_equal_t *ast);
void ast_gt_delete(ast_gt_t *ast);
void ast_gte_delete(ast_gte_t *ast);
void ast_lt_delete(ast_lt_t *ast);
void ast_lte_delete(ast_lte_t *ast);
void ast_not_delete(ast_not_t *ast);
void ast_not_equal_delete(ast_not_equal_t *ast);
void ast_function_declaration_delete(ast_function_declaration_t * ast);
void ast_function_call_delete(ast_function_call_t * ast);
void ast_delete(ast_t *node) {
    ast_dump(node);
    ast_t *child = node;

    if (child->type == AST_VALUE)
        ast_value_delete((ast_value_t *)child);
    else if (child->type == AST_CLASS_DECLARATION)
        ast_class_definition_delete((ast_class_definition_t *)child);
    else if (child->type == AST_VAR_DECLARATION)
        ast_var_declaration_delete((ast_var_declaration_t *)child);
    else if (child->type == AST_ASSIGNMENT)
        ast_assignment_delete((ast_assignment_t *)child);
    else if (child->type == AST_WHILE) {
        ast_while_delete((ast_while_t *)child);
    } else if (child->type == AST_FOR) {
        ast_for_delete((ast_for_t *)child);
    } else if (child->type == AST_EQUAL) {
        ast_equal_delete((ast_equal_t *)child);
    } else if (child->type == AST_LT) {
        ast_lt_delete((ast_lt_t *)child);
    } else if (child->type == AST_LTE) {
        ast_lte_delete((ast_lte_t *)child);
    } else if (child->type == AST_GT) {
        ast_gt_delete((ast_gt_t *)child);
    } else if (child->type == AST_GTE) {
        ast_gte_delete((ast_gte_t *)child);
    } else if (child->type == AST_NOT_EQUAL) {
        ast_not_equal_delete((ast_not_equal_t *)child);
    } else if (child->type == AST_NOT) {
        ast_not_delete((ast_not_t *)child);
    } else if(child->type == AST_FUNCTION_DECLARATION){
        ast_function_declaration_delete((ast_function_declaration_t *)child);
    } else if(child->type == AST_FUNCTION_CALL){
        ast_function_call_delete((ast_function_call_t *)child);
    }
    for (unsigned int i = 0; i < child->children_count; i++)
        ast_delete(child->children[i]);
    free(child);
}

void ast_add_child(ast_t *parent, ast_t *child) {
    parent->children = (ast_t **)realloc(
        parent->children, sizeof(ast_t *) * (parent->children_count + 1));
    parent->children[parent->children_count] = child;
    parent->children_count++;
}

void ast_value_delete(ast_value_t *value) {
    printf("Free value %s of type %s.\n", value->value, value->type);
    if (value->value)
        free(value->value);
    if (value->type)
        free(value->type);
}
void ast_value_dump(ast_value_t *value);
void ast_class_definition_dump(ast_class_definition_t *definition);
void ast_var_declaration_dump(ast_var_declaration_t *declaration);
void ast_assignment_dump(ast_assignment_t *assignment);
void ast_while_dump(ast_while_t *while_fn);
void ast_for_dump(ast_for_t *for_fn);
void ast_equal_dump(ast_equal_t *ast);
void ast_lt_dump(ast_lt_t *ast);
void ast_lte_dump(ast_lte_t *ast);
void ast_gt_dump(ast_gt_t *ast);
void ast_gte_dump(ast_gte_t *ast);
void ast_function_declaration_dump(ast_function_declaration_t * ast);
void ast_function_call_dump(ast_function_call_t * ast);
void ast_dump(ast_t *node) {
    if (!node)
        return;
    if (node->type == AST_VALUE) {
        ast_value_dump((ast_value_t *)node);
    } else if (node->type == AST_CLASS_DECLARATION) {
        ast_class_definition_dump((ast_class_definition_t *)node);
    } else if (node->type == AST_VAR_DECLARATION) {
        ast_var_declaration_dump((ast_var_declaration_t *)node);
    } else if (node->type == AST_ASSIGNMENT) {
        ast_assignment_dump((ast_assignment_t *)node);
    } else if (node->type == AST_WHILE) {
        ast_while_dump((ast_while_t *)node);
    } else if (node->type == AST_FOR) {
        ast_for_dump((ast_for_t *)node);
    } else if (node->type == AST_EQUAL) {
        ast_equal_dump((ast_equal_t *)node);
    } else if (node->type == AST_LT) {
        ast_lt_dump((ast_lt_t *)node);
    } else if (node->type == AST_LTE) {
        ast_lte_dump((ast_lte_t *)node);
    } else if (node->type == AST_GT) {
        ast_gt_dump((ast_gt_t *)node);
    } else if (node->type == AST_GTE) {
        ast_gte_dump((ast_gte_t *)node);
    }else if(node->type == AST_FUNCTION_DECLARATION){
        ast_function_declaration_dump((ast_function_declaration_t *)node);
    }
}

ast_value_t *ast_value_new(char *value, char *type) {
    ast_value_t *result = (ast_value_t *)malloc(sizeof(ast_value_t));
    ast_init(&result->node);
    result->node.type = AST_VALUE;
    result->type = strdup(type);
    result->value = value ? strdup(value) : NULL;
    return result;
}
void ast_value_dump(ast_value_t *value) {
    printf("Type: value. Value: %s. Type: %s\n", value->value, value->type);
}
ast_class_definition_t *ast_class_definition_new(char *name) {
    ast_class_definition_t *result =
        (ast_class_definition_t *)malloc(sizeof(ast_class_definition_t));
    ast_init(&result->node);
    result->node.type = AST_CLASS_DECLARATION;
    result->name = strdup(name);
    result->body = NULL;
    result->extends = NULL;
    return result;
}
void ast_class_definition_delete(ast_class_definition_t *definition) {
    printf("Free class %s\n", definition->name);
    if (definition->extends) {
        array_delete(definition->extends);
    }
    if (definition->name)
        free(definition->name);
    if (definition->body) {
        ast_delete(definition->body);
    }
}
void ast_class_definition_dump(ast_class_definition_t *definition) {
    printf("Defined class %s", definition->name);
    printf("(");
    if (definition->extends && definition->extends->count > 0) {
        for (unsigned int i = 0; i < definition->extends->count; i++) {
            if (definition->extends->data[i]->value) {
                printf("%s", (char *)definition->extends->data[i]->value);
            }
            if (i != definition->extends->count - 1) {
                printf(",");
            }
        }
    }
    printf(")\n");
}

ast_var_declaration_t *ast_var_declaration_new(char *type, char *name) {
    ast_var_declaration_t *result =
        (ast_var_declaration_t *)malloc(sizeof(ast_var_declaration_t));
    ast_init(&result->node);

    result->node.type = AST_VAR_DECLARATION;
    result->type = strdup(type);
    result->name = strdup(name);
    result->stars = 0;
    return result;
}
void ast_var_declaration_dump(ast_var_declaration_t *declaration) {
    printf("Declared variable %s with type %s", declaration->name,
           declaration->type);
    for (unsigned int i = 0; i < declaration->stars; i++) {
        printf("*");
    }
    printf("\n");
}
void ast_var_declaration_delete(ast_var_declaration_t *declaration) {
    printf("Free %s %s.\n", declaration->type, declaration->name);
    if (declaration->type)
        free(declaration->type);
    if (declaration->name)
        free(declaration->name);
}
void ast_assignment_dump(ast_assignment_t *assignment) {
    printf("Assigned %s with \"%s\".\n", assignment->name,
           assignment->value->value);
}
ast_assignment_t *ast_assignment_new(char *name, ast_value_t *value) {
    ast_assignment_t *result =
        (ast_assignment_t *)malloc(sizeof(ast_assignment_t));
    ast_init(&result->node);

    result->node.type = AST_ASSIGNMENT;
    result->name = strdup(name);
    result->value = value;
    return result;
}
void ast_assignment_delete(ast_assignment_t *assignment) {
    printf("Free assignment %s.\n", assignment->name);

    free(assignment->name);
    ast_value_delete(assignment->value);
}

ast_t *ast_closure_new() {
    ast_t *closure = (ast_t *)malloc(sizeof(ast_t));
    ast_init(closure);
    return closure;
}
/*
void ast_closure_delete(ast_t *closure) { return; }
*/
ast_t *ast_array_new() {
    ast_t *array = (ast_t *)malloc(sizeof(ast_t));
    ast_init(array);
    return array;
}

void ast_while_dump(ast_while_t *while_fn) {
    printf("While (\n");
    ast_dump(while_fn->statement);
    printf(")\n{\n");
    ast_dump(while_fn->closure);
    printf("}\n");
}
ast_while_t *ast_while_new(ast_t *statement, ast_t *closure) {
    ast_while_t *result = (ast_while_t *)malloc(sizeof(ast_while_t));
    ast_init(&result->node);

    result->node.type = AST_WHILE;
    result->statement = statement;
    result->closure = closure;
    return result;
}
void ast_while_delete(ast_while_t *while_fn) {
    printf("Free while.\n");
    ast_delete(while_fn->statement);
    ast_delete(while_fn->closure);
}

void ast_for_dump(ast_for_t *for_fn) {
    printf("for (\n");
    ast_dump(for_fn->start);
    ast_dump(for_fn->end);
    ast_dump(for_fn->statement);
    printf(")\n{\n");
    ast_dump(for_fn->closure);
    printf("}\n");
}
ast_for_t *ast_for_new(ast_t *start, ast_t *end, ast_t *statement,
                       ast_t *closure) {
    ast_for_t *result = (ast_for_t *)malloc(sizeof(ast_for_t));
    ast_init(&result->node);

    result->node.type = AST_FOR;
    result->start = start;
    result->end = end;
    result->statement = statement;
    result->closure = closure;
    return result;
}
void ast_for_delete(ast_for_t *for_fn) {
    printf("Free for.\n");
    ast_delete(for_fn->statement);
    ast_delete(for_fn->start);
    ast_delete(for_fn->end);
    ast_delete(for_fn->closure);
}

ast_equal_t *ast_equal_new(char *name, ast_value_t *value) {
    ast_equal_t *ast = (ast_equal_t *)malloc(sizeof(ast_equal_t));
    ast_init(&ast->node);
    ast->node.type = AST_EQUAL;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_equal_dump(ast_equal_t *ast) {
    printf("Equal comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_equal_delete(ast_equal_t *ast) {
    printf("Free equal comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_lt_t *ast_lt_new(char *name, ast_value_t *value) {
    ast_lt_t *ast = (ast_lt_t *)malloc(sizeof(ast_lt_t));
    ast_init(&ast->node);
    ast->node.type = AST_LT;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_lt_dump(ast_lt_t *ast) {
    printf("Lt comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_lt_delete(ast_lt_t *ast) {
    printf("Free lt comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_lte_t *ast_lte_new(char *name, ast_value_t *value) {
    ast_lte_t *ast = (ast_lte_t *)malloc(sizeof(ast_lte_t));
    ast_init(&ast->node);
    ast->node.type = AST_LTE;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_lte_dump(ast_lte_t *ast) {
    printf("Lte comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_lte_delete(ast_lte_t *ast) {
    printf("Free lte comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_gt_t *ast_gt_new(char *name, ast_value_t *value) {
    ast_gt_t *ast = (ast_gt_t *)malloc(sizeof(ast_gt_t));
    ast_init(&ast->node);
    ast->node.type = AST_GT;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_gt_dump(ast_gt_t *ast) {
    printf("Gt comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_gt_delete(ast_gt_t *ast) {
    printf("Free gt comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}
ast_gte_t *ast_gte_new(char *name, ast_value_t *value) {
    ast_gte_t *ast = (ast_gte_t *)malloc(sizeof(ast_gte_t));
    ast_init(&ast->node);
    ast->node.type = AST_GTE;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_gte_dump(ast_gte_t *ast) {
    printf("Gte comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_gte_delete(ast_gte_t *ast) {
    printf("Free gte comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_not_equal_t *ast_not_equal_new(char *name, ast_value_t *value) {
    ast_not_equal_t *ast = (ast_not_equal_t *)malloc(sizeof(ast_not_equal_t));
    ast_init(&ast->node);
    ast->node.type = AST_NOT_EQUAL;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_not_equal_dump(ast_not_equal_t *ast) {
    printf("not_equal comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_not_equal_delete(ast_not_equal_t *ast) {
    printf("Free not_equal comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_not_t *ast_not_new(char *name, ast_value_t *value) {
    ast_not_t *ast = (ast_not_t *)malloc(sizeof(ast_not_t));
    ast_init(&ast->node);
    ast->node.type = AST_NOT;
    ast->name = strdup(name);
    ast->value = value;
    return ast;
}
void ast_not_dump(ast_not_t *ast) {
    printf("not comparison %s with \"%s\".\n", ast->name,
           ast->value->value);
}
void ast_not_delete(ast_not_t *ast) {
    printf("Free not comparison %s.\n", ast->name);
    free(ast->name);
    ast_value_delete(ast->value);
}

ast_function_declaration_t *ast_function_declaration_new(char *type, char *name, dict_t * params, ast_t * body) {
    ast_function_declaration_t *result =
        (ast_function_declaration_t *)calloc(1, sizeof(ast_function_declaration_t));
    ast_init(&result->node);
    result->node.type = AST_FUNCTION_DECLARATION;
    result->name = strdup(name);
    result->type = strdup(type);
    result->params = params;
    result->body = body;
    return result;
}
void ast_function_declaration_delete(ast_function_declaration_t *declaration) {
    printf("Free class %s\n", declaration->name);
    if (declaration->name)
        free(declaration->name);
    if(declaration->type)
        free(declaration->type);
    if (declaration->body) {
        ast_delete(declaration->body);
    }
    if(declaration->params){
        dict_delete(declaration->params);
    }
}
void ast_function_declaration_dump(ast_function_declaration_t *definition) {
    printf("Function declaration of %s<%s>", definition->name,definition->type ? definition->type : "NULL");
    printf("params: ");
    char * dict_string = dict_to_string(definition->params);
    printf("%s\n", dict_string);
    free(dict_string);
}

ast_function_call_t *ast_function_call_new(char *name, dict_t * params) {
    ast_function_call_t *result =
        (ast_function_call_t *)calloc(1, sizeof(ast_function_call_t));
    ast_init(&result->node);
    result->node.type = AST_FUNCTION_CALL;
    result->name = strdup(name);
    result->params = params;
    return result;
}
void ast_function_call_delete(ast_function_call_t *call) {
    printf("Free class %s\n", call->name);
    if (call->name)
        free(call->name);
    if(call->params){
        dict_delete(call->params);
    }
}
void ast_function_call_dump(ast_function_call_t *definition) {
    printf("Function call of %s\n", definition->name);
    printf("params: ");
    char * dict_string = dict_to_string(definition->params);
    printf("%s\n", dict_string);
    free(dict_string);
}

#endif
