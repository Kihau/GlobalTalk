#include "xorg.h"

namespace xorg {

static XDevice *get_device_by_name(Display *display, const char *device_name);
static bool initialize_xi2_events(Input *input, int device_id);
static bool any_class_pressed(XDeviceState *state, int button_code);

Input* initialize_input(const char *device_name) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        log_error("Failed to open X11 display.");
        return NULL;
    }

    Input *input = (Input *)malloc(sizeof(Input));
    input->display = display;

    XDevice *device = get_device_by_name(display, device_name);
    if (device == NULL) {
        log_error("Failed to open device with the name %s", device_name);
        XCloseDisplay(display);
        return NULL;
    }

    input->device = device;

    bool success = initialize_xi2_events(input, device->device_id);
    if (!success) {
        XCloseDevice(display, device);
        XCloseDisplay(display);
        return NULL;
    }

    return input;
}

Input* initialize_input() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        log_error("Failed to open X11 display.");
        return NULL;
    }

    Input *input = (Input *)malloc(sizeof(Input));
    input->display = display;
    input->device  = NULL;

    bool success = initialize_xi2_events(input, XIAllMasterDevices);
    if (!success) {
        XCloseDisplay(display);
        return NULL;
    }

    return input;
}

void destroy_input(Input *input) {
    if (input == NULL) {
        return; 
    }

    if (input->device != NULL) {
        XCloseDevice(input->display, input->device);
        input->device = NULL;
    }

    XCloseDisplay(input->display);
    input->display = NULL;

    free(input);
}

bool get_next_button(Input *input, Button *button) {
    XEvent event;
    XNextEvent(input->display, &event);

    if (event.xcookie.type != GenericEvent || event.xcookie.extension != input->op_code) {
        log_error("Failed to get next event.");
        return false;
    }

    int result = XGetEventData(input->display, &event.xcookie);
    if (result == 0) {
        log_error("Failed to get event data.");
        return false;
    }
    defer { XFreeEventData(input->display, &event.xcookie); };

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

bool query_button_state(Input *input, Button *button) {
    XDeviceState *state = XQueryDeviceState(input->display, input->device);
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

static bool initialize_xi2_events(Input *input, int device_id) {
    int op_code, event_code, error_code;
    if (!XQueryExtension(input->display, "XInputExtension", &op_code, &event_code, &error_code)) {
        log_error("X Input extension is not available.");
        return false;
    }

    // TODO: Perform version check here?

    // NOTE: Since XInputExtension version 2.1 (released in 2011) event grabs no longer affect raw events. 
    //       The previous, 2.0, version is buggy in that regard.
    //       https://rafaelgieschke.github.io/xorg-xorgproto/XI2proto.html
    int major = 2, minor = 1;
    if (XIQueryVersion(input->display, &major, &minor) == BadRequest) {
        log_error("XI2 not available. Server supports %d.%d.", major, minor);
        return false;
    }

    unsigned char mask[8] = { 0 };
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);
    XISetMask(mask, XI_RawKeyPress);

    XIEventMask event_mask = {
        .deviceid = device_id,
        .mask_len = sizeof(mask),
        .mask = mask,
    };

    Window window = DefaultRootWindow(input->display);
    int result = XISelectEvents(input->display, window, &event_mask, 1);
    if (result != Success) {
        log_error("Failed to select XI2 events.");
        return false;
    }

    input->op_code = op_code;
    return true;
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

}
