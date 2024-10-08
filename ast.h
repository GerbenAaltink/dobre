#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum ast_type_t
{
    AST_NODE,
    AST_VALUE,
    AST_CLASS_DECLARATION,
    AST_VAR_DECLARATION,
    AST_ASSIGNMENT
} ast_type_t;

typedef struct ast_t
{
    ast_type_t type;
    struct ast_t **children;
    unsigned int children_count;
} ast_t;

typedef struct ast_value_t
{
    ast_t node;
    char *value;
    char *type;
} ast_value_t;

typedef struct ast_class_definition_t
{
    ast_t node;
    char *name;
} ast_class_definition_t;

typedef struct ast_var_declaration_t
{
    ast_t node;
    char *type;
    char *identifier;
    unsigned int stars;
} ast_var_declaration_t;

typedef struct ast_assignment_t
{
    ast_t node;
    char *identifier;
    ast_value_t *value;
} ast_assignment_t;

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
void ast_init(ast_t *ast)
{
    ast->children = NULL;
    ast->children_count = 0;
    ast->type = AST_VALUE;
}

void ast_value_delete(ast_value_t *value);
void ast_class_definition_delete(ast_class_definition_t *definition);
void ast_var_declaration_delete(ast_var_declaration_t *declaration);
void ast_assignment_delete(ast_assignment_t *assignment);   
void ast_delete(ast_t *node)
{
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
    for (unsigned int i = 0; i < child->children_count; i++)
        ast_delete(child->children[i]);
    free(child);
}

void ast_add_child(ast_t *parent, ast_t *child)
{
    parent->children = (ast_t **)realloc(parent->children, sizeof(ast_t *) * (parent->children_count + 1));
    parent->children[parent->children_count] = child;
    parent->children_count++;
}

void ast_value_delete(ast_value_t *value)
{
    if (value->value)
        free(value->value);
    if (value->type)
        free(value->type);
}
void ast_value_dump(ast_value_t *value);
void ast_class_definition_dump(ast_class_definition_t *definition);
void ast_var_declaration_dump(ast_var_declaration_t *declaration);
void ast_assignment_dump(ast_assignment_t *assignment);

void ast_dump(ast_t *node)
{
    printf("%p", &node);
    if (node->type == AST_VALUE)
    {
        ast_value_dump((ast_value_t *)node);
    }
    else if (node->type == AST_CLASS_DECLARATION)
    {
        ast_class_definition_dump((ast_class_definition_t *)node);
    }
    else if (node->type == AST_VAR_DECLARATION)
    {
        ast_var_declaration_dump((ast_var_declaration_t *)node);
    }
    else if (node->type == AST_ASSIGNMENT)
    {
        ast_assignment_dump((ast_assignment_t *)node);
    }
    for (unsigned int i = 0; i < node->children_count; i++)
    {
        ast_dump(node->children[i]);
    }
}

ast_value_t *ast_value_new(char *value, char *type)
{
    ast_value_t *result = (ast_value_t *)malloc(sizeof(ast_value_t));
    ast_init(&result->node);
    result->node.type = AST_VALUE;
    result->type = strdup(type);
    result->value = value ? strdup(value) : NULL;
    return result;
}
void ast_value_dump(ast_value_t *value)
{
    printf("Type: value. Value: %s. Type: %s\n", value->value, value->type);
}
ast_class_definition_t *ast_class_definition_new(char *name)
{
    ast_class_definition_t *result = (ast_class_definition_t *)malloc(sizeof(ast_class_definition_t));
    ast_init(&result->node);
    result->node.type = AST_CLASS_DECLARATION;
    result->name = strdup(name);
    return result;
}
void ast_class_definition_delete(ast_class_definition_t *definition)
{
    if (definition->name)
        free(definition->name);
}
void ast_class_definition_dump(ast_class_definition_t *definition)
{
    printf("Type: class definition. Name: %s\n", definition->name);
}

ast_var_declaration_t *ast_var_declaration_new(char *type, char *identifier)
{
    ast_var_declaration_t *result = (ast_var_declaration_t *)malloc(sizeof(ast_var_declaration_t));
    ast_init(&result->node);

    result->node.type = AST_VAR_DECLARATION;
    result->type = strdup(type);
    result->identifier = strdup(identifier);
    result->stars = 0;
    return result;
}
void ast_var_declaration_dump(ast_var_declaration_t *declaration)
{
    printf("Type: var declration. Identifier: %s\n", declaration->identifier);
}
void ast_var_declaration_delete(ast_var_declaration_t *declaration)
{
    if (declaration->type)
        free(declaration->type);
    if (declaration->identifier)
        free(declaration->identifier);
}
void ast_assignment_dump(ast_assignment_t *assignment)
{
    printf("Type: assignment. Identifier: %s Value: %s\n", assignment->identifier, assignment->value->value);
}
ast_assignment_t *ast_assignment_new(char *identifier, ast_value_t *value)
{
    ast_assignment_t *result = (ast_assignment_t *)malloc(sizeof(ast_assignment_t));
    ast_init(&result->node);

    result->node.type = AST_ASSIGNMENT;
    result->identifier = strdup(identifier);
    result->value = value;
    return result;
}
void ast_assignment_delete(ast_assignment_t *assignment)
{
    free(assignment->identifier);
    ast_value_delete(assignment->value);
}

#endif