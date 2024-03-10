#include "audio.h"

//
// TODO: Saves state which is not good and needs to be fixed (You cannot change volume while running the app)
//

/// Alsa initialization stuff.
bool initialize_audio(Audio *audio) {
    snd_mixer_t *handle = NULL;
    const char *card = "default";

    int result = snd_mixer_open(&handle, 0); 
    if (result < 0) {
        fprintf(stderr, "ERROR: Mixer %s open error: %s\n", card, snd_strerror(result));
        return false;
    }

    result = snd_mixer_attach(handle, card);
    if (result < 0) {
        fprintf(stderr, "ERROR: Mixer attach %s error: %s", card, snd_strerror(result));
        return false;
    }

    struct snd_mixer_selem_regopt smixer_options;
    result = snd_mixer_selem_register(handle, NULL, NULL);
    if (result < 0) {
        fprintf(stderr, "ERROR: Mixer register error: %s", snd_strerror(result));
        return false;
    }

    result = snd_mixer_load(handle);
    if (result < 0) {
        fprintf(stderr, "ERROR: Mixer %s load error: %s", card, snd_strerror(result));
        return false;
    }

    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Capture");

    snd_mixer_elem_t *element = snd_mixer_find_selem(handle, sid);
    if (element == NULL) {
        printf(
            "Unable to find simple control '%s',%i\n",
            snd_mixer_selem_id_get_name(sid), 
            snd_mixer_selem_id_get_index(sid)
        );
        return false;
    }

    audio->handle  = handle;
    audio->element = element;

    return true;
}

void destroy_audio(Audio audio) {
    snd_mixer_close(audio.handle);
}

bool mute_microphone(Audio audio) {
    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        snd_mixer_selem_channel_id_t channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(audio.element, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(audio.element, channel, 0);
        if (result >= 0) {
            return false;
        }
    }

    return true;
}

bool unmute_microphone(Audio audio) {
    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        snd_mixer_selem_channel_id_t channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(audio.element, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(audio.element, channel, 1);
        if (result >= 0) {
            return false;
        }
    }

    return true;
}
