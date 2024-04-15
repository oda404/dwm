
ifeq (, $(shell which dmenu_run 2> /dev/null))
$(error No dmenu_run found in PATH. Disable the dmenu dwm extension or install dmenu)
endif

EXTENSIONS_SRC += extensions/dmenu/dmenu.c

CPPFLAGS += -DUSE_DMENU
