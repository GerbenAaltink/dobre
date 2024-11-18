#include <rlib.h>

#include "lexer.h"

char * generate_script(size_t size){
    char *small_script = "    // comment\n"
                   "    /* multiline comment */\n"
                   "    class Number{}\n"
                   "    class String{}\n"
                   "    class Boolean{}\n"
                   "    Number number1 = -1\n"
                   "    Number number2 = 2\n"
                   "    Number number3 = 3.0\n"
                   "    Number number4 = -3.0\n"
                   "    String string1 = \"\"\n"
                   "    String string2 = \"2\"\n"
                   "    Boolean bool1 = true\n"
                   "    Boolean bool2 = true\n"
                   "    class Myclass{}\n"
                   "    class MyClass2(MyClass){}\n"
                   "    for(MyClass i = 0 j = 20 k = 40){\n"
                   "        Number number = 55\n"
                   "    }\n";
    char * script = (char *)calloc(size + 10000,sizeof(char));
    strcpy(script, small_script);
    while(strlen(script) * 2 < size + 10000) {
        char * script2 = strdup(script);
        strcat(script, script2);
        free(script2);   
   }
   return script;
}

int main() {
    printf("Testing lexer.c\n");
    char *script = generate_script(1024*1024*80);
    lexer_t *lexer = lexer_new();
    nsecs_t time_start = nsecs();
    lexer_parse(lexer, script);
    nsecs_t time_end = nsecs();
    nsecs_t duration = time_end - time_start;
    printf("Size: %f\n", (double)strlen(script) / (1024 * 1024));
    printf("Lines per second: %f\n",
           ((float)lexer->lines) / (duration / 1000000000.0f));
    printf("Lines: %d\n", lexer->lines);
    printf("Duration: %s\n",format_time(duration));
    printf("Token count: %zu\n", lexer->count);
    lexer_delete(lexer);
    printf(rmalloc_stats());
    printf("\n");
    free(script);
    if (rmalloc_count) {
        rprintr("MEMORY ERROR\n");
        return 0; // 1;
    }
    return 0;
}