#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct test_definition_t {
    char *script;
    char *expectation;
} test_definition_t;

typedef struct test_t {
    test_definition_t definition;
    bool valid;
} test_t;

test_t *parse_test(char **content_str) {
    char *content = *content_str;

    // define start
    char *script_start = strstr(content, "```");
    if (!script_start) {
        return NULL;
    }
    script_start += 3;

    // define end of script
    char *script_end = strstr(script_start, "```");
    if (!script_end) {
        printf("Error, missing end of script.\n");
        return NULL;
    }
    unsigned int script_length = script_end - script_start;
    char script[script_length + 1];
    script[script_length] = 0;
    strncpy(script, script_start, script_length);
    script_end += 3;

    // define start or expectation
    char *expectation_start = strstr(script_end, "```");
    if (!expectation_start) {
        printf("Error, missing expectation start.\n");
        return NULL;
    }
    expectation_start += 3;

    // define end of expectation
    char *expectation_end = strstr(expectation_start, "```");
    if (!expectation_end) {
        printf("Error: missing expectation end.\n");
        return NULL;
    }
    unsigned int expectation_length = expectation_end - expectation_start;
    char expectation[expectation_length + 1];
    expectation[expectation_length] = 0;
    strncpy(expectation, expectation_start, expectation_length);
    expectation_end += 3;

    // prepare return value
    test_t *test = (test_t *)malloc(sizeof(test_t));
    test->definition.script = (char *)malloc(script_length + 1);
    test->definition.expectation = (char *)malloc(expectation_length + 1);
    strcpy(test->definition.script, script);
    strcpy(test->definition.expectation, expectation);
    test->valid = false;

    // update pointer
    *content_str = expectation_end;

    return test;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: <executable> <*files>");
        exit(1);
    }

    char *execute = argv[1];
    stderr = stdout;
    FILE *f = popen(execute, "r");
    if (!f) {
        printf("Couldn't execute %s.\n", execute);
        return 1;
    }
    char data[2048];
    fread(data, 1, sizeof(data), f);
    printf("%s\n", data);
    fclose(f);

    char *file_ptr = "```a``````b``````c``````d```";
    test_t *test;
    while ((test = parse_test(&file_ptr)) != NULL) {
        printf("source: <%s> expectation: <%s>\n", test->definition.script,
               test->definition.expectation);
    }

    return 0;
}
