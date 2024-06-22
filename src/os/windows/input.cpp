#include "os/input.h"

#include <cstdlib>

struct Input {
    int dummy;
};

Input* initialize_input(const char *device_name) {
    Input *input = (Input *)malloc(sizeof(Input));
    not_implemented();
    return input;
}

Input* initialize_input() {
    Input *input = (Input *)malloc(sizeof(Input));
    not_implemented();
    return input;
}

void destroy_input(Input *input) {
    not_implemented();
}

bool get_next_button(Input *input, Button *button) {
    not_implemented();
    return false;

    // if ((GetKeyState(VK_XBUTTON1) & 0x80) != 0) {
    //     result = endpoint_volume->SetMute(TRUE, NULL);
    //     if (FAILED(result)) {
    //         printf("Failed to mute microphone\n");
    //     } else {
    //         printf("Microphone disabled successfully\n");
    //     }
    //     printf("Mouse 4 pressed\n");
    //     Sleep(300);
    // }
    //
    // else if ((GetKeyState(VK_XBUTTON2) & 0x80) != 0) {
    //     result = endpoint_volume->SetMute(FALSE, NULL);
    //     if (FAILED(result)) {
    //         printf("Failed to unmute microphone\n");
    //     } else {
    //         printf("Microphone enabled successfully\n");
    //     }
    //     printf("Mouse 5 pressed\n");
    //     Sleep(300);
    // } else if (GetAsyncKeyState(VK_ESCAPE)) {
    //     endpoint_volume->Release();
    //     default_device->Release();
    //     enumerator->Release();
    //     CoUninitialize();
    //     return 0;
    // }
}

bool query_button_state(Input *input, Button *button) {
    not_implemented();
    return false;
}
