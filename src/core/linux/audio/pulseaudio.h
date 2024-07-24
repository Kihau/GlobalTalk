#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include "utils.h"

#include <cstring>
#include <vector>
#include <thread>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

namespace pulseaudio {

enum class Entry_Type {
    NONE,
    PLAYBACK_STREAM,
    CAPTURE_STREAM,
    OUTPUT_DEVICE,
    INPUT_DEVICE,
};

struct Entry_Data {
    i32 index;
    const char *name;
    const char *display_name;
};

struct Audio_Entry {
    Entry_Type type;
    Entry_Data data;
};

struct Audio {
    pa_threaded_mainloop *pulse;
    pa_mainloop_api      *api;
    pa_context           *context;
    pa_simple            *stream;
    Audio_Entry           target_entry;
};

Audio* initialize_audio();
void   destroy_audio(Audio *audio);
bool   mute_microphone(Audio *audio);
bool   unmute_microphone(Audio *audio);
bool   is_microphone_muted(Audio *audio);
bool   play_raw_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);
bool   play_wav_sound(Audio *audio, const unsigned char *sound_buffer, size_t buffer_size);

std::vector<Entry_Data> get_playback_streams(Audio *audio);
std::vector<Entry_Data> get_capture_streams(Audio *audio);
std::vector<Entry_Data> get_output_devices(Audio *audio);
std::vector<Entry_Data> get_input_devices(Audio *audio);

} // namespace pulseaudio

#endif // PULSEAUDIO_H

