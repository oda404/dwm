
# Hardcoded for now
ifeq (, $(shell which pactl 2> /dev/null))
$(error No pactl found in PATH. Disable the audiocon dwm extension and everything that depends on it or install pulse audio)
endif

HAVE_PULSE = 1

ifeq (${HAVE_AUDIOCON},)
    HAVE_AUDIOCON = 1
    CPPFLAGS += -DHAVE_AUDIOCON
    INCS += -I$(EXTENSIONS_DIR)/audiocon/include

    ifeq (${HAVE_PULSE}, 1)
		EXTENSIONS_SRC += $(EXTENSIONS_DIR)/audiocon/backends/pulse.c
        CPPFLAGS += -DHAVE_PULSE
        LIBS += -lpulse
    endif
endif
