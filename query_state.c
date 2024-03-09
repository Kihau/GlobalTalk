#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

XDeviceInfo *find_device_info(Display *display, const char *name) {
    // Bool is_id = True;
    // int len = strlen(name);
    // for (int loop = 0; loop < len; loop++) {
    //     if (!isdigit(name[loop])) {
    //         is_id = False;
    //         break;
    //     }
    // }

    // XID id = (XID)-1;
    // if (is_id) {
    //     id = atoi(name);
    // }

    int device_count;
    XDeviceInfo *devices = XListInputDevices(display, &device_count);

    XDeviceInfo *found = NULL;
    for (int i = 0; i < device_count; i++) {
        XDeviceInfo device = devices[i];
        printf("Device %i = atom: %i, name:%s\n", i, device.type, device.name);

        if (device.id == 13) {
            return &devices[i];
        }

        // if (
        //     devices[i].use >= IsXExtensionDevice && 
        //     ((!is_id && strcmp(devices[i].name, name) == 0) || (is_id && devices[i].id == id))
        // ) {
        //     if (found) {
        //         fprintf(
        //             stderr,
        //             "Warning: There are multiple devices named '%s'.\n"
        //             "To ensure the correct one is selected, please use "
        //             "the device ID instead.\n\n",
        //             name
        //         );
        //         return NULL;
        //     } else {
        //         found = &devices[i];
        //     }
        // }
    }
    return found;
}

int query_state() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Failed to open X11 display\n");
        return 1;
    }

    const char *device_name = "SteelSeries SteelSeries Rival 100 Gaming Mouse";
    XDeviceInfo *info = find_device_info(display, device_name);
    if (!info) {
        fprintf(stderr, "unable to find device '%s'\n", device_name);
        return 1;
    }

    XDevice *device = XOpenDevice(display, info->id);
    if (!device) {
        fprintf(stderr, "unable to open device '%s'\n", device_name);
        return 1;
    }

    XDeviceState *state = XQueryDeviceState(display, device);
    if (!state) {
        return 0;
    }

    XInputClass *input_class = state->data;
    printf("%d class%s :\n", state->num_classes, (state->num_classes > 1) ? "es" : "");
    for (int i = 0; i < state->num_classes; i++) {
        switch (input_class->class) {
            // case ValuatorClass: {
            //     XValuatorState *val_state = (XValuatorState *)input_class;
            //     printf(
            //         "ValuatorClass Mode=%s Proximity=%s\n",
            //         val_state->mode & 1 ? "Absolute" : "Relative",
            //         val_state->mode & 2 ? "Out" : "In"
            //     );
            //     for (int loop2 = 0; loop2 < val_state->num_valuators; loop2++) {
            //         printf("\tvaluator[%d]=%d\n", loop2, val_state->valuators[loop2]);
            //     }
            // } break;

            case ButtonClass: {
                XButtonState *button_state = (XButtonState *)input_class;
                printf("ButtonClass\n");
                printf("Button count is: %d", button_state->num_buttons);
                for (int j = 1; j <= button_state->num_buttons; j++) {
                    int button = button_state->buttons[j];
                    printf("Button %i = %i\n", button);
                    // printf(
                    //     "\tbutton[%d]=%s\n", j,
                    //     (button_state->buttons[j / 8] & (1 << (j % 8))) ? "down" : "up"
                    // );
                }
            } break;

            // case KeyClass: {
            //     XKeyState *key_state = (XKeyState *)input_class;
            //     printf("KeyClass\n");
            //     for (int loop2 = 0; loop2 < key_state->num_keys; loop2++) {
            //         printf(
            //             "\tkey[%d]=%s\n", loop2,
            //             (key_state->keys[loop2 / 8] & (1 << (loop2 % 8))) ? "down" : "up"
            //         );
            //     }
            // } break;
        }
        input_class = (XInputClass *)((char *)input_class + input_class->length);
    }

    XFreeDeviceState(state);
    return 0;
}
