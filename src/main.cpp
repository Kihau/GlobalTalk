#include "os/audio.h"
#include "os/input.h"
#include "utils.h"

void push_to_talk() {

}

int main(int argc, char **argv) {
    mark_unused(argc);
    mark_unused(argv);

    Input input = {};
    const char *my_mouse_name = "SteelSeries SteelSeries Rival 100 Gaming Mouse";
    bool success = initialize_input(&input, my_mouse_name);
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

        // Push to mute
        // switch (button.state) {
        //     case ButtonState::BUTTON_PRESS: {
        //         if (is_microphone_muted(audio)) {
        //             log_info("Microphone unmuted.");
        //             unmute_microphone(audio);
        //         } else {
        //             log_info("Microphone muted.");
        //             mute_microphone(audio);
        //         } 
        //     } break;
        // }

        // Push to talk
        switch (button.state) {
            case ButtonState::BUTTON_PRESS: {
                log_info("Microphone unmuted.");
                unmute_microphone(audio);
            } break;

            case ButtonState::BUTTON_RELEASE: {
                log_info("Microphone muted.");
                mute_microphone(audio);
            } break;
        }
    }

    return 0;
}
