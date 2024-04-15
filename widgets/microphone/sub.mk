
include ${EXTENSIONS_DIR}/audiocon/sub.mk

WIDGETS_SRC += widgets/microphone/microphone.c

CPPFLAGS += -DHAVE_WIDGET_MICROPHONE
