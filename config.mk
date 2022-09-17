# dwm version
VERSION = 6.3

# Customize below to fit your system

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

# Enable the audio connection API. Used for getting/setting the output/input sound volumes.
# Note that this also requires a backend. See below.
AUDIOCON_SRC = audiocon.c
AUDICON_FLAGS = -DUSE_AUDIOCON

# PulseAudio backend for the audio connection.
PULSE_SRC = audiocon_backends/pulse.c
PULSE_LIBS = -lpulse
PULSE_FLAGS = -DUSE_PULSE

# includes and libs
INCS = -I${X11INC} -I${FREETYPEINC}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS} ${PULSE_LIBS}

MARCH = znver1

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" \
${XINERAMAFLAGS} ${AUDICON_FLAGS} ${PULSE_FLAGS}

CFLAGS   = -std=c2x -pedantic -Wall -Werror=implicit-function-declaration \
-Wno-deprecated-declarations -O3 -march=${MARCH} ${INCS} ${CPPFLAGS}

LDFLAGS  = ${LIBS}

BUILDDIR = build

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = clang
