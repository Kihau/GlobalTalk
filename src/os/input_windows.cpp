#include "input.h"

bool initialize_input(Input *input) {
    return false;
}

void destroy_input(Input input) {

}

bool get_next_button(Input input, Button *button) {
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

