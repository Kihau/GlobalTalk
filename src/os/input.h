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

// #ifdef LINUX_XORG
// #elif LINUX_WAYLAND
// #elif WINDOWS
// #else
// #endif

enum ButtonState {
    BUTTON_PRESS,
    BUTTON_RELEASE,
};

enum ButtonType {
    INSERT,
    MOUSE_4,
    MOUSE_5,
    OTHER,
};

// NOTE: Buttons usually refer to mouse buttons.
//       Should probably rename this to something else.
struct Button {
    ButtonState state;
    ButtonType type;
};

/// Initialize global input for all devices.
bool initialize_input(Input *input);

/// NOTE: Could probably add support to add multiple devices.
/// Initialize global input for device with specified name.
bool initialize_input(Input *input, const char *device_name);

/// Free global input.
void destroy_input(Input input);

/// NOTE: Could be non-blocking (just like the query_button_state)
///       Windows does non-blocking by default with GetKeyState
///       Linux X11 can do XPending before XNextEvent
/// Get next mouse event. Blocks the current thread.
bool get_next_button(Input input, Button *button);

/// Queries state for selecred device and selected button type.
/// Returns state of the queried button.
bool query_button_state(Input input, Button *button);

#endif // INPUT_H
