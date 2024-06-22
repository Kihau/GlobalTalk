#ifndef INPUT_LINUX_INTERNAL_H
#define INPUT_LINUX_INTERNAL_H

#include "os/audio.h"

Audio* pulseaudio_initialize_audio();
void pulseaudio_destroy_audio(Audio *audio);
bool pulseaudio_mute_microphone(Audio *audio);
bool pulseaudio_unmute_microphone(Audio *audio);
bool pulseaudio_is_microphone_muted(Audio *audio);
bool pulseaudio_play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);
bool pulseaudio_play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);

#endif // INPUT_LINUX_INTERNAL_H
