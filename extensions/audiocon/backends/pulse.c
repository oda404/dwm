
#include <dwm/audiocon.h>
#include <dwm/util.h>
#include <math.h>
#include <pulse/pulseaudio.h>
#include <string.h>
#include <threads.h>

typedef struct S_PulseAudioData
{
    pa_threaded_mainloop* mainloop;
    pa_mainloop_api* api;
    pa_context* ctx;

    /* Cached sink info */
    char* def_sink_name;
    pa_cvolume def_sink_vol;

    /* Cached source info */
    char* def_src_name;
    pa_cvolume def_src_vol;

    /* Sink callback */
    volume_change_cb_t sink_vol_changed;
    void* sink_vol_changed_userdata;

    /* Source callback */
    volume_change_cb_t src_vol_changed;
    void* src_vol_changed_userdata;
} PulseAudioData;

static PulseAudioData g_pulsedata;

/* This function will only get called for the default sink. */
static void
sink_info_cb(pa_context* c, const pa_sink_info* info, int eol, void* userdata)
{
    if (!info)
        return;

    unsigned int vol =
        ceil(((float)pa_cvolume_avg(&info->volume) / PA_VOLUME_NORM) * 100);

    /* Cache the sink name */
    if (g_pulsedata.def_sink_name)
        free(g_pulsedata.def_sink_name);
    g_pulsedata.def_sink_name = strdup(info->name);

    /* Cache the sink volume */
    g_pulsedata.def_sink_vol = info->volume;

    if (g_pulsedata.sink_vol_changed)
        g_pulsedata.sink_vol_changed(
            vol, g_pulsedata.sink_vol_changed_userdata);
}

static void source_info_cb(
    pa_context* c, const pa_source_info* info, int eol, void* userdata)
{
    if (!info)
        return;

    unsigned int vol =
        ceil(((float)pa_cvolume_avg(&info->volume) / PA_VOLUME_NORM) * 100);

    /* Cache the src name */
    if (g_pulsedata.def_src_name)
        free(g_pulsedata.def_src_name);
    g_pulsedata.def_src_name = strdup(info->name);

    /* Cache the src volume */
    g_pulsedata.def_src_vol = info->volume;

    if (g_pulsedata.src_vol_changed)
        g_pulsedata.src_vol_changed(vol, g_pulsedata.src_vol_changed_userdata);
}

static void server_info_callback(
    pa_context* ctx, const pa_server_info* info, void* userdata)
{
    pa_context_get_sink_info_by_name(
        ctx, info->default_sink_name, sink_info_cb, userdata);
    pa_context_get_source_info_by_name(
        ctx, info->default_source_name, source_info_cb, userdata);
}

static void subscribe_callback(
    pa_context* ctx,
    pa_subscription_event_type_t type,
    uint32_t idx,
    void* userdata)
{
    type &= PA_SUBSCRIPTION_EVENT_FACILITY_MASK;

    pa_operation* op = NULL;

    switch (type)
    {
    case PA_SUBSCRIPTION_EVENT_SINK:
        op =
            pa_context_get_sink_info_by_index(ctx, idx, sink_info_cb, userdata);
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE:
        op = pa_context_get_source_info_by_index(
            ctx, idx, source_info_cb, userdata);
        break;

    case PA_SUBSCRIPTION_EVENT_SERVER:
        op = pa_context_get_server_info(ctx, server_info_callback, userdata);
        break;

    default:
        break;
    }

    if (op)
        pa_operation_unref(op);
}

static void context_state_callback(pa_context* ctx, void* userdata)
{
    switch (pa_context_get_state(g_pulsedata.ctx))
    {
    case PA_CONTEXT_READY:
        pa_context_get_server_info(
            g_pulsedata.ctx, server_info_callback, userdata);

        pa_context_set_subscribe_callback(
            g_pulsedata.ctx, subscribe_callback, userdata);
        pa_context_subscribe(
            g_pulsedata.ctx,
            PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE |
                PA_SUBSCRIPTION_MASK_SERVER,
            NULL,
            NULL);

        [[fallthrough]];
    case PA_CONTEXT_TERMINATED:
    case PA_CONTEXT_FAILED:
        pa_threaded_mainloop_signal(g_pulsedata.mainloop, 0);
        break;

    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
    case PA_CONTEXT_UNCONNECTED:
        break;
    }
}

static void pulsebackend_set_sink_cvolume(const pa_cvolume* vol)
{
    pa_context_set_sink_volume_by_name(
        g_pulsedata.ctx, g_pulsedata.def_sink_name, vol, NULL, NULL);
}

static void pulsebackend_set_source_cvolume(const pa_cvolume* vol)
{
    pa_context_set_source_volume_by_name(
        g_pulsedata.ctx, g_pulsedata.def_src_name, vol, NULL, NULL);
}

int audiocon_init()
{
    /* The pulseaudio mainloop */
    g_pulsedata.mainloop = pa_threaded_mainloop_new();
    if (!g_pulsedata.mainloop)
        return 1;

    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    g_pulsedata.api = pa_threaded_mainloop_get_api(g_pulsedata.mainloop);

    g_pulsedata.ctx = pa_context_new(g_pulsedata.api, "dwm");
    if (!g_pulsedata.ctx)
    {
        pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
        pa_threaded_mainloop_free(g_pulsedata.mainloop);
        return 1;
    }

    pa_context_set_state_callback(
        g_pulsedata.ctx, context_state_callback, NULL);

    if (pa_context_connect(
            g_pulsedata.ctx, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) < 0)
    {
        pa_context_unref(g_pulsedata.ctx);
        pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
        pa_threaded_mainloop_free(g_pulsedata.mainloop);
        return 1;
    }

    if (pa_threaded_mainloop_start(g_pulsedata.mainloop) < 0)
    {
        pa_context_disconnect(g_pulsedata.ctx);
        pa_context_unref(g_pulsedata.ctx);
        pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
        pa_threaded_mainloop_free(g_pulsedata.mainloop);
        return 1;
    }

    /* Wait for the context to be ready. */
    pa_threaded_mainloop_wait(g_pulsedata.mainloop);
    if (pa_context_get_state(g_pulsedata.ctx) != PA_CONTEXT_READY)
    {
        pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
        pa_threaded_mainloop_stop(g_pulsedata.mainloop);
        pa_context_disconnect(g_pulsedata.ctx);
        pa_context_unref(g_pulsedata.ctx);
        pa_threaded_mainloop_free(g_pulsedata.mainloop);
        return 1;
    }

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);

    return 0;
}

void audiocon_destroy()
{
    pa_threaded_mainloop_stop(g_pulsedata.mainloop);
    pa_context_disconnect(g_pulsedata.ctx);
    pa_context_unref(g_pulsedata.ctx);
    pa_threaded_mainloop_free(g_pulsedata.mainloop);

    if (g_pulsedata.def_sink_name)
        free(g_pulsedata.def_sink_name);

    if (g_pulsedata.def_src_name)
        free(g_pulsedata.def_src_name);

    memset(&g_pulsedata, 0, sizeof(PulseAudioData));
}

int audiocon_on_output_volume_change(volume_change_cb_t cb, void* userdata)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    g_pulsedata.sink_vol_changed = cb;
    g_pulsedata.sink_vol_changed_userdata = userdata;

    /* Force a sink info update */
    pa_context_get_sink_info_by_name(
        g_pulsedata.ctx, g_pulsedata.def_sink_name, sink_info_cb, NULL);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_set_output_volume(u32 vol)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume;
    pa_cvolume_set(
        &volume,
        g_pulsedata.def_sink_vol.channels,
        (float)vol * PA_VOLUME_NORM / 100);
    pulsebackend_set_sink_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_inc_output_volume(u32 inc)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume = g_pulsedata.def_sink_vol;
    pa_cvolume_inc(&volume, (float)inc * PA_VOLUME_NORM / 100);
    pulsebackend_set_sink_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_dec_output_volume(u32 dec)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume = g_pulsedata.def_sink_vol;
    pa_cvolume_dec(&volume, (float)dec * PA_VOLUME_NORM / 100);
    pulsebackend_set_sink_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_on_input_volume_change(volume_change_cb_t cb, void* userdata)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    g_pulsedata.src_vol_changed = cb;
    g_pulsedata.src_vol_changed_userdata = userdata;

    /* Force a source info update */
    pa_context_get_source_info_by_name(
        g_pulsedata.ctx, g_pulsedata.def_src_name, source_info_cb, NULL);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_set_input_volume(u32 vol)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume;
    pa_cvolume_set(
        &volume,
        g_pulsedata.def_src_vol.channels,
        (float)vol * PA_VOLUME_NORM / 100);
    pulsebackend_set_source_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_inc_input_volume(u32 inc)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume = g_pulsedata.def_src_vol;
    pa_cvolume_inc(&volume, (float)inc * PA_VOLUME_NORM / 100);
    pulsebackend_set_source_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}

int audiocon_dec_input_volume(u32 dec)
{
    pa_threaded_mainloop_lock(g_pulsedata.mainloop);

    pa_cvolume volume = g_pulsedata.def_src_vol;
    pa_cvolume_dec(&volume, (float)dec * PA_VOLUME_NORM / 100);
    pulsebackend_set_source_cvolume(&volume);

    pa_threaded_mainloop_unlock(g_pulsedata.mainloop);
    return 0;
}
