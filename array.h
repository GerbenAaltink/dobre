#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
typedef enum array_item_type_t {
    ARRAY_ITEM_TYPE_POINTER,
    ARRAY_ITEM_TYPE_STRING
} array_item_type_t;

typedef struct array_item_t {
    void * value;
    bool free;
    array_item_type_t type;
} array_item_t;

typedef struct array_t
{
    array_item_t **data;
    unsigned int count;
    unsigned int size;
} array_t;



array_t * array_new(){
    array_t *a = (array_t *)malloc(sizeof(array_t));
    a->data = NULL;
    a->count = 0;
    a->size = 0;
    return a;
};

void array_delete(array_t *a){
    for(int i = 0; i<a->count;i++){
        if(a->data[i]->free){
            free(a->data[i]->value);
        }
        free(a->data[i]);
    }
    //if(a->data)
      //  free(a->data);
    free(a);
}

array_item_t * array_item_new(array_item_type_t type, void * value, bool pfree){
    array_item_t    *t = (array_item_t *)malloc(sizeof(array_item_t));
    t->value = value;
    t->free = pfree;
    t->type = type;
    return t;
}

void array_push_item(array_t *a, array_item_t * item){
    if(a->count >= a->size){
        a->data = (array_item_t **)realloc(a->data,a->size*sizeof(array_item_t *) + 1);
        a->data[a->size] = item;
        a->size += 1;
    }
    a->count++;
}
bool array_string_exists(array_t *a, char * data){
    if(data == NULL)
        return true;
     for(int i = 0; i<a->count;i++){
        if(a->data[i]->type == ARRAY_ITEM_TYPE_STRING && !strcmp((char *)a->data[i]->value,data)){
            return true;
        }
    }
    return false;
}
bool array_pointer_exists(array_t *a, void * value){
    if(value == NULL)
        return true;
    for(int i = 0; i<a->count;i++){
        if(a->data[i]->value == value){
            return true;
        }
    }
    return false;
}
bool array_push_pointer(array_t *a, void * value, bool pfree){
    if(array_pointer_exists(a,value))
        return false;
    array_item_t * item = array_item_new(ARRAY_ITEM_TYPE_POINTER,value,pfree);
    array_push_item(a,item);
    return true;
}

bool array_push_string(array_t *a, char * data){
    if(!data || !*data)
        return false;
    if(array_string_exists(a,data))
        return false;
    array_item_t * item = array_item_new(ARRAY_ITEM_TYPE_STRING,strdup(data),true);
    array_push_item(a,item);
    return true;
}
char * array_to_option_string(array_t *a){
    buffer_t * buffer = buffer_new(NULL,0);
    for(int i = 0; i<a->count;i++){
        if(a->data[i]->type == ARRAY_ITEM_TYPE_STRING){
            buffer_write(buffer,(char *)a->data[i]->value, strlen((char *)a->data[i]->value));
        }
        if(i != a->count - 1)
            buffer_push(buffer,'|');
    }
    return buffer_to_str(buffer);
}
#endif;
