#ifndef AUDIO_H
#define AUDIO_H

#include "../utils.h"

#ifdef __linux__ 
    #include <alsa/asoundlib.h>

    struct Audio {
        snd_mixer_t *handle;
        snd_mixer_elem_t *element;
    };
#elif _WIN32
    #include <mmdeviceapi.h>
    #include <endpointvolume.h>

    struct Audio {
        IAudioEndpointVolume *endpoint_volume;
        IMMDevice *default_device;
        IMMDeviceEnumerator *enumerator;
    };
#endif

/// Initialized system audio.
bool initialize_audio(Audio *audio);

/// Frees audio related resource.
void destroy_audio(Audio audio);

/// Mutes the microphone.
bool mute_microphone(Audio audio);

/// Unmutes the microphone.
bool unmute_microphone(Audio audio);

/// Check if microphone is muted. 
/// If any audio capture channel is umuted, the function returns false.
bool is_microphone_muted(Audio audio);

/// Play raw audio. The sound format must be:
///     - signed 16bit little endian,
///     - 44100Hz,
///     - stereo.
bool play_raw_sound(Audio audio, char *sound_buffer, size_t buffer_size);

/// Play sound audio. The sound format is wave (or wav).
bool play_wav_sound(Audio audio, char *sound_buffer, size_t buffer_size);

#endif // AUDIO_H
