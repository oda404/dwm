
ifeq (, $(shell which xbacklight 2> /dev/null))
$(error No xbacklight found in PATH. Disable the backlight dwm widget or install xbacklight)
endif

WIDGETS_SRC += widgets/backlight/backlight.c

CPPFLAGS += -DHAVE_WIDGET_BACKLIGHT
