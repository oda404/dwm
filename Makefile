# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = \
core/drw.c core/dwm.c core/util.c core/log.c core/widget.c core/colors.c widgets/backlight.c widgets/battery.c \
widgets/microphone.c widgets/network.c widgets/speakers.c widgets/time.c widgets/cpuinfo/cpuload.c \
$(EXTRA_SRC)

OBJ = ${SRC:%.c=$(BUILDDIR)/%.c.o}

all: options $(BUILDDIR)/dwm

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	${CC} -c ${CFLAGS} $< -o $@

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

$(BUILDDIR)/dwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f $(BUILDDIR)/dwm ${OBJ} dwm-${VERSION}.tar.gz

dist: clean
	mkdir -p dwm-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		dwm.1 drw.h util.h ${SRC} dwm.png transient.c dwm-${VERSION}
	tar -cf dwm-${VERSION}.tar dwm-${VERSION}
	gzip dwm-${VERSION}.tar
	rm -rf dwm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f $(BUILDDIR)/dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dwm\
		${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all options clean dist install uninstall
