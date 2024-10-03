#include "core/audio.h"
#include "os/linux/internal/audio/pulseaudio.h"

// Similar to the comment in input.cpp, this could initialize different audio systems if needed.

Audio* initialize_audio() {
    return (Audio *)pulseaudio::initialize_audio();
}

void destroy_audio(Audio *audio) {
    pulseaudio::destroy_audio((pulseaudio::Audio *)audio);
}

bool mute_microphone(Audio *audio, Audio_Stream stream) {
    return pulseaudio::mute_microphone((pulseaudio::Audio *)audio, stream);
}

bool unmute_microphone(Audio *audio, Audio_Stream stream) {
    return pulseaudio::unmute_microphone((pulseaudio::Audio *)audio, stream);
}

bool is_microphone_muted(Audio *audio, Audio_Stream stream) {
    return pulseaudio::is_microphone_muted((pulseaudio::Audio *)audio, stream);
}

bool play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    return pulseaudio::play_raw_sound((pulseaudio::Audio *)audio, sound_buffer, buffer_size);
}

bool play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    return pulseaudio::play_wav_sound((pulseaudio::Audio *)audio, sound_buffer, buffer_size);
}
