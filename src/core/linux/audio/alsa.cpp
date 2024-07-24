#include "alsa.h"

namespace alsa {

static snd_mixer_elem_t *get_mixer_capture(Audio audio);
static bool open_playback_device(Audio *audio);
static bool open_alsa_mixer(Audio *audio);

/// Alsa initialization stuff.
Audio* initialize_audio() {
    Audio temp;
    bool success = open_alsa_mixer(&temp);
    if (!success) {
        return NULL;
    }

    success = open_playback_device(&temp);
    if (!success) {
        return NULL;
    }

    Audio *audio = (Audio *)malloc(sizeof(Audio));
    *audio = temp;
    return audio;
}

void destroy_audio(Audio *audio) {
    if (audio == NULL) {
        return;
    }

    snd_mixer_close(audio->mixer);
    audio->mixer = NULL;
    free(audio);
}


bool mute_microphone(Audio *audio) {
    snd_mixer_handle_events(audio->mixer); 

    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        auto channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(audio->capture, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(audio->capture, channel, 0);
        if (result != 0) {
            return false;
        }
    }

    return true;
}

bool unmute_microphone(Audio *audio) {
    snd_mixer_handle_events(audio->mixer); 

    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        auto channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(audio->capture, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(audio->capture, channel, 1);
        if (result != 0) {
            return false;
        }
    }

    return true;
}

bool is_microphone_muted(Audio *audio) {
    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        auto channel = (snd_mixer_selem_channel_id_t)i;
        if (!snd_mixer_selem_has_capture_channel(audio->capture, channel)) {
            continue;
        }

        int is_unmuted = false;
        snd_mixer_selem_get_capture_switch(audio->capture, channel, &is_unmuted);
        if (is_unmuted) {
            return false;
        }
    }

    return true;
}

bool play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    snd_pcm_prepare(audio->playback);
    snd_pcm_writei(audio->playback, sound_buffer, buffer_size / (2 * 2));
    snd_pcm_drain(audio->playback);
    return true;
}

bool play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    mark_unused(audio);
    mark_unused(sound_buffer);
    mark_unused(buffer_size);
    not_implemented();
}

static bool open_alsa_mixer(Audio *audio) {
    snd_mixer_t *handle = NULL;
    const char *card = "default";

    int result = snd_mixer_open(&handle, 0); 
    if (result < 0) {
        log_error("Mixer %s open error: %s.", card, snd_strerror(result));
        return false;
    }

    bool had_error = true;
    defer { if (had_error) snd_mixer_close(handle); };

    result = snd_mixer_attach(handle, card);
    if (result < 0) {
        log_error("Mixer attach %s error: %s.", card, snd_strerror(result));
        return false;
    }

    result = snd_mixer_selem_register(handle, NULL, NULL);
    if (result < 0) {
        log_error("Mixer register error: %s.", snd_strerror(result));
        return false;
    }

    result = snd_mixer_load(handle);
    if (result < 0) {
        log_error("Mixer %s load error: %s.", card, snd_strerror(result));
        return false;
    }

    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Capture");

    snd_mixer_elem_t *element = snd_mixer_find_selem(handle, sid);
    if (element == NULL) {
        const char *name = snd_mixer_selem_id_get_name(sid);
        int index = snd_mixer_selem_id_get_index(sid);
        log_error("Unable to find simple control '%s',%i.", name, index);
        return false;
    }


    audio->mixer   = handle;
    audio->capture = element;

    had_error = false;
    return true;
}

static bool open_playback_device(Audio *audio) {
    snd_pcm_t *playback = NULL;
    const char *card = "default";

    int result = snd_pcm_open(&playback, card, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (result < 0) {
        log_error("Can't open \"%s\" PCM device. %s", card, snd_strerror(result));
        return false;
    } 

    // snd_pcm_hw_params_t *params = NULL;
    // snd_pcm_hw_params_alloca(&params);
    // snd_pcm_hw_params_any(playback, params);
    // snd_pcm_hw_params_set_access(playback, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // snd_pcm_hw_params_set_format(playback, params, SND_PCM_FORMAT_S16_LE); 
    // snd_pcm_hw_params_set_channels(playback, params, 2); 
    // u32 rate = 44100;
    // snd_pcm_hw_params_set_rate_near(playback, params, &rate, 0); 
    //
    // result = snd_pcm_hw_params(playback, params);
    // if (result < 0) {
    //     log_error("Can't set harware parameters. %s", snd_strerror(result));
    //     return false;
    // }

    snd_pcm_set_params(playback, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, 44100, 1, 500000);
    // int test_buffer_size = 60816;
    // snd_pcm_hw_params_set_period_size_near(playback, params, &test_buffer_size, 0);

    // snd_pcm_prepare(playback);
    // snd_pcm_writei(playback, unmute_raw, unmute_raw_len / (2 * 2));
    // snd_pcm_drain(playback);

    // snd_pcm_uframes_t frame_size = 32;
    // snd_pcm_hw_params_set_period_size_near(playback, params, &frame_size, 0);

    // snd_pcm_hw_params_get_buffer_size(params, &frame_size);
    // log_info("%li", frame_size);

    // int samples = unmute_raw_len / frame_size;
    // log_info("%i", samples);


    audio->playback = playback;

    return true;
}

static snd_mixer_elem_t *get_mixer_capture(Audio audio) {
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Capture");

    snd_mixer_elem_t *element = snd_mixer_find_selem(audio.mixer, sid);
    if (element == NULL) {
        const char *name = snd_mixer_selem_id_get_name(sid);
        int index = snd_mixer_selem_id_get_index(sid);
        log_error("Unable to find simple control '%s',%i.", name, index);
        return NULL;
    }

    return element;
}

}
