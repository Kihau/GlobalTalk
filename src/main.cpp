#include <thread>

#include "os/audio.h"
#include "os/input.h"
#include "utils.h"
#include "../res/unmute.h"

int main(int argc, char **argv) {
    mark_unused(argc);
    mark_unused(argv);

    Input input = {};
    // const char *my_mouse_name = "SteelSeries SteelSeries Rival 100 Gaming Mouse";
    // bool success = initialize_input(&input, my_mouse_name);
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
        if (!success) {
            log_error("Getting next button failed.");
            continue;
        }

        switch (button.type) {
            case INSERT: {
                switch (button.state) {
                    case ButtonState::BUTTON_PRESS: {
                        std::thread alsa_thread(play_raw_sound, audio, unmute_raw, unmute_raw_len);
                        alsa_thread.detach();

                        if (is_microphone_muted(audio)) {
                            unmute_microphone(audio);
                            log_info("Microphone unmuted.");
                        } else {
                            mute_microphone(audio);
                            log_info("Microphone muted.");
                        }

                    } break;

                    default: break;
                }
            } break;

            case MOUSE_4: {
                switch (button.state) {
                    case ButtonState::BUTTON_PRESS: {
                        std::thread alsa_thread(play_raw_sound, audio, unmute_raw, unmute_raw_len);
                        alsa_thread.detach();

                        unmute_microphone(audio);
                        log_info("Microphone unmuted.");
                    } break;

                    case ButtonState::BUTTON_RELEASE: {
                        mute_microphone(audio);
                        log_info("Microphone muted.");
                    } break;

                    default: break;
                }
            } break;

            default: break;
        }

    }

    return 0;
}
