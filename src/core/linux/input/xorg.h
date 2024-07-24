#ifndef XORG_H
#define XORG_H

#include "core/input.h"

#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <cstring>
#include <cstdlib>

namespace xorg {

struct Input {
    Display *display;
    int op_code;
    XDevice *device;
};

Input* initialize_input();
Input* initialize_input(const char *device_name);
void   destroy_input(Input *input);
bool   get_next_button(Input *input, Button *button);
bool   query_button_state(Input *input, Button *button);

}

#endif // XORG_H
