#include "wayland.h"

namespace wayland {

Input* initialize_input(const char *device_name) {
    mark_unused(device_name);
    not_implemented();
}

Input* initialize_input() {
    not_implemented();
}

void destroy_input(Input *input) {
    mark_unused(input);
    not_implemented();
}

bool get_next_button(Input *input, Button *button) {
    mark_unused(input);
    mark_unused(button);
    not_implemented();
}

bool query_button_state(Input *input, Button *button) {
    mark_unused(input);
    mark_unused(button);
    not_implemented();
}

} // namespace wayland
