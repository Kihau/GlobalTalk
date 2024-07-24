#include "core/input.h"
#include "input/xorg.h"

// This file could tie x11 and wayland together in a windowsing system independent way...

Input* initialize_input(const char *device_name) {
    return (Input *)xorg::initialize_input(device_name);
}

Input* initialize_input() {
    return (Input *)xorg::initialize_input();
}

void destroy_input(Input *input) {
    xorg::destroy_input((xorg::Input *)input);
}

bool get_next_button(Input *input, Button *button) {
    return xorg::get_next_button((xorg::Input *)input, button);
}

bool query_button_state(Input *input, Button *button) {
    return xorg::query_button_state((xorg::Input *)input, button);
}

