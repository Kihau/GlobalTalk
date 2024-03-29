#include <thread>

#include "utils.h"
#include "os/audio.h"
#include "os/input.h"
#include "gui/config_menu.h"
#include "../res/unmute.h"

i32 run_global_talk() {
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
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        i32 result = run_global_talk();
        return result;
    }

    char *program_name = argv[0];
    char *argument     = argv[1];
    if (strcmp(argument, "--help") == 0) {
        printf("Global Talk - Cross platform global push-to-talk and push-to-mute.\n");
        printf("\n");
        printf("Usage:\n");
        printf("    %s        - Start listening for input events.\n", program_name);
        printf("    %s --menu - Open GUI configuration menu.\n", program_name);
        printf("    %s --help - Print help.\n", program_name);
        return 0;
    } else if (strcmp(argument, "--menu") == 0) {
        int result = run_config_menu();
        return result;
    } else {
        log_error("\'%s\' is not a valid argument. Use --help for usage.", argument);
        return 1;
    }

    return 0;
}
