#include "pulseaudio.h"

#define lock_scope(mainloop, code)         \
    pa_threaded_mainloop_lock(mainloop);   \
    code;                                  \
    pa_threaded_mainloop_unlock(mainloop); \

namespace pulseaudio {

std::vector<Entry_Data> get_playback_streams(Audio *audio) {
    struct Callback_Data {
        Audio *audio;
        bool end_of_list;
        Entry_Data return_data;
    };

    auto info_callback = [](pa_context *context, const pa_sink_input_info *info, int eol, void *userdata) {
        mark_unused(context);
        mark_unused(eol);

        auto data = (Callback_Data *)userdata;

        if (info == nullptr) {
            data->end_of_list = true;
            pa_threaded_mainloop_signal(data->audio->pulse, 1);
            return;
        }

        // TODO: Copy both info->name and app_name into separate buffer.

        const char *app_name = pa_proplist_gets(info->proplist, "application.name");
        char *display_name = NULL;
        if (app_name != NULL) {
            size_t needed = snprintf(NULL, 0, "%s - %s", app_name, info->name) + 1;
            display_name  = (char *)malloc(needed);
            sprintf(display_name, "%s - %s", app_name, info->name);
        } else {
            size_t needed = snprintf(NULL, 0, "%s", info->name) + 1;
            display_name  = (char *)malloc(needed);
            sprintf(display_name, "%s", info->name);
        }

        Entry_Data entry;
        entry.index = info->index;
        entry.name  = info->name;
        entry.display_name = display_name;

        data->end_of_list = false;
        data->return_data = entry;

        pa_threaded_mainloop_signal(data->audio->pulse, 1);
    };

    Callback_Data data = { .audio = audio };
    auto op = pa_context_get_sink_input_info_list(audio->context, info_callback, &data);	
    assert(op);

    std::vector<Entry_Data> playback_streams;

    lock_scope(audio->pulse, {
        while (true) {
            pa_threaded_mainloop_wait(audio->pulse);

            if (data.end_of_list) {
                break;
            }

            playback_streams.push_back(data.return_data);
            pa_threaded_mainloop_accept(audio->pulse);
        }
        pa_operation_cancel(op);
        pa_threaded_mainloop_accept(audio->pulse);
    })

    return playback_streams;
}

std::vector<Entry_Data> get_capture_streams(Audio *audio) {
    struct Callback_Data {
        Audio *audio;
        bool end_of_list;
        Entry_Data return_data;
    };

    auto info_callback = [](pa_context *context, const pa_source_output_info *info, int eol, void *userdata) {
        mark_unused(context);
        mark_unused(eol);

        auto data = (Callback_Data *)userdata;

        if (info == nullptr) {
            data->end_of_list = true;
            pa_threaded_mainloop_signal(data->audio->pulse, 1);
            return;
        }

        // TODO: Copy both info->name and app_name into separate buffer.

        const char *app_name = pa_proplist_gets(info->proplist, "application.name");
        char *display_name = NULL;
        if (app_name != NULL) {
            size_t needed = snprintf(NULL, 0, "%s - %s", app_name, info->name) + 1;
            display_name  = (char *)malloc(needed);
            sprintf(display_name, "%s - %s", app_name, info->name);
        } else {
            size_t needed = snprintf(NULL, 0, "%s", info->name) + 1;
            display_name  = (char *)malloc(needed);
            sprintf(display_name, "%s", info->name);
        }

        Entry_Data entry;
        entry.index = info->index;
        entry.name  = info->name;
        entry.display_name = display_name;

        data->end_of_list = false;
        data->return_data = entry;

        pa_threaded_mainloop_signal(data->audio->pulse, 1);
    };

    Callback_Data data = { .audio = audio };
    auto op = pa_context_get_source_output_info_list(audio->context, info_callback, &data);	
    assert(op);

    std::vector<Entry_Data> playback_streams;

    lock_scope(audio->pulse, {
        while (true) {
            pa_threaded_mainloop_wait(audio->pulse);

            if (data.end_of_list) {
                break;
            }

            playback_streams.push_back(data.return_data);
            pa_threaded_mainloop_accept(audio->pulse);
        }
        pa_operation_cancel(op);
        pa_threaded_mainloop_accept(audio->pulse);
    })

    return playback_streams;
}

std::vector<Entry_Data> get_input_devices(Audio *audio) {
    struct Callback_Data {
        Audio *audio;
        bool end_of_list;
        Entry_Data return_data;
    };

    auto info_callback = [](pa_context *context, const pa_source_info *info, int eol, void *userdata) {
        mark_unused(context);
        mark_unused(eol);

        auto data = (Callback_Data *)userdata;

        if (info == nullptr) {
            data->end_of_list = true;
            pa_threaded_mainloop_signal(data->audio->pulse, 1);
            return;
        }

        // TODO: Copy both info->name and app_name into separate buffer.

        size_t needed = snprintf(NULL, 0, "%s", info->description) + 1;
        char *display_name = (char *)malloc(needed);
        sprintf(display_name, "%s", info->description);

        Entry_Data entry;
        entry.index = info->index;
        entry.name  = info->name;
        entry.display_name = display_name;

        data->end_of_list = false;
        data->return_data = entry;

        pa_threaded_mainloop_signal(data->audio->pulse, 1);
    };

    Callback_Data data = { .audio = audio };
    auto op = pa_context_get_source_info_list(audio->context, info_callback, &data);	
    assert(op);

    std::vector<Entry_Data> playback_streams;

    lock_scope(audio->pulse, {
        while (true) {
            pa_threaded_mainloop_wait(audio->pulse);

            if (data.end_of_list) {
                pa_threaded_mainloop_accept(audio->pulse);
                break;
            }

            playback_streams.push_back(data.return_data);
            pa_threaded_mainloop_accept(audio->pulse);
        }
        pa_operation_cancel(op);
    })

    return playback_streams;
}

std::vector<Entry_Data> get_output_devices(Audio *audio) {
    struct Callback_Data {
        Audio *audio;
        bool end_of_list;
        Entry_Data return_data;
    };

    auto info_callback = [](pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
        mark_unused(context);
        mark_unused(eol);

        auto data = (Callback_Data *)userdata;

        if (info == nullptr) {
            data->end_of_list = true;
            pa_threaded_mainloop_signal(data->audio->pulse, 1);
            return;
        }

        // TODO: Copy both info->name and app_name into separate buffer.

        size_t needed = snprintf(NULL, 0, "%s", info->description) + 1;
        char *display_name = (char *)malloc(needed);
        sprintf(display_name, "%s", info->description);

        Entry_Data entry;
        entry.index = info->index;
        entry.name  = info->name;
        entry.display_name = display_name;

        data->end_of_list = false;
        data->return_data = entry;

        pa_threaded_mainloop_signal(data->audio->pulse, 1);
    };

    Callback_Data data = { .audio = audio };
    auto op = pa_context_get_sink_info_list(audio->context, info_callback, &data);	
    assert(op);

    std::vector<Entry_Data> playback_streams;

    lock_scope(audio->pulse, {
        while (true) {
            pa_threaded_mainloop_wait(audio->pulse);

            if (data.end_of_list) {
                pa_threaded_mainloop_accept(audio->pulse);
                break;
            }

            playback_streams.push_back(data.return_data);
            pa_threaded_mainloop_accept(audio->pulse);
        }
        pa_operation_cancel(op);
    })

    return playback_streams;
}

static Audio_Entry get_entry_from_stream(Audio *audio, Audio_Stream stream) {
    Audio_Entry entry;

    if (!stream.has_audio_stream) {
        entry.type = Entry_Type::NONE;
        return entry;
    }

    std::vector<Entry_Data> entries_data;
    switch (stream.type) {
        case Stream_Type::INPUT_DEVICE: {
            entries_data = get_input_devices(audio);
            entry.type = Entry_Type::INPUT_DEVICE;
        } break;

        case Stream_Type::OUTPUT_DEVICE: {
            entries_data = get_output_devices(audio);
            entry.type = Entry_Type::OUTPUT_DEVICE;
        } break;

        case Stream_Type::PLAYBACK_STREAM: {
            entries_data = get_playback_streams(audio);
            entry.type = Entry_Type::PLAYBACK_STREAM;
        } break;

        case Stream_Type::CAPTURE_STREAM: {
            entries_data = get_capture_streams(audio);
            entry.type = Entry_Type::CAPTURE_STREAM;
        } break;
    }

    for (auto &data : entries_data) {
        if (strcmp(stream.name, data.display_name) == 0) {
            entry.data = data;
            return entry;
        }
    }

    entry.type = Entry_Type::NONE;
    return entry;
}

Audio* initialize_audio() {
    auto pulse = pa_threaded_mainloop_new();
    pa_threaded_mainloop_start(pulse);

    auto api = pa_threaded_mainloop_get_api(pulse);
    auto context = pa_context_new(api, "GlobalTalk");
    if (context == NULL) {
        return NULL;
    }

    auto connection_callback = [](pa_context *context, void *userdata) {
        // NOTE: This callback somehow needs to be removed after establishing the connection.
        //       Things might break if the PA_CONTEXT_READY passes through again after the initialization.
        auto pulse = (pa_threaded_mainloop *)userdata;
        if (pa_context_get_state(context) == PA_CONTEXT_READY) {
            pa_threaded_mainloop_signal(pulse, 1);
        }
    };

    pa_context_set_state_callback(context, connection_callback, pulse);
    lock_scope(pulse, {
        pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, NULL);
        pa_threaded_mainloop_wait(pulse);
        pa_threaded_mainloop_accept(pulse);
    })
    log_debug("Context connection established.");

    static const pa_sample_spec spec = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    int error;
    pa_simple *stream = pa_simple_new(
        NULL, "GlobalTalk" , PA_STREAM_PLAYBACK, NULL, "Audio Feedback", &spec, NULL, NULL, &error
    );

    auto audio = (Audio *)malloc(sizeof(Audio));

    audio->pulse   = pulse;
    audio->api     = api;
    audio->context = context;
    audio->stream  = stream;

    return audio;
}

void destroy_audio(Audio *audio) {
    if (audio == NULL) {
        return;
    }

    pa_context_disconnect(audio->context);
    pa_context_unref(audio->context);
    pa_threaded_mainloop_free(audio->pulse);
    free(audio);
}

bool set_audio_state(Audio *audio, Audio_Stream stream, bool should_mute) {
    auto context = audio->context;
    auto entry = get_entry_from_stream(audio, stream);
    auto index = entry.data.index;

    switch (entry.type) {
        case Entry_Type::NONE: {
            log_warning("Audio entry not found. Open configuration menu and select target entry.");
            return false;
        } break;

        case Entry_Type::PLAYBACK_STREAM: {
            pa_context_set_sink_input_mute(context, index, should_mute, NULL, NULL);
        } break;

        case Entry_Type::CAPTURE_STREAM: {
            pa_context_set_source_output_mute(context, index, should_mute, NULL, NULL);
        } break;

        case Entry_Type::OUTPUT_DEVICE: {
            pa_context_set_sink_mute_by_index(context, index, should_mute, NULL, NULL);
        } break;

        case Entry_Type::INPUT_DEVICE: {
            pa_context_set_source_mute_by_index(context, index, should_mute, NULL, NULL);
        } break;
    }

    return true;
}

bool mute_microphone(Audio *audio, Audio_Stream stream) {
    return set_audio_state(audio, stream, true);
}

bool unmute_microphone(Audio *audio, Audio_Stream stream) {
    return set_audio_state(audio, stream, false);
}

bool is_microphone_muted(Audio *audio, Audio_Stream stream) {
    auto context = audio->context;
    auto entry = get_entry_from_stream(audio, stream);
    auto index = entry.data.index;

    pa_operation *op = NULL;
    struct Callback_Data {
        Audio *audio;
        bool is_muted;
    };

    Callback_Data data = {
        .audio = audio,
        .is_muted = false,
    };

    auto info_callback = [](pa_context *context, const auto *info, int eol, void *userdata) {
        auto data = (Callback_Data *)userdata;
        if (info != NULL) data->is_muted = info->mute;
        pa_threaded_mainloop_signal(data->audio->pulse, 1);
    };

    switch (entry.type) {
        case Entry_Type::NONE: {
            log_warning("Audio entry not found. Open configuration menu and select target entry.");
            return false;
        } break;

        // NOTE: What if the index changes?
        case Entry_Type::PLAYBACK_STREAM: {
            op = pa_context_get_sink_input_info(context, index, info_callback, &data);
        } break;

        case Entry_Type::CAPTURE_STREAM: {
            op = pa_context_get_source_output_info(context, index, info_callback, &data);
        } break;

        case Entry_Type::OUTPUT_DEVICE: {
            op = pa_context_get_sink_info_by_index(context, index, info_callback, &data);
        } break;

        case Entry_Type::INPUT_DEVICE: {
            op = pa_context_get_source_info_by_index(context, index, info_callback, &data);
        } break;
    }

    if (op == NULL) {
        return false;
    }

    lock_scope(audio->pulse, {
        pa_threaded_mainloop_wait(audio->pulse);
        pa_operation_cancel(op);
        pa_threaded_mainloop_accept(audio->pulse);
    });

    return data.is_muted;
}

// TODO: Make this a structure    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv 
bool play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    int error = 0;

    pa_simple_flush(audio->stream, &error);
    if (error != 0) {
        log_error("Failed to flush audio stream: %i", error);
        return false;
    }

    pa_simple_write(audio->stream, sound_buffer, buffer_size, &error);
    if (error != 0) {
        log_error("Failed to write to audio stream: %i", error);
        return false;
    }

    pa_simple_drain(audio->stream, &error);
    if (error != 0) {
        log_error("Failed to drain audio stream: %i", error);
        return false;
    }

    return true;
}

bool play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size) {
    mark_unused(audio);
    mark_unused(sound_buffer);
    mark_unused(buffer_size);
    not_implemented();
}

} // namespace pulseaudio
