#include "os/input.h"

#include <windows.h>
#include <cstdlib>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

std::queue<Button> button_queue;
std::mutex queue_mutex;
std::condition_variable queue_notifier;

struct Input {
    HHOOK keyboard_hook;
};

LRESULT keyboard_hook_callback(int code, WPARAM key_state, LPARAM key_data) {
    auto key = (LPKBDLLHOOKSTRUCT) key_data;
    Button button;

    switch (key->vkCode) {
        case 0x2D: {
            button.type = ButtonType::INSERT;
        } break;

        default: {
            button.type = ButtonType::OTHER;
        } break;
    }

    switch (key_state) {
        case WM_KEYDOWN: {
            button.state = ButtonState::BUTTON_PRESS;
        } break;

        case WM_KEYUP: {
            button.state = ButtonState::BUTTON_RELEASE;
        } break;

        case WM_SYSKEYDOWN: {
            button.state = ButtonState::BUTTON_PRESS;
        } break;

        case WM_SYSKEYUP: {
            button.state = ButtonState::BUTTON_RELEASE;
        } break;

        default: {
            assert(false && "Unreachable");
        } break;
    }

    std::lock_guard<std::mutex> lock(queue_mutex);
    button_queue.push(myButton);
    lock.unlock();
    queue_notifier.notify_one();

    return CallNextHookEx(NULL, code, wParam, lParam);
}

Input *initialize_input(const char *device_name) {
   Input *input = (Input *) malloc(sizeof(Input));
   not_implemented();
   return input;
}

// void message_loop() {
//     MSG msg, qmsg;
//     while (GetMessage(&msg, (HWND) NULL, 0, 0)) {
//         if (!CallMsgFilter(&qmsg, 0)) {
//             DispatchMessage(&qmsg);
//         }
//     }
// }

Input *initialize_input() {
    Input *input = (Input *)malloc(sizeof(Input));
    HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_hook_callback, nullptr, 0);
    input->keyboard_hook = keyboard_hook;

    // NOTE: Does this thread ever exit?
    std::thread message_loop_thread([] {
        MSG msg, qmsg;
        while (GetMessage(&msg, (HWND) NULL, 0, 0)) {
            if (!CallMsgFilter(&qmsg, 0)) {
                DispatchMessage(&qmsg);
            }
        }
    });
    message_loop_thread.detach();

    return input;
}

void destroy_input(Input *input) {
    UnhookWindowsHookEx(input->keyboard_hook);
}

bool get_next_button(Input *input, Button *button) {
    // Is this correct?
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_notifier.wait(lock, [] { return !button_queue.empty(); });

    *button = button_queue.front();
    button_queue.pop();
    lock.unlock();

    return true;
}

bool query_button_state(Input *input, Button *button) {
    not_implemented();

    // TODO(???): 
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
