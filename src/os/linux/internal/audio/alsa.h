#ifndef ALSA_H
#define ALSA_H

#include "utils.h"

#include <alsa/asoundlib.h>

namespace alsa {

struct Audio {
    snd_mixer_t *mixer;
    snd_mixer_elem_t *capture;
    snd_pcm_t *playback;
};

Audio* alsa_initialize_audio();
void alsa_destroy_audio(Audio *audio);
bool alsa_mute_microphone(Audio *audio);
bool alsa_unmute_microphone(Audio *audio);
bool alsa_is_microphone_muted(Audio *audio);
bool alsa_play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);
bool alsa_play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);

}

#endif // ALSA_H
