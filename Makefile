# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = \
core/drw.c core/dwm.c core/util.c core/log.c core/widget.c core/colors.c \
core/screenshot.c core/multimon.c core/wallpaper.c ${WIDGETS_SRC} ${EXTENSIONS_SRC} ${EXTRA_SRC} ${LTILS_SRC}

COBJS = ${SRC:%.c=$(BUILDDIR)/%.c.o}
CDEPS = ${SRC:%.c=$(BUILDDIR)/%.c.d}
CORE_DEPS = config.h config.mk

DWM = $(BUILDDIR)/dwm

all: options ${DWM}

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

config.h:
	cp config.def.h $@

${DWM}: ${COBJS}
	${CC} -o $@ ${COBJS} ${LDFLAGS}

-include ${CDEPS}
$(BUILDDIR)/%.c.o: %.c ${CORE_DEPS}
	@mkdir -p $(dir $@)
	${CC} -c ${CFLAGS} $< -o $@

clean:
	rm -f ${DWM} ${COBJS} ${CDEPS} dwm-${VERSION}.tar.gz

dist: clean
	mkdir -p dwm-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		dwm.1 drw.h util.h ${SRC} transient.c dwm-${VERSION}
	tar -cf dwm-${VERSION}.tar dwm-${VERSION}
	gzip dwm-${VERSION}.tar
	rm -rf dwm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${DWM} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

uninstall:
# Remove log/mmsetup files
	rm -f ${DESTDIR}${PREFIX}/bin/dwm\
		${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all options clean dist install uninstall
