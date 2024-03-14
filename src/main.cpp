#include <thread>

#include "os/audio.h"
#include "os/input.h"
#include "utils.h"
#include "../res/unmute.h"

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

        switch (button.state) {
            case ButtonState::BUTTON_PRESS: {
                std::thread alsa_thread(play_raw_sound, audio, unmute_raw, unmute_raw_len);
                alsa_thread.detach();

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
