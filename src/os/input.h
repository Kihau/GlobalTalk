#ifndef INPUT_H
#define INPUT_H

#include <cstdio>
#include "../utils.h"

#ifdef __linux__ 
    #include <X11/extensions/XInput2.h>

    struct Input {
        Display *display;
        int op_code;
    };
#elif _WIN32
    struct Input {
        // TODO
    };
#endif

enum ButtonState {
    BUTTON_PRESS,
    BUTTON_RELEASE,
};

enum ButtonType {
    MOUSE_4,
    MOUSE_5,
    OTHER,
};

struct Button {
    ButtonState state;
    ButtonType type;
};

bool initialize_input(Input *input);
void destroy_input(Input input);
bool get_next_button(Input input, Button *button);

#endif // INPUT_H
