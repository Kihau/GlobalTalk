#include "os/audio.h"
#include "os/input.h"
#include "utils.h"

int main(int argc, char **argv) {
    mark_unused(argc);
    mark_unused(argv);

    Input input = {};
    bool success = initialize_input(&input);
    if (!success) {
        return 1;
    }
    defer { destroy_input(input); };
    log_info("Input successfully initialized.");

    Audio audio = {};
    success = initialize_audio(&audio);
    if (!success) {
        return 1;
    }
    defer { destroy_audio(audio); };
    log_info("Audio successfully initialized.");

    while (true) {
        Button button;
        success = get_next_button(input, &button);
        if (!success || button.type != ButtonType::MOUSE_4) {
            continue;
        }

        switch (button.state) {
            case ButtonState::BUTTON_PRESS: {
                if (is_microphone_muted(audio)) {
                    unmute_microphone(audio);
                    log_info("Microphone unmuted.");
                }
            } break;

            case ButtonState::BUTTON_RELEASE: {
                if (!is_microphone_muted(audio)) {
                    mute_microphone(audio);
                    log_info("Microphone muted.");
                }
            } break;
        }
    }

    return 0;
}
