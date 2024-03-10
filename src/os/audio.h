#ifndef AUDIO_H
#define AUDIO_H

#ifdef __linux__ 
    #include <alsa/asoundlib.h>

    struct Audio {
        snd_mixer_t *handle;
        snd_mixer_elem_t *element;
    };
#elif _WIN32
    struct Audio {
        // TODO
    };
#endif


bool initialize_audio(Audio *audio);
void destroy_audio(Audio audio);
bool mute_microphone(Audio audio);
bool unmute_microphone(Audio audio);

#endif // AUDIO_H
