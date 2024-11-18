#include "dict.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    printf("Testing dict.\n");
    dict_t * dict = dict_new();
    dict_set_string(dict, "str1", "value1");
    assert(!strcmp(dict_get_string(dict,"str1"), "value1"));
    assert(dict->count == 1);
    dict_remove(dict, "str1");
    assert(dict->count == 0);
    assert(dict->list == NULL);
    assert(dict_get_string(dict,"str1") == NULL);
    dict_set_string(dict, "str2", "value2");
    assert(!strcmp(dict_get_string(dict,"str2"), "value2"));
    assert(dict->count == 1);
    dict_remove(dict, "str2");
    assert(dict->count == 0);
    assert(dict->list == NULL);
    return 0;
    assert(dict_get_string(dict,"str2") == NULL);
    dict_set_string(dict, "str3", "value3");
    assert(!strcmp(dict_get_string(dict,"str3"), "value3"));
    assert(dict->count == 1);
    dict_remove(dict, "str3");
    assert(dict->count == 0);
    assert(dict->list == NULL);
    assert(dict_get_string(dict,"str3") == NULL);

    dict_delete(dict);
    
    return 0;   
}