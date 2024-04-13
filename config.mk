# dwm version
VERSION = 6.3

# Customize below to fit your system
BUILDDIR = build

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA

# freetype
FREETYPELIBS = -lfontconfig -lXft
FREETYPEINC = /usr/include/freetype2
# OpenBSD (uncomment)
#FREETYPEINC = ${X11INC}/freetype2
#MANPREFIX = ${PREFIX}/man

# Audio control/display API
AUDIOCON_SRC = core/audiocon.c
AUDIOCON_FLAGS = -DUSE_AUDIOCON

# PulseAudio support for audiocon
HAVE_PULSE = 1
PULSE_SRC = audiocon_backends/pulse.c
PULSE_LIBS = -lpulse
PULSE_FLAGS = -DUSE_PULSE

# includes and libs
INCS = -I${X11INC} -I${FREETYPEINC} -Iltils/include
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS} -lXrender -lm

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" \
${XINERAMAFLAGS} -Iinclude

# Config options
ifeq ($(HAVE_PULSE), 1)
    HAVE_AUDIOCON = 1
	EXTRA_SRC += $(PULSE_SRC)
	LIBS += $(PULSE_LIBS)
	CPPFLAGS += $(PULSE_FLAGS)
endif

ifeq ($(HAVE_AUDIOCON), 1)
    EXTRA_SRC += $(AUDIOCON_SRC)
	CPPFLAGS += $(AUDIOCON_FLAGS)
endif

WIDGETS_SRC = widgets/backlight.c widgets/battery.c widgets/microphone.c widgets/network.c \
widgets/speakers.c widgets/time.c widgets/cpuinfo/cpuload.c widgets/mem.c 

LTILS_SRC = ltils/src/proc/proc.c ltils/src/string/numeric.c ltils/src/cleanup.c

CFLAGS   = \
-std=c2x -Wall -Wno-constant-logical-operand \
-Werror=implicit-function-declaration \
-Wno-deprecated-declarations -O3 -march=native \
${INCS} ${CPPFLAGS} -MMD -MP

LDFLAGS  = ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"

#LDFLAGS = ${LIBS}

# compiler and linker
CC = clang
