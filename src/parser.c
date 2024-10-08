#include <rlib.h>

#define print rprintb
int printf_dummy(const char *format, ...) {
    (void)format;
    return 0;
}
#ifdef printf
#undef printf
#endif
#define printf printf_dummy

#include "parser.h"


unsigned int parse_only(char *script) {
    lexer_t * lexer = lexer_new();
     lexer = lexer_parse(lexer, script);
    parse(lexer);
    unsigned int result = lexer->lines;
    lexer_delete(lexer);
    return result;
}
void lex_and_parse(char * script){
     parse_string(script);
}

int main() {
    char *script = "    // comment\n"
                   "    /* multiline comment */\n"
                   "    class Number{}\n"
                   "    class String{}\n"
                   "    class Boolean{}\n"
                   "    Number number1 = -1\n"
                   "    Number number2 = 2\n"
                   "    Number number3 = 3.0\n"
                   "    Number number4 = -3.0\n"
                   "    String string1 = 5\n"
                   "    String string2 = 4\n"
                   "    Boolean bool1 = 1\n"
                   "    Boolean bool2 = 0\n"
                   "    class Myclass(){}\n"
                   "    class MyClass2(MyClass){}";
    nsecs_t time_start = nsecs();
    lexer_t * lexer = lexer_new();
     lexer = lexer_parse(lexer, script);
    parse(lexer);
    unsigned int lines = lexer->lines;
    unsigned int token_count = lexer->count;
    lexer_delete(lexer);
    nsecs_t time_end = nsecs();
    nsecs_t duration = time_end - time_start;
    print("Full parsing (lex+parse):\n");
    print("Time: %s\n", format_time(duration));
     print("Lines per second: %f\n",
           ((float)lines) / (duration / 1000000000.0f));
    
      time_start = nsecs();
    parse_string(script);
     time_end = nsecs();
     duration = time_end - time_start;
    print("Full parsing (lex+parse):\n");
    print("Time: %s\n", format_time(duration));
     print("Lines per second: %f\n",
           ((float)lines) / (duration / 1000000000.0f));
    
    print("Lines: %d\n", lines);
    print("Token count: %zu\n", token_count); 
    print("%s\n", rmalloc_stats());
    return 0;
}