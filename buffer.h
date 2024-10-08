#ifndef BUFFER_H
#define BUFFER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
typedef struct buffer_t {
    char *ptr;
    char *data;
    size_t size;
    size_t pos;
    bool eof;
    unsigned int line;
    unsigned int col;
} buffer_t;

buffer_t *buffer_new(char *data, size_t size);
void buffer_delete(buffer_t *buff);
void buffer_reset(buffer_t *buff);
void buffer_write(buffer_t *buff, const char *data, size_t size);
size_t buffer_push(buffer_t *buff, char);
char buffer_pop(buffer_t *buff);
char * buffer_expect(buffer_t *buff, char * options,char * ignore);
void buffer_set(buffer_t *buff, const char *data, size_t size);   
void buffer_unread(buffer_t *buff, unsigned int);

void buffer_unread(buffer_t *buff, unsigned int count){
    if(count > buff->pos){
        return ;
    }
    buff->pos -= count;
    buff->ptr -= count;
}

void buffer_set(buffer_t *buff, const char * data, size_t size){
    if(buff->ptr){
        free(buff->ptr);
        buff->ptr = NULL;
        buff->eof = true;
    }
    if(size){
        buff->data = (char *)malloc(size);
        memcpy(buff->data,data,size);
        buff->ptr = buff->data;
        buff->eof = false;
    }
    buff->line = 0;
    buff->col = 0;
    buff->size = size;
    buff->pos = 0;
}

buffer_t * buffer_new(char * data, size_t size){
    buffer_t *buff = (buffer_t *)malloc(sizeof(buffer_t));
    if(size){
        buff->data = (char *)malloc(size);
        memcpy(buff->data,data,size);   
        buff->ptr = buff->data; 
        buff->eof = false;
    }else{
        buff->ptr = NULL;
        buff->eof = true;
        buff->data = NULL;
        buff->ptr = NULL;
    }
    buff->line = 0;
    buff->col = 0;
    buff->size = size;
    buff->pos = 0;
    return buff;
}
void buffer_delete(buffer_t * buff){
    if(buff->data){
       free(buff->data);
    }
    free(buff);
}

size_t buffer_push(buffer_t *buff, char c){
    if(buff->pos < buff->size){
        buff->ptr[buff->pos++] = c;
    
        buff->ptr[buff->pos] = 0;
        return 1;
    }
    buff->data = realloc(buff->data,buff->size+2);
    buff->ptr = buff->data;
    buff->ptr[buff->pos++] = c;
    buff->ptr[buff->pos] = 0;
    buff->size++;
   
    return buff->pos;
}
void buffer_write(buffer_t *buff, const char *data, size_t size){
    char * data_ptr = (char *)data;
    for(size_t i = 0; i < size; i++){
        buffer_push(buff,data_ptr[i]);
    }
}

char buffer_peek(buffer_t *buff){
    char result = EOF;
    if(buff->pos != buff->size){
        result = *(buff->ptr);
        return result;
    }
    buff->eof = true;
    return EOF;
    
}
char buffer_pop(buffer_t *buff){
    if(buff->pos && *(buff->ptr - 1) == '\n'){
        buff->line++;
        buff->col = 0;
    }else{
        buff->col++;
    }
    
    char result = EOF;
    if(buff->pos < buff->size){
        buff->ptr = buff->data + buff->pos;
        result = *buff->ptr;
        
        buff->pos++;
        buff->ptr++;
        if(buff->pos == buff->size){
            buff->eof = true;
        }
        return result;
    }
    buff->eof = true;
    return result;
}
void buffer_reset(buffer_t *buff){
    if(buff->ptr)
        buff->ptr -= buff->pos;
    buff->pos = 0;
    buff->eof = false;
}

char * buffer_to_str(buffer_t *buff){
    char * result = buff->data; 
    buff->data = NULL;
    buff->ptr = NULL;
    buffer_delete(buff);
    return result;
}


char ustrncmp(const char *s1, const char *s2, size_t n) {
    return strncmp((char *)s1, (char *)s2,n);
    while (n && *s1 == *s2) {
        n--;
        s1++;
        s2++;
    }
    return *s1 != *s2;
}
size_t ustrlen(const char *s) {
    return strlen((char *)s);
}


char * string_match_option(char * buff,char * options){
    char * option = NULL;
    char options_cpy[1024] = {0};
    strcpy(options_cpy,options);
    char * memory = options_cpy;
    size_t buff_length = strlen(buff);
    while((option = strtok_r(option == NULL ? memory : NULL, "|", &memory)) != NULL){
        bool reverse = option[0] == '^';
        if(reverse){
            option++;
        }
        size_t option_length = strlen(option);
        if(option_length > buff_length){
            continue;
        }
        if(!strcmp(option,"\\d") && *buff >= '0' && *buff <= '9'){
            if(reverse){
                return NULL;
            }
            return buff;
        }
        if(buff_length >= 5 && !strcmp(option,"\\b") && ((!strncmp(buff,"true",4) || !strncmp(buff,"false",5)))){
            if(reverse){
                return NULL;
            }
            return buff;
        }
        if(!ustrncmp(buff,(char *)option,option_length)){
            if(reverse){
                return NULL;
            }
            return buff;
        }
    }
    return NULL;
}

char * buffer_match_option(buffer_t * buff,char * options){
    return string_match_option(buff->ptr,options);
}

char * buffer_expect(buffer_t * buff, char * options,char * ignore){
    while(buff->pos < buff->size){
        if(buffer_match_option(buff,options) != NULL){
            return buff->ptr;
        }
        printf("MATCIHGG\n");
        if(buffer_match_option(buff, ignore)){
            printf("SKIP:%s\n",buff->ptr);
            buffer_pop(buff);
            continue;
        }
        break;
    }
    return NULL;
}
char * buffer_consume(buffer_t * buff, char * options, char *ignore){
    char * result = NULL;
    if((result = buffer_expect(buff,options,ignore)) != NULL){
        printf("HAAA%s\n", result);
        buffer_pop(buff);
    }
    return result;
}
#endif
