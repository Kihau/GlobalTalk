#include "input.h"

bool initialize_input(Input *input) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Failed to open X11 display\n");
        return false;
    }

    int op_code, event_code, error_code;
    if (XQueryExtension(display, "XInputExtension", &op_code, &event_code, &error_code) == 0) {
        log_error("X Input extension is not available.\n");

        XCloseDisplay(display);
        return false;
    }

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        log_error("XI2 not available. Server supports %d.%d\n", major, minor);

        XCloseDisplay(display);
        return false;
    }

    unsigned char mask[8] = { 0 };
    XISetMask(mask, XI_ButtonPress);
    XISetMask(mask, XI_ButtonRelease);

    XIEventMask event_mask = {
        .deviceid = XIAllDevices,
        .mask_len = sizeof(mask),
        .mask = mask,
    };

    Window window = DefaultRootWindow(display);
    XISelectEvents(display, window, &event_mask, 1);

    input->display = display;
    input->op_code = op_code;

    return true;
}

void destroy_input(Input input) {
    XCloseDisplay(input.display);
}

bool get_next_button(Input input, Button *button) {
    XEvent event;
    XNextEvent(input.display, &event);

    if (event.xcookie.type != GenericEvent || event.xcookie.extension != input.op_code) {
        return false;
    }

    int result = XGetEventData(input.display, &event.xcookie);
    if (result == 0) {
        return false;
    }
    defer { XFreeEventData(input.display, &event.xcookie); };

    switch (event.xcookie.evtype) {
        case XI_ButtonPress: {
            XIDeviceEvent *device_event = (XIDeviceEvent *)event.xcookie.data;
            KeyCode code = device_event->detail;

            button->state = ButtonState::BUTTON_PRESS;
            switch (code) {
                case 9:  button->type = ButtonType::MOUSE_4; break;
                case 8:  button->type = ButtonType::MOUSE_5; break;
                default: button->type = ButtonType::OTHER;   break;
            }
        } break;

        case XI_ButtonRelease: {
            XIDeviceEvent *device_event = (XIDeviceEvent *)event.xcookie.data;
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
