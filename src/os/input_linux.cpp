#include "input.h"
#include <cstdlib>

static XDevice *get_device_by_name(Display *display, const char *device_name);
static bool any_class_pressed(XDeviceState *state, int button_code);

bool initialize_input(Input *input, const char *device_name) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        log_error("Failed to open X11 display.");
        return false;
    }

    bool had_error = true;
    defer { if (had_error) XCloseDisplay(display); };


    XDevice *device = get_device_by_name(display, device_name);
    if (device == NULL) {
        log_error("Failed to open device with the name %s", device_name);
        return false;
    }

    int op_code, event_code, error_code;
    if (!XQueryExtension(display, "XInputExtension", &op_code, &event_code, &error_code)) {
        log_error("X Input extension is not available.");
        return false;
    }

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        log_error("XI2 not available. Server supports %d.%d.", major, minor);
        return false;
    }

    unsigned char mask[8] = { 0 };
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);
    XISetMask(mask, XI_RawKeyPress);

    XIEventMask event_mask = {
        .deviceid = (int)device->device_id,
        .mask_len = sizeof(mask),
        .mask = mask,
    };

    Window window = DefaultRootWindow(display);
    int result = XISelectEvents(display, window, &event_mask, 1);
    if (result != Success) {
        log_error("Failed to select XI2 events.");
        return false;
    }

    input->display = display;
    input->op_code = op_code;
    input->device  = device;

    had_error = false;
    return true;
}

bool initialize_input(Input *input) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        log_error("Failed to open X11 display.");
        return false;
    }

    bool had_error = true;
    defer { if (had_error) XCloseDisplay(display); };

    int op_code, event_code, error_code;
    if (!XQueryExtension(display, "XInputExtension", &op_code, &event_code, &error_code)) {
        log_error("X Input extension is not available.");
        return false;
    }

    int major = 2, minor = 2;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        log_error("XI2 not available. Server supports %d.%d.", major, minor);
        return false;
    }

    u8 mask[8] = {};
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);
    XISetMask(mask, XI_RawKeyPress);

    XIEventMask event_mask = {
        .deviceid = XIAllMasterDevices,
        .mask_len = sizeof(mask),
        .mask = mask,
    };

    Window window = DefaultRootWindow(display);
    int result = XISelectEvents(display, window, &event_mask, 1);
    if (result != Success) {
        log_error("Failed to select XI2 events.");
        return false;
    }

    input->display = display;
    input->op_code = op_code;
    input->device  = NULL;

    had_error = false;
    return true;
}

void destroy_input(Input input) {
    if (input.device != NULL) {
        XCloseDevice(input.display, input.device);
        input.device = NULL;
    }

    XCloseDisplay(input.display);
    input.display = NULL;
}

bool get_next_button(Input input, Button *button) {
    // NOTE: 
    //     Other apps can grab mouse pointer by using the XGrabPointer function.
    //     This disables event polling for the function call below.
    //     Because of this, query-ing the mouse state might be more reliable.
    XEvent event;
    XNextEvent(input.display, &event);

    if (event.xcookie.type != GenericEvent || event.xcookie.extension != input.op_code) {
        log_error("Failed to get next event.");
        return false;
    }

    int result = XGetEventData(input.display, &event.xcookie);
    if (result == 0) {
        log_error("Failed to get event data.");
        return false;
    }
    defer { XFreeEventData(input.display, &event.xcookie); };

    switch (event.xcookie.evtype) {
        case XI_RawButtonPress: {
            auto device_event = (XIRawEvent *)event.xcookie.data;
            KeyCode code = device_event->detail;

            button->state = ButtonState::BUTTON_PRESS;
            switch (code) {
                case 9:  button->type = ButtonType::MOUSE_4; break;
                case 8:  button->type = ButtonType::MOUSE_5; break;
                default: button->type = ButtonType::OTHER;   break;
            }
        } break;

        case XI_RawButtonRelease: {
            auto device_event = (XIRawEvent *)event.xcookie.data;
            KeyCode code = device_event->detail;

            button->state = ButtonState::BUTTON_RELEASE;
            switch (code) {
                case 9:  button->type = ButtonType::MOUSE_4; break;
                case 8:  button->type = ButtonType::MOUSE_5; break;
                default: button->type = ButtonType::OTHER;   break;
            }
        } break;

        case XI_RawKeyPress: {
            auto device_event = (XIRawEvent *)event.xcookie.data;
            KeyCode code = device_event->detail;

            button->state = ButtonState::BUTTON_PRESS;
            switch (code) {
                case 90: button->type = ButtonType::INSERT; break;
                default: button->type = ButtonType::OTHER;  break;
            }
        } break;

        default: {
            log_warning("Got unhandled event type with code: %i", event.xcookie.evtype);
            return false;
        }
    }

    return true;
}

bool query_button_state(Input input, Button *button) {
    XDeviceState *state = XQueryDeviceState(input.display, input.device);
    if (state == NULL) {
        return false;
    }

    switch (button->type) {
        case ButtonType::MOUSE_4:
            button->state = any_class_pressed(state, 9) ? BUTTON_PRESS : BUTTON_RELEASE;
            return true;

        case ButtonType::MOUSE_5:
            button->state = any_class_pressed(state, 8) ? BUTTON_PRESS : BUTTON_RELEASE;
            return true;

        case OTHER: break;
    }

    return false;
}

static XDevice *get_device_by_name(Display *display, const char *device_name) {
    int device_count;
    XDeviceInfo *device_list = XListInputDevices(display, &device_count);
    if (device_list == NULL) {
        log_error("Quering devices failed");
        return NULL;
    }
    defer { XFreeDeviceList(device_list); };

    for (int h = 0; h < device_count; h++) {
        XDeviceInfo *info = &device_list[h];
        if (info->use != IsXExtensionPointer) {
            continue;
        }

        XDevice *device = XOpenDevice(display, info->id);
        if (device == NULL) {
            continue;
        }

        if (strcmp(device_name, info->name) == 0) {
            return device;
        }
    }

    return NULL;
}

static bool any_class_pressed(XDeviceState *state, int button_code) {
    for (int i = 0; i < state->num_classes; i++) {
        XInputClass *input_class = &state->data[i];
        if (input_class->c_class != ButtonClass) {
            continue;
        }

        auto button_state = (XButtonState *)input_class;
        if (button_code >= button_state->num_buttons) {
            continue;
        }

        int array_index = button_code / 8;
        int bit_index = button_code - array_index * 8;
        return (button_state->buttons[array_index] >> bit_index) & 1;
    }

    return false;
}
