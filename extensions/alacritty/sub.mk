
ifeq (, $(shell which alacritty 2> /dev/null))
$(error No alacritty found in PATH. Disable the alacritty dwm extension or install alacritty)
endif

CPPFLAGS += -DUSE_ALACRITTY
