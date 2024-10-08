#include "buffer.h"

int main() {

    buffer_t *buff = buffer_new(NULL, 0);
    buffer_write(buff, "hello", 5);

    assert(buffer_pop(buff) == EOF);

    buffer_reset(buff);
    assert(buffer_pop(buff) == 'h');
    assert(buffer_pop(buff) == 'e');
    assert(buffer_pop(buff) == 'l');
    assert(buffer_pop(buff) == 'l');
    assert(buffer_pop(buff) == 'o');
    assert(buffer_pop(buff) == EOF);

    buffer_reset(buff);
    assert(buff->eof == false);
    assert(buff->pos == 0);
    assert(buff->size == 5);

    assert(!string_match_option("test", "^test"));
    assert(string_match_option("test", "test"));
    assert(string_match_option("a", "d|e|f|a"));
    //", buffer_to_str(buff));
}