#ifndef DOBRE_MAIN
#define DOBRE_MAIN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef strdup
char *_dobre_strdup(const char *s) {
    if (!s)
        return NULL;

    char *result;
    rmalloc_count++;
    rmalloc_alloc_count++;
    size_t size = strlen(s) + 1;
    while (!(result = (char *)malloc(size))) {
        fprintf(stderr, "Warning: strdup failed, trying again.\n");
    }
    memcpy(result, s, size);
    return result;
}
#define strdup _dobre_strdup
#endif

#endif