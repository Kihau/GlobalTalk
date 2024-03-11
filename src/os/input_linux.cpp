#include "input.h"

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

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        log_error("XI2 not available. Server supports %d.%d.", major, minor);
        return false;
    }

    unsigned char mask[8] = { 0 };
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);

    XIEventMask event_mask = {
        .deviceid = XIAllDevices,
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

    had_error = false;
    return true;
}

void destroy_input(Input input) {
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
    }

    return true;
}

static bool button_pressed(XButtonState *state, int button_number) {
    if (button_number / 8 <= state->num_buttons) {
        return state->buttons[button_number / 8] >> (button_number % 8) & 1;
    }

    return false;
}


bool open_device(Input *input, char *device_name) {
    int device_count;
    XDeviceInfo *device_list = XListInputDevices(input->display, &device_count);
    if (device_list == NULL) {
        log_error("Quering devices failed");
        return false;
    }
    defer { XFreeDeviceList(device_list); };

    for (int h = 0; h < device_count; h++) {
        XDeviceInfo *info = &device_list[h];
        if (info->use != IsXExtensionPointer) {
            continue;
        }

        XDevice *device = XOpenDevice(input->display, info->id);
        if (device == NULL) {
            continue;
        }

        if (strcmp(device_name, info->name) == 0) {
            input->device = device;
            return true;
        }
    }

    return false;
}

bool query_button_state(Input input, Button *button) {
    XDeviceState *state = XQueryDeviceState(input.display, input.device);
    if (state == NULL) {
        return false;
    }
    defer { XFreeDeviceState(state); };

    XInputClass *input_classes = state->data;
    for (int i = 0; i < state->num_classes; i++) {
        XInputClass *input_class = &input_classes[i];
        if (input_class->c_class != ButtonClass) {
            continue;
        }

        XButtonState *button_state = (XButtonState *)input_class;
        switch (button->type) {
            case ButtonType::MOUSE_4: {
                if (button_pressed(button_state, 9)) {
                    button->state = BUTTON_PRESS;
                    return true;
                }
            } break;

            case ButtonType::MOUSE_5: {
                if (button_pressed(button_state, 8)) {
                    button->state = BUTTON_PRESS;
                    return true;
                }
            } break;

            case OTHER: break;
        }
    }
    button->state = BUTTON_RELEASE;
    return true;
}

bool query_button_state_old(Input input, Button *button) {
    int device_count;
    XDeviceInfo *device_list = XListInputDevices(input.display, &device_count);
    if (device_list == NULL) {
        log_error("Quering devices failed");
        return false;
    }
    defer { XFreeDeviceList(device_list); };


    for (int h = 0; h < device_count; h++) {
        XDeviceInfo *info = &device_list[h];
        if (info->use != IsXExtensionPointer) {
            continue;
        }

        XDevice *device = XOpenDevice(input.display, info->id);
        if (device == NULL) {
            continue;
        }
        defer { XCloseDevice(input.display, device); };

        XDeviceState *state = XQueryDeviceState(input.display, device);
        if (state == NULL) {
            continue;
        }
        defer { XFreeDeviceState(state); };

        XInputClass *input_classes = state->data;
        for (int i = 0; i < state->num_classes; i++) {
            XInputClass *input_class = &input_classes[i];
            if (input_class->c_class != ButtonClass) {
                continue;
            }

            XButtonState *button_state = (XButtonState *)input_class;
            switch (button->type) {
                case ButtonType::MOUSE_4: {
                    if (button_pressed(button_state, 9)) {
                        button->state = BUTTON_PRESS;
                        return true;
                    }
                } break;

                case ButtonType::MOUSE_5: {
                    if (button_pressed(button_state, 8)) {
                        button->state = BUTTON_PRESS;
                        return true;
                    }
                } break;

                case OTHER: break;
            }
        }
    }

    button->state = BUTTON_RELEASE;
    return true;
}
