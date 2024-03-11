#ifndef INPUT_H
#define INPUT_H

#include "../utils.h"

#ifdef __linux__ 
    #include <X11/extensions/XInput.h>
    #include <X11/extensions/XInput2.h>
    #include <cstring>

    struct Input {
        Display *display;
        int op_code;
        XDevice *device;
    };
#elif _WIN32
    struct Input {
        // TODO
        int dummy;
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

/// Initialize global input.
bool initialize_input(Input *input);

/// Free global input.
void destroy_input(Input input);

/// Get next mouse event. Blocks the current thread.
bool get_next_button(Input input, Button *button);

/// Opens selected input device.
bool open_device(Input *input, char *device_name);

/// Queries state for selecred device and selected button type.
/// Returns state of the queried button.
bool query_button_state(Input input, Button *button);

#endif // INPUT_H
