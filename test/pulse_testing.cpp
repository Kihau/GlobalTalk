#include <cstdio>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>

#define lock_scope(mainloop, code)         \
    pa_threaded_mainloop_lock(mainloop);   \
    code;                                  \
    pa_threaded_mainloop_unlock(mainloop); \


int main(int argc, char **argv) {
    auto pulse = pa_threaded_mainloop_new();
    pa_threaded_mainloop_start(pulse);

    auto api = pa_threaded_mainloop_get_api(pulse);
    auto context = pa_context_new(api, "GlobalTalk");
    assert(context);

    auto connection_callback = [](pa_context *context, void *userdata) {
        auto pulse = (pa_threaded_mainloop *)userdata;
        if (pa_context_get_state(context) == PA_CONTEXT_READY) {
            pa_threaded_mainloop_signal(pulse, 0);
        }
    };

    pa_context_set_state_callback(context, connection_callback, (void*)pulse);

    lock_scope(pulse, pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, NULL))

    lock_scope(pulse, {
        while (pa_context_get_state(context) != PA_CONTEXT_READY) {
            pa_threaded_mainloop_wait(pulse);
        }
    })

    printf("Output Devices:\n");
    {
        auto info_callback = [](pa_context *context, const pa_source_info *info, int eol, void *userdata) {
            if (info == nullptr) {
                pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
                return;
            }

            // printf("  %i : %s - %s\n", info->index, info->name, info->description);
            printf("  %s\n", info->description);
            pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
        };

        auto op = pa_context_get_source_info_list(context, info_callback, (void*)pulse);	
        assert(op);

        lock_scope(pulse, {
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
                pa_threaded_mainloop_wait(pulse);
            }
            pa_operation_cancel(op);
        })
    }
    printf("\n");

    printf("Input Devices:\n");
    {
        auto info_callback = [](pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
            if (info == nullptr) {
                pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
                return;
            }

            // printf("  %i : %s - %s\n", info->index, info->name, info->description);
            printf("  %s\n", info->description);
            pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
        };

        auto op = pa_context_get_sink_info_list(context, info_callback, (void*)pulse);	
        assert(op);

        lock_scope(pulse, {
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
                pa_threaded_mainloop_wait(pulse);
            }
            pa_operation_cancel(op);
        })
    }
    
    printf("\n");

    printf("Recording:\n");
    {
        auto info_callback = [](pa_context *context, const pa_source_output_info *info, int eol, void *userdata) {
            if (info == nullptr) {
                pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
                return;
            }

            const char *app_name = pa_proplist_gets(info->proplist, "application.name");
            // printf("  %i : %s - %s\n", info->index, info->name, app_name);
            printf("  %s - %s\n", app_name, info->name);
            pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
        };

        auto op = pa_context_get_source_output_info_list(context, info_callback, (void*)pulse);	
        assert(op);

        lock_scope(pulse, {
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
                pa_threaded_mainloop_wait(pulse);
            }
            pa_operation_cancel(op);
        })
    }
    printf("\n");

    printf("Playback:\n");
    {
        auto info_callback = [](pa_context *context, const pa_sink_input_info *info, int eol, void *userdata) {
            if (info == nullptr) {
                pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
                return;
            }

            const char *app_name = pa_proplist_gets(info->proplist, "application.name");
            printf("  %s - %s\n", app_name, info->name);
            // printf("  %i : %s - %s\n", info->index, info->name, app_name);
            pa_threaded_mainloop_signal((pa_threaded_mainloop *)userdata, 0);
        };

        auto op = pa_context_get_sink_input_info_list(context, info_callback, (void*)pulse);	
        assert(op);

        lock_scope(pulse, {
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
                pa_threaded_mainloop_wait(pulse);
            }
            pa_operation_cancel(op);
        })
    }

    return 0;
}
