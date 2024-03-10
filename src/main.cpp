#include "os/audio.h"
#include "os/input.h"
#include "utils.h"

int main(int argc, char **argv) {
    Input input;
    bool success = initialize_input(&input);
    if (!success) {
        return 1;
    }
    defer { destroy_input(input); };

    Audio audio;
    success = initialize_audio(&audio);
    if (!success) {
        return 1;
    }
    defer { destroy_audio(audio); };

    while (true) {
        Button button;
        success = get_next_button(input, &button);
        if (!success || button.type != ButtonType::MOUSE_4) {
            continue;
        }

        switch (button.state) {
            case ButtonState::BUTTON_PRESS: {
                unmute_microphone(audio);
            } break;

            case ButtonState::BUTTON_RELEASE: {
                mute_microphone(audio);
            } break;
        }
    }

    return 0;
}
