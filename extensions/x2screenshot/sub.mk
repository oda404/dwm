
ifeq (, $(shell which x2screenshot 2> /dev/null))
$(error No x2screenshot found in PATH. Disable the x2screenshot dwm extension or install x2screenshot)
endif

CPPFLAGS += -DUSE_X2SCREENSHOT
