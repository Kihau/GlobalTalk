#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <stdio.h>

// Implemented by asgoodasbread@github

int main() {
    HRESULT result = CoInitialize(NULL);
    if (FAILED(result)) {
        printf("Failed to initialize COM library\n");
        return 1;
    }

    IMMDeviceEnumerator *enumerator = NULL;
    result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&enumerator);
    if (FAILED(result)) {
        printf("Failed to create device enumerator\n");
        CoUninitialize();
        return 1;
    }

    IMMDevice *default_device = NULL;
    result = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &default_device);
    if (FAILED(result)) {
        printf("Failed to get default capture endpoint\n");
        enumerator->Release();
        CoUninitialize();
        return 1;
    }

    IAudioEndpointVolume *endpoint_volume = NULL;
    result = default_device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&endpoint_volume);
    if (FAILED(result)) {
        printf("Failed to activate endpoint volume interface\n");
        default_device->Release();
        enumerator->Release();
        CoUninitialize();
        return 1;
    }

    while (true) {
        if ((GetKeyState(VK_XBUTTON1) & 0x80) != 0) {
            result = endpoint_volume->SetMute(TRUE, NULL);
            if (FAILED(result)) {
                printf("Failed to mute microphone\n");
            } else {
                printf("Microphone disabled successfully\n");
            }
            printf("Mouse 4 pressed\n");
            Sleep(300);
        }

        else if ((GetKeyState(VK_XBUTTON2) & 0x80) != 0) {
            result = endpoint_volume->SetMute(FALSE, NULL);
            if (FAILED(result)) {
                printf("Failed to unmute microphone\n");
            } else {
                printf("Microphone enabled successfully\n");
            }
            printf("Mouse 5 pressed\n");
            Sleep(300);
        } else if (GetAsyncKeyState(VK_ESCAPE)) {
            endpoint_volume->Release();
            default_device->Release();
            enumerator->Release();
            CoUninitialize();
            return 0;
        }
    }

}
