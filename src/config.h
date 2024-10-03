#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <cstring>
#include <vector>

#include "core/input.h"

enum class Audio_Action {
    MUTE,
    UNMUTE,
    TOGGLE,
};

enum class Stream_Type {
    INPUT_DEVICE,
    OUTPUT_DEVICE,
    PLAYBACK_STREAM,
    CAPTURE_STREAM,
};

struct Audio_Stream {
    bool has_audio_stream;

    Stream_Type type;
    const char *name;
};

struct Audio_Bind {
    Button button;
    Audio_Action action;
    Audio_Stream stream;
};

struct Config {
    std::vector<Audio_Bind> binds;
};

// const char* translate_stream_type(Stream_Type type);
void save_config(Config config);
Config load_config();

#endif // CONFIG_H
