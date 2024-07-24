#ifndef WAYLAND_H
#define WAYLAND_H

#include "core/input.h"

//
// TODO
//

namespace wayland {

Input* initialize_input(const char *device_name);
Input* initialize_input();
void destroy_input(Input *input);
bool get_next_button(Input *input, Button *button);
bool query_button_state(Input *input, Button *button);

}


#endif // WAYLAND_H
