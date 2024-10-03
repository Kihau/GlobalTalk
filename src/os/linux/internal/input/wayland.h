#ifndef WAYLAND_H
#define WAYLAND_H

#include "core/input.h"

//
// TODO:
// The Wayland implementation is going to be a massive hack that makes use of the shared library injection and
// takes advantage of the fact that the Wayland protocol is going to run with the same permissions as my app.
//
// This definitely IS NOT a good way to do this, but the Wayland protocol does not provide a way to read 
// global inputs (as far as I'm concerned), so the hack is the only option here.
//

namespace wayland {

Input* initialize_input(const char *device_name);
Input* initialize_input();
void destroy_input(Input *input);
bool get_next_button(Input *input, Button *button);
bool query_button_state(Input *input, Button *button);

}


#endif // WAYLAND_H
