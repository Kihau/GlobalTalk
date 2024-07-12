#include "os/input.h"

#include <cstdlib>

struct Input {
    HHOOK input_hook;
    Button last_button;
};

static LRESULT input_hook_callback(int nCode, WPARAM wParam, LPARAM lParam)
{
    not_implemented();
    return false;
    // BOOL fEatKeystroke = FALSE;
    //
    // if (nCode == HC_ACTION)
    // {
    //     switch (wParam)
    //     {
    //         case WM_KEYDOWN:
    //         case WM_SYSKEYDOWN:
    //         case WM_KEYUP:
    //         case WM_SYSKEYUP:
    //             PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
    //             if (fEatKeystroke = (p->vkCode == 0x41))  //redirect a to b
    //             {     
    //                 printf("Hello a\n");
    //
    //                 if ( (wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN) ) // Keydown
    //                 {
    //                     keybd_event('B', 0, 0, 0);
    //                 }
    //                 else if ( (wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP) ) // Keyup
    //                 {
    //                     keybd_event('B', 0, KEYEVENTF_KEYUP, 0);
    //                 }
    //                 break;
    //             }
    //             break;
    //     }
    // }
    // return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}


Input* initialize_input(const char *device_name) {
    Input *input = (Input *)malloc(sizeof(Input));
    not_implemented();
    return input;
}

Input* initialize_input() {
    Input *input = (Input *)malloc(sizeof(Input));

    input->input_hook = [&](int code, WPARAM sent_by_self, LPARAM message_details) {
        // Here goes the hook logic
    };

    // HHOOK hook_result = SetWindowsHookEx(WH_KEYBOARD_LL | WH_MOUSE_LL, input->input_hook, NULL, 0);
    HHOOK hook_result = SetWindowsHookEx(WH_KEYBOARD | WH_MOUSE, input->input_hook, NULL, 0);
    return input;
}

void destroy_input(Input *input) {
    UnhookWindowsHookEx(input->input_hook);
}

bool get_next_button(Input *input, Button *button) {
    Msg message;
    GetMessage(&message, NULL, NULL, NULL);
    TranslateMessage(&message);
    DispatchMessage(&message);

    *button = input->button;

    return true;

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
