#ifndef CLASS_H
#define CLASS_H
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum class_type_t {
    CLASS_VALUE,
    CLASS_METHOD,
    CLASS_ARRAY,
    CLASS_OBJECT,
    CLASS_NULL
} class_type_t;

typedef struct class_t {
    char *name;
    class_type_t type;
    size_t attributes_count;
    struct class_t *closure;
    struct class_t **extends;
    struct class_t **attributes;
    size_t extends_count;
    size_t length;
    size_t count;
    struct {
        unsigned char *string;
        long long integer;
        double decimal;
        bool boolean;

    } value;

} class_t;

class_t *class_extend(class_t *self, class_t *parent) {
    self->extends_count++;
    self->extends = (class_t **)realloc(self->extends, sizeof(class_t *) *
                                                           self->extends_count);
    self->extends[self->extends_count - 1] = parent;
    return self;
}

class_t *class_new(char *name) {
    class_t *result = (class_t *)calloc(1, sizeof(class_t));
    result->name = strdup(name);
    result->length = 0;
    result->attributes = NULL;
    result->count = 0;
    return result;
}

void class_delete(class_t *self) {
    free(self->name);
    if (self->count) {
        for (unsigned int i = 0; i < self->count; i++) {
            class_delete(self->attributes[i]);
        }
    }
    free(self);
}
#endif