#include "lexer.h"
#include <rlib.h>

int main() {
    printf("Testing lexer.c\n");
    char *script = "class Myclass{}\n"
                   "MyClass c = 3\n"
                   "class MyClass2(MyClass){}\n";
    lexer_t *lexer = lexer_new();
    nsecs_t time_start = nsecs();
    lexer_parse(lexer, script);
    nsecs_t time_end = nsecs();
    nsecs_t duration = time_end - time_start;
    printf("Lines per second: %f\n",
           ((float)lexer->lines) / (duration / 1000000000.0f));
    printf("Lines: %d\n", lexer->lines);
    printf("Token count: %zu\n", lexer->count);
    lexer_delete(lexer);
    printf(rmalloc_stats());
    printf("\n");
    if (rmalloc_count) {
        rprintr("MEMORY ERROR\n");
        return 1;
    }
    return 0;
}