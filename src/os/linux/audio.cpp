#include "os/audio.h"
#include "os/linux/internal.h"

// Similar to the comment in input.cpp, this could initialize different audio systems if needed.

Audio* initialize_audio() {
    pulseaudio_initialize_audio();
    not_implemented();
}

void destroy_audio(Audio *audio) {
    not_implemented();
}

bool mute_microphone(Audio *audio) {
    not_implemented();
}

bool unmute_microphone(Audio *audio) {
    not_implemented();
}

bool is_microphone_muted(Audio *audio) {
    not_implemented();
}

bool play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    not_implemented();
}

bool play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    not_implemented();
}
