#ifndef DICT_H
#define DICT_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DICT_HASH_SIZE 255

unsigned dict_hash(char *s) {
    unsigned int hash;
    for (hash = 0; *s != '\0'; s++)
        hash = *s + 31 * hash;
    return hash % DICT_HASH_SIZE;
}

typedef struct dict_entry_t {
    char *key;
    void *value;
    bool is_managed;
    bool is_string;
    struct dict_entry_t *next;
    struct dict_entry_t *prev;
} dict_entry_t;

typedef struct dict_t {
    dict_entry_t *list;
    unsigned long count;
    dict_entry_t *items[DICT_HASH_SIZE];
} dict_t;
dict_t *dict_new();
dict_entry_t *dict_new_entry();
dict_entry_t *dict_set(dict_t *dict, char *key, void *value);
dict_entry_t *dict_set_managed(dict_t *dict, char *key, void *value);
dict_entry_t *_dict_set(dict_t *dict, bool is_managed, char *key,
                              void *value);
char *dict_get_string(dict_t *dict, char *key);
dict_entry_t *dict_set_string(dict_t *dict, char *key, char *value);
void dict_remove(dict_t *dict, char *key);
dict_entry_t *dict_get(dict_t *dict, char *key);

dict_entry_t *dict_get(dict_t *dict, char *key) {
    return dict->items[dict_hash(key)];
}

char * dict_to_string(dict_t *dict) {
    char * dict_string = (char *)calloc(100,sizeof(char));
    sprintf(dict_string,"{\n count: %ld\n keys: \n",dict->count);
    for(dict_entry_t *entry = dict->list; entry; entry = entry->next) {
        char *entry_string = (char *)calloc(100,sizeof(char));
        sprintf(entry_string," key: %s value: ",entry->key);
        if(entry->is_string){
            char value_string[80];
            snprintf(value_string,sizeof(value_string),"%s",(char *)entry->value);
            strcat(entry_string, "\"");
            strcat(entry_string,value_string);
            strcat(entry_string, "\"\n");
        }else
            strcat(entry_string, "*ptr\n");
        strcat(dict_string,entry_string);
        free(entry_string);
    }
    strcat(dict_string,"}");
    return dict_string;
}

void dict_dump(dict_t *dict) {
    printf("%s\n",dict_to_string(dict));
}

void dict_remove(dict_t *dict, char *key) {
    dict_entry_t *entry = dict_get(dict, key);
    if (!entry) {
        printf("HIerooO\n");
        return;
    }
    dict->items[dict_hash(key)] = NULL;
    if (entry->next) {
        entry->next->prev = entry->prev;
    }
    if (dict->list == entry) {
        dict->list = entry->next;
    }
    if (entry->prev) {
        entry->prev->next = entry->next;
    }
    if(entry->key)
        free(entry->key);
    if (entry->value && entry->is_managed) {
        free(entry->value);
    }
    free(entry);
    dict->count--;
}

void dict_remove_all(dict_t *dict) {
    dict_entry_t * entry = dict->list;
    while (entry) {
        if(entry->key)
            free(entry->key);
        if(entry->value && entry->is_managed)
            free(entry->value);
        dict_entry_t *next = entry->next;
        free(entry);
        entry = next;
    }
    dict->count = 0;
    dict->list = NULL;
}

dict_entry_t *dict_set_string(dict_t *dict, char *key, char *value) {
    char *dup_value = strdup(value);
    dict_entry_t * entry = _dict_set(dict, true, key, (void *)dup_value);
    entry->is_string = true; 
    return entry;
}

char *dict_get_string(dict_t *dict, char *key) {
    dict_entry_t *entry = dict_get(dict, key);
    if (!entry)
    {
        return NULL;
    }
    return entry->value;
}
dict_entry_t *_dict_set(dict_t *dict, bool is_managed, char *key,
                              void *value) {
    dict_entry_t *entry = dict_get(dict, key);
    if (!entry) {
        entry = dict_new_entry();
        entry->key = strdup(key);
        entry->next = dict->list;
        dict->list = entry;
        dict->items[dict_hash(key)] = entry;
        dict->count++;
    } else {
        if (entry->value && entry->is_managed) {
            free(entry->value);
        }
    }
    entry->is_string = false;
    entry->is_managed = is_managed;
    entry->value = value;
    return entry;
}

dict_entry_t *dict_set(dict_t *dict, char *key, void *value) {
    return _dict_set(dict, false, key, value);
}

dict_entry_t *dict_set_managed(dict_t *dict, char *key, void *value) {
    return _dict_set(dict, true, key, value);
}
dict_entry_t *dict_new_entry() {
    dict_entry_t *entry = (dict_entry_t *)calloc(1, sizeof(dict_entry_t));
    entry->key = NULL;
    entry->value = NULL;
    entry->is_managed = false;
    entry->next = NULL;
    entry->prev = NULL;
    return entry;
}
void dict_delete(dict_t *dict) {
    dict_remove_all(dict);
    free(dict);
}

dict_t *dict_new() { return (dict_t *)calloc(1, sizeof(dict_t)); }

#endif