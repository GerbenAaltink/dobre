#define RMALLOC_OVERRIDE 0
#define RMALLOC_DEBUG 1
#include <rlib.h>
#define print rprintb

int printf_dummy(const char *format, ...) {
    (void)format;
    return 0;
}
#ifdef printf_a
#undef printf
#define printf printf_dummy
#endif


#include "class.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    char *filepath = argc > 1 ? argv[1] : "../scripts/valgrind.dob";
    nsecs_t time_start = nsecs();
    parse_file(filepath);
    nsecs_t time_end = nsecs();
    nsecs_t duration = time_end - time_start;
    size_t line_count = get_line_count(filepath);
    print("Time: %f\n", (float)duration / 1000000000.0f);
    print("Lines per second: %f\n",
          ((float)line_count) / (duration / 1000000000.0f));
    print("Time; %s\n", format_time(duration));
    print("%s\n", rmalloc_stats());
    return 0;
}
