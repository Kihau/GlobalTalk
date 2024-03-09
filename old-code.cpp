#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

int main(int argc, char **argv) {
    Display *display = XOpenDisplay(NULL);

    // {
    //     int device_count;
    //     XIDeviceInfo *all_devices = XIQueryDevice(display, XIAllDevices, &device_count);
    //
    //     for (int i = 0; i < device_count; i++) {
    //         XIDeviceInfo *device = &all_devices[i];
    //         printf("Device %i = %s\n", i + 1, device->name);
    //     }
    //
    //
    //     XIFreeDeviceInfo(all_devices);
    // }

    int device_count;
    XDeviceInfo *device_infos = XListInputDevices(display, &device_count);
    for (int i = 0; i < device_count; i++) {
        XDeviceInfo *info = &device_infos[i];
        printf("Device %i = %s\n", i, info->name);
    }

    XDeviceInfo *info = &device_infos[14];
    Atom a = info->type;
    XDevice *device = XOpenDevice(display, info->id);

    XDeviceState *state = XQueryDeviceState(display, device);
    // XButtonState *button_state = (XButtonState *)state->data;
    // for (int i = 0; i < button_state->num_buttons; i++) {
    //     printf("state %i = %i\n", i, button_state->buttons[i]);
    // }

    XInputClass *data = state->data;
    for (int _i = 0; _i < state->num_classes; _i++) {
        if (data->class != ButtonClass) {
            data = (XInputClass *)((char *)data + data->length);
            continue;
        }

        XButtonState *button_state = (XButtonState *)data;
        for (int i = 1; i < button_state->num_buttons; i++) {
            printf("state %i = %i\n", i, button_state->buttons[i]);
        }

        data = (XInputClass *)((char *)data + data->length);
    }

    XCloseDisplay(display);
    return 0;
}
