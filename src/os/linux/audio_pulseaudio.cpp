#include "os/audio.h"

#include <pulse/simple.h>
#include <dlfcn.h>

// dynamic load this, throw error during runtime when dyn load fails?

Audio* pulseaudio_initialize_audio() {
    void *libpulse = dlopen("libpulse.so", RTLD_NOW);
    if (libpulse == NULL) {
        log_error("Failed to load libpulse.so");
    }
    not_implemented();
}

void pulseaudio_destroy_audio(Audio *audio) {
    not_implemented();
}

bool pulseaudio_mute_microphone(Audio *audio) {
    not_implemented();
}

bool pulseaudio_unmute_microphone(Audio *audio) {
    not_implemented();
}

bool pulseaudio_is_microphone_muted(Audio *audio) {
    not_implemented();
}

bool pulseaudio_play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    not_implemented();
}

bool pulseaudio_play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    not_implemented();
}
