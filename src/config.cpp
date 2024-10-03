#include "config.h"

const char* translate_stream_type(Stream_Type type) {
    switch (type) {
        case Stream_Type::INPUT_DEVICE:    return "INPUT_DEVICE";
        case Stream_Type::OUTPUT_DEVICE:   return "OUTPUT_DEVICE";
        case Stream_Type::PLAYBACK_STREAM: return "PLAYBACK_STREAM";
        case Stream_Type::CAPTURE_STREAM:  return "CAPTURE_STREAM";
    }
    assert(false && "Unreachable code detected");
}

u8 *read_entire_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return {};
    }

    fseek(file, 0, SEEK_END);
    i64 file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8 *buffer = (u8 *)malloc(file_size + 1);

    fread(buffer, 1, file_size, file);
    fclose(file);

    buffer[file_size] = '\0';
    return buffer;
}

void save_config(Config config) {
    FILE *file = fopen("global_talk.cfg", "w");
    if (file == NULL) {
        log_error("Failed to save config to a file. Does the program have required permissions?");
        return;
    }

    for (auto bind : config.binds) {
        fprintf(file, "[bind]\n");
        fprintf(file, "key_type     = %s\n", translate_button_type(bind.button.type));
        fprintf(file, "key_state    = %s\n", translate_button_state(bind.button.state));
        switch (bind.action) {
            case Audio_Action::MUTE: {
                fprintf(file, "audio_action = MUTE\n");
            } break;
            case Audio_Action::UNMUTE: {
                fprintf(file, "audio_action = UNMUTE\n");
            } break;
            case Audio_Action::TOGGLE: {
                fprintf(file, "audio_action = TOGGLE\n");
            } break;
        }

        fprintf(file, "stream_type  = %s\n", translate_stream_type(bind.stream.type));
        fprintf(file, "stream_name  = %s\n", bind.stream.name);
        fprintf(file, "\n");
    }

    log_info("Config was saved to a file");
    fclose(file);
}

Config get_debug_config() {
    Config config;

    Audio_Bind test_bind = {
        .button = {
            .state = BUTTON_PRESS,
            .type  = INSERT,
        },
        .action = Audio_Action::TOGGLE,
        .stream = {
            .has_audio_stream = true,
            .type = Stream_Type::PLAYBACK_STREAM,
            .name = "Firefox - YouTube Music",
        },
    };
    config.binds.push_back(test_bind);
    return config;
}

struct Config_Parser {
    u8 *buffer;
    u64 now;
    // u64 line_number;
    // u64 line_offest;
};

void step_byte(Config_Parser *parser) {
    u8 byte = parser->buffer[parser->now];
    if (byte != '\0') {
        parser->now += 1;
    }
}

#define string_equals(buffer, const_string) \
    strncmp((char*)buffer, const_string, sizeof(const_string) - 1) == 0

#define string_add(buffer, const_string) \
    buffer += sizeof(const_string) - 1;

void skip_to_next_word(Config_Parser *parser) {
    while (true) {
        step_byte(parser);
        u8 byte = parser->buffer[parser->now];
        if (byte == '\0') {
            break;
        }

        if (byte == '=' || byte == ' ' || byte == '\n' || byte == '\r' || byte == '\t') {
            continue;
        }

        break;
    }
}

u64 number_of_bytes_to_end(Config_Parser parser) {
    u64 count = 1;
    while (true) {
        step_byte(&parser);
        u8 byte = parser.buffer[parser.now];
        if (byte == '\0' || byte == '\n') {
            return count;
        }
        count += 1;
    }
}

bool parse_bind(Config_Parser *parser, Audio_Bind *bind) {
    string_add(parser->buffer, "[bind]");

    bool has_button_state = false;
    bool has_button_type  = false;
    bool has_audio_action = false;
    bool has_stream_type  = false;
    bool has_stream_name  = false;

    while (true) {
        u8 byte = parser->buffer[parser->now];
        if (byte == '\0') {
            break;
        }

        if (byte == ' ' || byte == '\n' || byte == '\r' || byte == '\t') {
            step_byte(parser);
            continue;
        } 

        if (byte == '[') {
            break;
        } 

        if (string_equals(&parser->buffer[parser->now], "key_type")) { 
            string_add(parser->buffer, "key_type");
            skip_to_next_word(parser);
            if (string_equals(&parser->buffer[parser->now], "INSERT")) {
                string_add(parser->buffer, "INSERT");
                bind->button.type = INSERT;
            } else if (string_equals(&parser->buffer[parser->now], "MOUSE_4")) {
                string_add(parser->buffer, "MOUSE_4");
                bind->button.type = MOUSE_4;
            } else if (string_equals(&parser->buffer[parser->now], "MOUSE_5")) {
                string_add(parser->buffer, "MOUSE_5");
                bind->button.type = MOUSE_5;
            }
            has_button_type = true;
        } else if (string_equals(&parser->buffer[parser->now], "key_state")) { 
            string_add(parser->buffer, "key_state");
            skip_to_next_word(parser);
            if (string_equals(&parser->buffer[parser->now], "PRESS")) {
                string_add(parser->buffer, "PRESS");
                bind->button.state = BUTTON_PRESS;
            } else if (string_equals(&parser->buffer[parser->now], "RELEASE")) {
                string_add(parser->buffer, "RELEASE");
                bind->button.state = BUTTON_RELEASE;
            }
            has_button_state = true;
        } else if (string_equals(&parser->buffer[parser->now], "audio_action")) { 
            string_add(parser->buffer, "audio_action");
            skip_to_next_word(parser);
            if (string_equals(&parser->buffer[parser->now], "MUTE")) {
                string_add(parser->buffer, "MUTE");
                bind->action = Audio_Action::MUTE;
            } else if (string_equals(&parser->buffer[parser->now], "UNMUTE")) {
                string_add(parser->buffer, "UNMUTE");
                bind->action = Audio_Action::UNMUTE;
            } else if (string_equals(&parser->buffer[parser->now], "TOGGLE")) {
                string_add(parser->buffer, "TOGGLE");
                bind->action = Audio_Action::TOGGLE;
            } 
            has_audio_action = true;
        } else if (string_equals(&parser->buffer[parser->now], "stream_type")) { 
            string_add(parser->buffer, "stream_type");
            skip_to_next_word(parser);

            if (string_equals(&parser->buffer[parser->now], "INPUT_DEVICE")) {
                string_add(parser->buffer, "INPUTE_DEVICE");
                bind->stream.type = Stream_Type::INPUT_DEVICE;
            } else if (string_equals(&parser->buffer[parser->now], "OUTPUT_DEVICE")) {
                string_add(parser->buffer, "OUTPUT_DEVICE");
                bind->stream.type = Stream_Type::OUTPUT_DEVICE;
            } else if (string_equals(&parser->buffer[parser->now], "PLAYBACK_STREAM")) {
                string_add(parser->buffer, "PLAYBACK_STREAM");
                bind->stream.type = Stream_Type::PLAYBACK_STREAM;
            } else if (string_equals(&parser->buffer[parser->now], "CAPTURE_STREAM")) {
                string_add(parser->buffer, "CAPTURE_STREAM");
                bind->stream.type = Stream_Type::CAPTURE_STREAM;
            }
            has_stream_type = true;
        } else if (string_equals(&parser->buffer[parser->now], "stream_name")) { 
            string_add(parser->buffer, "stream_name");
            skip_to_next_word(parser);
            u64 count = number_of_bytes_to_end(*parser);
            void *name = calloc(count + 1, 1);
            memcpy(name, &parser->buffer[parser->now], count);
            parser->now += count;
            bind->stream.name = (const char *)name;
            has_stream_name = true;
        } else {
            log_error("Failed to parse the bind. Unknown entry.");
            return false;
        } 
    }

    if (!has_button_type) {
        log_error("Failed to parse the bind. Button type missing.");
        return false;
    }

    if (!has_button_state) {
        log_error("Failed to parse the bind. Button state missing.");
        return false;
    }

    if (!has_audio_action) {
        log_error("Failed to parse the bind. Audio action missing.");
        return false;
    }

    if (!has_stream_type) {
        log_error("Failed to parse the bind. Stream type missing.");
        return false;
    }

    if (!has_stream_name) {
        log_error("Failed to parse the bind. Stream name missing.");
        return false;
    }

    bind->stream.has_audio_stream = true;
    return true;
}

Config load_config() {
    // return get_debug_config();

    u8 *config_content = read_entire_file("global_talk.cfg");
    if (config_content == NULL) {
        return {};
    }

    Config_Parser parser = {
        .buffer = config_content,
        .now    = 0,
    };

    Config config;

    while (true) {
        u8 byte = parser.buffer[parser.now];
        if (byte == '\0') {
            break;
        }

        if (byte == ' ' || byte == '\n' || byte == '\r') {
            step_byte(&parser);
            continue;
        } 

        if (string_equals(&parser.buffer[parser.now], "[bind]")) {
            Audio_Bind bind;
            bool success = parse_bind(&parser, &bind);
            if (!success) {
                return {};
            }
            config.binds.push_back(bind);
            continue;
        } 

        log_error("Failed to parse config. Unknown section.");
        return {};
    }

    return config;
}
