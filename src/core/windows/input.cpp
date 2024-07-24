#include "os/input.h"

#include <cstdlib>

struct Input {
    int dummy;
};

Input* initialize_input(const char *device_name) {
    Input *input = (Input *)malloc(sizeof(Input));
    not_implemented();
    return input;
}

Input* initialize_input() {
    Input *input = (Input *)malloc(sizeof(Input));
    not_implemented();
    return input;
}

void destroy_input(Input *input) {
    not_implemented();
}

bool get_next_button(Input *input, Button *button) {
    not_implemented();
    return true;
}

bool query_button_state(Input *input, Button *button) {
    not_implemented();
    return false;
}
