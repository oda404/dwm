
#ifndef DWM_CONFIG_WIDGETS_H
#define DWM_CONFIG_WIDGETS_H

#ifdef HAVE_WIDGET_TIME
#include "../../../widgets/time/time.h"
#endif // HAVE_WIDGET_TIME

#ifdef HAVE_WIDGET_SPEAKERS
#include "../../../widgets/speakers/speakers.h"
#endif // HAVE_WIDGET_SPEAKERS

#ifdef HAVE_WIDGET_MICROPHONE
#include "../../../widgets/microphone/microphone.h"
#endif // HAVE_WIDGET_MICROPHONE

#ifdef HAVE_WIDGET_BACKLIGHT
#include "../../../widgets/backlight/backlight.h"
#endif // HAVE_WIDGET_BACKLIGHT

#ifdef HAVE_WIDGET_BATTERY
#include "../../../widgets/battery/battery.h"
#endif // HAVE_WIDGET_BATTERY

#ifdef HAVE_WIDGET_CPUINFO
#include "../../../widgets/cpuinfo/cpuload.h"
#endif // HAVE_WIDGET_CPUINFO

#ifdef HAVE_WIDGET_MEM
#include "../../../widgets/mem/mem.h"
#endif // HAVE_WIDGET_MEM

#ifdef HAVE_WIDGET_NETWORK
#include "../../../widgets/network/network.h"
#endif // HAVE_WIDGET_NETWORK

#endif // !DWM_CONFIG_WIDGETS_H
