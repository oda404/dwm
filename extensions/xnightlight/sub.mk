
ifeq (, $(shell which xnightlight 2> /dev/null))
$(error No xnightlight found in PATH. Disable the xnightlight dwm extension or install xnightlight)
endif

EXTENSIONS_SRC += extensions/xnightlight/xnightlight.c
CPPFLAGS += -DHAVE_XNIGHTLIGHT
