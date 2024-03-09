#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdint>

// #include "query_state.h"
#include <X11/extensions/XInput2.h>
#include <alsa/asoundlib.h>

using i8 =  int8_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 =  uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

template <typename T> class ScopeDrop {
    T lambda;
    
public:
    ScopeDrop(T lambda) : lambda(lambda) {}
    ~ScopeDrop() { lambda(); }
    
private:
    ScopeDrop(const ScopeDrop &);
    ScopeDrop &operator=(const ScopeDrop &);
};

class ScopeDropCreate {
public:
    template <typename T> ScopeDrop<T> operator+(T t) { return t; }
};

#define _concatenate(x, y) x##y
#define _defer_internal(line) _concatenate(_defer_, line)
#define defer const auto &_defer_internal(__LINE__) = ScopeDropCreate() + [&]()


int query_state_new();

int main(int argc, char **argv) {
    // return query_state();
    return query_state_new();
}

static bool mute_microphone(snd_mixer_elem_t *element) {
    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        snd_mixer_selem_channel_id_t channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(element, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(element, channel, 0);
        if (result >= 0) {
            return false;
        }
    }

    return true;
}

static int unmute_microphone(snd_mixer_elem_t *element) {
    for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
        snd_mixer_selem_channel_id_t channel = (snd_mixer_selem_channel_id_t)i;

        if (!snd_mixer_selem_has_capture_channel(element, channel)) {
            continue;
        }

        int result = snd_mixer_selem_set_capture_switch(element, channel, 1);
        if (result >= 0) {
            return false;
        }
    }

    return true;
}

int query_state_new() {
    // X11 initialization stuff.
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Failed to open X11 display\n");
        return 1;
    }

    defer { XCloseDisplay(display); };

    int op_code, event_code, error_code;
    if (XQueryExtension(display, "XInputExtension", &op_code, &event_code, &error_code) == 0) {
        fprintf(stderr, "ERROR: X Input extension is not available.\n");
        return 1;
    }

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        fprintf(stderr, "ERROR: XI2 not available. Server supports %d.%d\n", major, minor);
        return 1;
    }

    unsigned char mask[8] = { 0 };
    XISetMask(mask, XI_ButtonPress);
    XISetMask(mask, XI_ButtonRelease);

    XIEventMask event_mask = {
        .deviceid = XIAllDevices,
        .mask_len = sizeof(mask),
        .mask = mask,
    };

    Window window = DefaultRootWindow(display);
    XISelectEvents(display, window, &event_mask, 1);


    // Alsa initialization stuff.
    snd_mixer_t *handle = NULL;
    const char *card = "default";

    int result = snd_mixer_open(&handle, 0); 
    if (result < 0) {
        printf("Mixer %s open error: %s\n", card, snd_strerror(result));
        return result;
    }
    defer { snd_mixer_close(handle); };

    int smixer_level = 0;
    if (smixer_level == 0 && (result = snd_mixer_attach(handle, card)) < 0) {
        printf("Mixer attach %s error: %s", card, snd_strerror(result));
        return result;
    }

    struct snd_mixer_selem_regopt smixer_options;
    result = snd_mixer_selem_register(handle, smixer_level > 0 ? &smixer_options : NULL, NULL);
    if (result < 0) {
        printf("Mixer register error: %s", snd_strerror(result));
        return result;
    }

    result = snd_mixer_load(handle);
    if (result < 0) {
        printf("Mixer %s load error: %s", card, snd_strerror(result));
        return result;
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
        return -ENOENT;
    }


    XEvent event;
    while (true) {
        XNextEvent(display, &event);

        if (event.xcookie.type != GenericEvent || event.xcookie.extension != op_code) {
            continue;
        }

        int result = XGetEventData(display, &event.xcookie);
        if (result == 0) {
            continue;
        }
        defer { XFreeEventData(display, &event.xcookie); };

        switch (event.xcookie.evtype) {
            case XI_ButtonPress: {
                XIDeviceEvent *device_event = (XIDeviceEvent *)event.xcookie.data;
                KeyCode code = device_event->detail;

                if (code == 9) {
                    printf("Mouse button pressed\n");
                    unmute_microphone(element);
                }
            } break;

            case XI_ButtonRelease: {
                XIDeviceEvent *device_event = (XIDeviceEvent *)event.xcookie.data;
                KeyCode code = device_event->detail;

                if (code == 9) {
                    printf("Mouse button release\n");
                    mute_microphone(element);
                }
            } break;
        }
    }

    return 0;
}
