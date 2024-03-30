#include "os/input.h"


bool initialize_input(Input *input, const char *device_name) {
    not_implemented();
}

bool initialize_input(Input *input) {
    not_implemented();
}

void destroy_input(Input input) {
    not_implemented();
}

bool get_next_button(Input input, Button *button) {
    not_implemented();

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

    return false;
}
 
bool query_button_state(Input input, Button *button) {
    not_implemented();
}
