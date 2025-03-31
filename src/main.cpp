#include "utils.h"
#include "core/audio.h"
#include "core/input.h"
#include "gui/config_menu.h"

#include "../res/unmute.h"
#include "../res/mute.h"

i32 run_global_talk() {
    // Pass config to input and audio?
    Config config = load_config();

    Input *input = initialize_input();
    defer { destroy_input(input); };
    if (input == NULL) {
        return 1;
    }
    log_info("Input successfully initialized.");

    Audio *audio = initialize_audio();
    defer { destroy_audio(audio); };
    if (audio == NULL) {
        return 1;
    }
    log_info("Audio successfully initialized.");

    while (true) {
        Button button;
        bool success = get_next_button(input, &button);
        if (!success) {
            log_error("Getting next button failed.");
            continue;
        }

        for (auto &bind : config.binds) {
            if (bind.button.type != button.type) {
                continue;
            }

            if (bind.button.state != button.state) {
                continue;
            }

            switch (bind.action) {
                case Audio_Action::MUTE: {
                    log_info("Muting audio source: %s", bind.stream.name);
                    bool success = mute_microphone(audio, bind.stream);
                    if (success) play_raw_sound(audio, mute_raw, mute_raw_len);
                } break;

                case Audio_Action::UNMUTE: {
                    log_info("Unmuting audio source: %s", bind.stream.name);
                    bool success = unmute_microphone(audio, bind.stream);
                    if (success) play_raw_sound(audio, unmute_raw, unmute_raw_len);
                } break;

                case Audio_Action::TOGGLE: {
                    log_info("Togging audio source: %s", bind.stream.name);
                    if (is_microphone_muted(audio, bind.stream)) {
                        bool success = unmute_microphone(audio, bind.stream);
                        if (success) play_raw_sound(audio, unmute_raw, unmute_raw_len);
                    } else {
                        bool success = mute_microphone(audio, bind.stream);
                        if (success) play_raw_sound(audio, mute_raw, mute_raw_len);
                    }
                } break;
            }
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
