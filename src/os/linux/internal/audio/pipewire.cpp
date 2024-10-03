#include "core/audio.h"

// dynamic load this, throw error during runtime when dyn load fails?
// #include <pipewire/pipewire.h>

Audio* pipewire_initialize_audio() {
    not_implemented();
}

void pipewire_destroy_audio(Audio *audio) {
    mark_unused(audio);
    not_implemented();
}

bool pipewire_mute_microphone(Audio *audio) {
    mark_unused(audio);
    not_implemented();
}

bool pipewire_unmute_microphone(Audio *audio) {
    mark_unused(audio);
    not_implemented();
}

bool pipewire_is_microphone_muted(Audio *audio) {
    mark_unused(audio);
    not_implemented();
}

bool pipewire_play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    mark_unused(audio);
    mark_unused(sound_buffer);
    mark_unused(buffer_size);
    not_implemented();
}

bool pipewire_play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    mark_unused(audio);
    mark_unused(sound_buffer);
    mark_unused(buffer_size);
    not_implemented();
}
