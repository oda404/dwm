
#include <dwm/audiocon.h>
#include <stddef.h>

typedef int (*backend_init)();
typedef void (*backend_destroy)();

typedef int (*backend_on_volume_change_t)(volume_change_cb_t, void *userdata);
typedef int (*backend_set_volume_t)(u32 vol);

#if defined(USE_PULSE)
#include "../audiocon_backends/pulse.h"
static backend_init g_backend_init = pulsebackend_init;
static backend_destroy g_backend_destroy = pulsebackend_detroy;

static backend_on_volume_change_t g_backend_on_output_volume_change = pulsebackend_on_output_volume_change;
static backend_set_volume_t g_backend_set_output_volume = pulsebackend_set_output_volume;
static backend_set_volume_t g_backend_inc_output_volume = pulsebackend_inc_output_volume;
static backend_set_volume_t g_backend_dec_output_volume = pulsebackend_dec_output_volume;

static backend_on_volume_change_t g_backend_on_input_volume_change = pulsebackend_on_input_volume_change;
static backend_set_volume_t g_backend_set_input_volume = pulsebackend_set_input_volume;
static backend_set_volume_t g_backend_inc_input_volume = pulsebackend_inc_input_volume;
static backend_set_volume_t g_backend_dec_input_volume = pulsebackend_dec_input_volume;
#endif

int audiocon_init()
{
    return g_backend_init();
}

void audiocon_destroy()
{
    g_backend_destroy();
}

int audiocon_on_output_volume_change(volume_change_cb_t cb, void *userdata)
{
    return g_backend_on_output_volume_change(cb, userdata);
}

int audiocon_set_output_volume(u32 vol)
{
    return g_backend_set_output_volume(vol);
}

int audiocon_inc_output_volume(u32 inc)
{
    return g_backend_inc_output_volume(inc);
}

int audiocon_dec_output_volume(u32 inc)
{
    return g_backend_dec_output_volume(inc);
}

int audiocon_on_input_volume_change(volume_change_cb_t cb, void *userdata)
{
    return g_backend_on_input_volume_change(cb, userdata);
}

int audiocon_set_input_volume(u32 vol)
{
    return g_backend_set_input_volume(vol);
}

int audiocon_inc_input_volume(u32 inc)
{
    return g_backend_inc_input_volume(inc);
}

int audiocon_dec_input_volume(u32 inc)
{
    return g_backend_dec_input_volume(inc);
}
