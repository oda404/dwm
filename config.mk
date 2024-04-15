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

# includes and libs
INCS = -I${X11INC} -I${FREETYPEINC} -Iltils/include -Iinclude
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS} -lXrender -lm

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" \
${XINERAMAFLAGS}

EXTENSIONS_DIR = extensions/
WIDGETS_DIR = widgets/

EXTENSIONS_SRC = 
WIDGETS_SRC = 

include extensions/xnightlight/sub.mk
include extensions/slock/sub.mk
include extensions/alacritty/sub.mk
include extensions/dmenu/sub.mk
include extensions/x2screenshot/sub.mk

include widgets/time/sub.mk
include widgets/cpuinfo/sub.mk
include widgets/backlight/sub.mk
include widgets/battery/sub.mk
include widgets/mem/sub.mk
include widgets/microphone/sub.mk
include widgets/speakers/sub.mk
include widgets/network/sub.mk

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
