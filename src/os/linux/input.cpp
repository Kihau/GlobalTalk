#include "os/input.h"

// This file could tie x11 and wayland together in a windowsing system independent way...

Input* initialize_input(const char *device_name) {
    // If X11 filed to initialize - use wayland.
    not_implemented();
}

Input* initialize_input() {
    not_implemented();
}

void destroy_input(Input *input) {
    not_implemented();
}

bool get_next_button(Input *input, Button *button) {
    not_implemented();
}

bool query_button_state(Input *input, Button *button) {
    not_implemented();
}

