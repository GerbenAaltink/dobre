#include <rlib.h>

#include "lexer.h"

int main() {
    printf("Testing lexer.c\n");
    char *script = "    // comment\n"
                   "    /* multiline comment */\n"
                   "    Number number1 = -1\n"
                   "    Number number2 = 2\n"
                   "    Number number3 = 3.0\n"
                   "    Number number4 = -3.0\n"
                   "    String string1 = \"\"\n"
                   "    String string2 = \"2\"\n"
                   "    Boolean bool1 = true\n"
                   "    Boolean bool2 = true\n"
                   "    class Myclass{}\n"
                   "    class MyClass2(MyClass){}\n";
    printf("Source:\n%s", script);
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
        return 0; // 1;
    }
    return 0;
}