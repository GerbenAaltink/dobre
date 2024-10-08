#define RMALLOC_OVERRIDE 1
#define RMALLOC_DEBUG 1
#include <rlib.h>
#include "class.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main(int argc, char *argv[]){
    char * filepath = argc > 1 ? argv[1] : "../rlib/build/rlib.h";
    //stdout = fopen("/dev/null", "wb");
    nsecs_t time_start = nsecs();
    parse_file(filepath);
    nsecs_t time_end = nsecs();
    nsecs_t  duration = time_end - time_start;
    size_t line_count = get_line_count(filepath);
    printf("Time: %f\n", (float)duration/1000000000.0f);
    printf("Lines per second: %f\n",((float)line_count) / (duration/1000000000.0f));
    printf("Time; %s\n",format_time(duration));
    printf("%s\n",rmalloc_stats());
    return 0;
}