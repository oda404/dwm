
#ifndef DWM_AUDIOCON_BACKEND_PULSE_H
#define DWM_AUDIOCON_BACKEND_PULSE_H

#include <dwm/audiocon.h>

int pulsebackend_init();
void pulsebackend_detroy();

int pulsebackend_on_output_volume_change(volume_change_cb_t, void *userdata);
int pulsebackend_set_output_volume(u32 vol);
int pulsebackend_inc_output_volume(u32 inc);
int pulsebackend_dec_output_volume(u32 dec);

int pulsebackend_on_input_volume_change(volume_change_cb_t, void *userdata);
int pulsebackend_set_input_volume(u32 vol);
int pulsebackend_inc_input_volume(u32 inc);
int pulsebackend_dec_input_volume(u32 dec);

#endif // !DWM_AUDIOCON_BACKEND_PULSE_H
