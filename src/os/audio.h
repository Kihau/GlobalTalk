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


bool initialize_audio(Audio *audio);
void destroy_audio(Audio audio);
bool mute_microphone(Audio audio);
bool unmute_microphone(Audio audio);

#endif // AUDIO_H
