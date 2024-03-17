#include "audio.h"

bool initialize_audio(Audio *audio) {
    HRESULT result = CoInitialize(NULL);
    if (FAILED(result)) {
        log_error("Failed to initialize COM library.");
        return false;
    }

    IMMDeviceEnumerator *enumerator = NULL;
    result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&enumerator);
    if (FAILED(result)) {
        log_error("Failed to create device enumerator.");
        CoUninitialize();
        return false;
    }

    IMMDevice *default_device = NULL;
    result = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &default_device);
    if (FAILED(result)) {
        log_error("Failed to get default capture endpoint.");
        enumerator->Release();
        CoUninitialize();
        return false;
    }

    IAudioEndpointVolume *endpoint_volume = NULL;
    result = default_device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&endpoint_volume);
    if (FAILED(result)) {
        log_error("Failed to activate endpoint volume interface.");
        default_device->Release();
        enumerator->Release();
        CoUninitialize();
        return false;
    }

    audio->endpoint_volume = endpoint_volume;
    audio->default_device  = default_device;
    audio->enumerator      = enumerator;

    return true;
}

void destroy_audio(Audio audio) {
    audio.endpoint_volume->Release();
    audio.default_device->Release();
    audio.enumerator->Release();
    CoUninitialize();
}

bool mute_microphone(Audio audio) {
    HRESULT result = audio.endpoint_volume->SetMute(FALSE, NULL);
    return FAILED(result) ? false : true;
}

bool unmute_microphone(Audio audio) {
    HRESULT result = audio.endpoint_volume->SetMute(TRUE, NULL);
    return FAILED(result) ? false : true;
}

bool is_microphone_muted(Audio audio) {
    return false;
}

bool play_raw_sound(Audio audio, const unsigned char *sound_buffer, size_t buffer_size) {
    return false;
}

bool play_wav_sound(Audio audio, const unsigned char *sound_buffer, size_t buffer_size) {
    return false;
}
