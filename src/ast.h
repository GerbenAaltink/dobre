#ifndef AST_H
#define AST_H
#include "array.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum ast_type_t {
    AST_NODE,
    AST_VALUE,
    AST_CLASS_DECLARATION,
    AST_VAR_DECLARATION,
    AST_ASSIGNMENT,
    AST_ARRAY,
    AST_WHILE
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

typedef struct ast_class_definition_t {
    ast_t node;
    char *name;
    ast_t *body;
    array_t *extends;
} ast_class_definition_t;

typedef struct ast_var_declaration_t {
    ast_t node;
    char *type;
    char *identifier;
    unsigned int stars;
} ast_var_declaration_t;

typedef struct ast_assignment_t {
    ast_t node;
    char *identifier;
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
ast_var_declaration_t *ast_var_declaration_new(char *type, char *identifier);
void ast_var_declaration_delete(ast_var_declaration_t *declaration);
ast_assignment_t *ast_assignment_new(char *identifier, ast_value_t *value);
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
    } else
        printf("NO DELETION\n");
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

ast_var_declaration_t *ast_var_declaration_new(char *type, char *identifier) {
    ast_var_declaration_t *result =
        (ast_var_declaration_t *)malloc(sizeof(ast_var_declaration_t));
    ast_init(&result->node);

    result->node.type = AST_VAR_DECLARATION;
    result->type = strdup(type);
    result->identifier = strdup(identifier);
    result->stars = 0;
    return result;
}
void ast_var_declaration_dump(ast_var_declaration_t *declaration) {
    printf("Declared variable %s with type %s", declaration->identifier,
           declaration->type);
    for (unsigned int i = 0; i < declaration->stars; i++) {
        printf("*");
    }
    printf("\n");
}
void ast_var_declaration_delete(ast_var_declaration_t *declaration) {
    printf("Free %s %s.\n", declaration->type, declaration->identifier);
    if (declaration->type)
        free(declaration->type);
    if (declaration->identifier)
        free(declaration->identifier);
}
void ast_assignment_dump(ast_assignment_t *assignment) {
    printf("Assigned %s with \"%s\".\n", assignment->identifier,
           assignment->value->value);
}
ast_assignment_t *ast_assignment_new(char *identifier, ast_value_t *value) {
    ast_assignment_t *result =
        (ast_assignment_t *)malloc(sizeof(ast_assignment_t));
    ast_init(&result->node);

    result->node.type = AST_ASSIGNMENT;
    result->identifier = strdup(identifier);
    result->value = value;
    return result;
}
void ast_assignment_delete(ast_assignment_t *assignment) {
    printf("Free assignment %s.\n", assignment->identifier);

    free(assignment->identifier);
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

#endif
