
ifeq (, $(shell which slock 2> /dev/null))
$(error No slock found in PATH. Disable the slock dwm extension or install slock)
endif

CPPFLAGS += -DUSE_SLOCK
