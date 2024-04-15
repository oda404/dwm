
#ifndef DWM_AUDIOCON_H
#define DWM_AUDIOCON_H

#include <dwm/types.h>

typedef void (*volume_change_cb_t)(u32 newvol, void *userdata);

int audiocon_init();
void audiocon_destroy();

int audiocon_on_output_volume_change(volume_change_cb_t, void *userdata);
int audiocon_set_output_volume(u32 vol);
int audiocon_inc_output_volume(u32 inc);
int audiocon_dec_output_volume(u32 dec);

int audiocon_on_input_volume_change(volume_change_cb_t, void *userdata);
int audiocon_set_input_volume(u32 vol);
int audiocon_inc_input_volume(u32 inc);
int audiocon_dec_input_volume(u32 dec);

#endif // !DWM_AUDIOCON_H
