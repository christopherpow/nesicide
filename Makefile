PREFIX=/usr/local
DESTDIR=
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
QMAKEFLAGS += -spec macx-g++ # force makefile generation on osx
ifeq ($(DEBUG),1)
QMAKEFLAGS += -config debug # force makefile generation on osx
else
QMAKEFLAGS += -config release # force makefile generation on osx
endif
endif

all: default

default: apps/famiplayer/famiplayer apps/famitracker/famitracker apps/nes-emulator/nes-emulator apps/ide/nesicide

%/Makefile: %/*.pro
	cd `dirname $@` && qmake $(QMAKEFLAGS)

libs/famitracker/libfamitracker.so.1.0.0: libs/famitracker/Makefile FORCE
	$(MAKE) -C libs/famitracker

libs/nes/libnes-emulator.so.1.0.0: libs/nes/Makefile FORCE
	$(MAKE) -C libs/nes

libs/c64/libc64-emulator.so.1.0.0: libs/c64/Makefile FORCE
	$(MAKE) -C libs/c64

apps/nes-emulator/nes-emulator: apps/nes-emulator/Makefile libs/nes/libnes-emulator.so.1.0.0 FORCE
	$(MAKE) -C apps/nes-emulator

apps/ide/nesicide: apps/ide/Makefile libs/nes/libnes-emulator.so.1.0.0 libs/c64/libc64-emulator.so.1.0.0 libs/famitracker/libfamitracker.so.1.0.0 FORCE
	$(MAKE) -C apps/ide

apps/famiplayer/famiplayer: apps/famiplayer/Makefile libs/famitracker/libfamitracker.so.1.0.0 FORCE
	$(MAKE) -C apps/famiplayer

apps/famitracker/famitracker: apps/famitracker/Makefile libs/famitracker/libfamitracker.so.1.0.0 FORCE
	$(MAKE) -C apps/famitracker

clean:
	cd libs/famitracker && $(MAKE) clean; rm -f libfamitracker.so*
	cd libs/nes && $(MAKE) clean; rm -f libnes-emulator.so*
	cd libs/c64 && $(MAKE) clean; rm -f libc64-emulator.so*
	cd apps/nes-emulator && $(MAKE) clean; rm -f nes-emulator
	cd apps/ide && $(MAKE) clean; rm -f nesicide
	cd apps/famitracker && $(MAKE) clean; rm -f famitracker
	cd apps/famiplayer && $(MAKE) clean; rm -f famiplayer
	rm -f */*/Makefile

install:
	install -d $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/bin
	install libs/c64/libc64-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib
	ln -f -s libc64-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libc64-emulator.so.1.0
	ln -f -s libc64-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libc64-emulator.so.1
	ln -f -s libc64-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libc64-emulator.so
	install libs/nes/libnes-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib
	ln -f -s libnes-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libnes-emulator.so.1.0
	ln -f -s libnes-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libnes-emulator.so.1
	ln -f -s libnes-emulator.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libnes-emulator.so
	install libs/famitracker/libfamitracker.so.1.0.0 $(DESTDIR)$(PREFIX)/lib
	ln -f -s libfamitracker.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libfamitracker.so.1.0
	ln -f -s libfamitracker.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libfamitracker.so.1
	ln -f -s libfamitracker.so.1.0.0 $(DESTDIR)$(PREFIX)/lib/libfamitracker.so
	install apps/nes-emulator/nes-emulator $(DESTDIR)$(PREFIX)/bin
	install apps/ide/famiplayer $(DESTDIR)$(PREFIX)/bin
	install apps/ide/famitracker $(DESTDIR)$(PREFIX)/bin
	install apps/ide/nesicide $(DESTDIR)$(PREFIX)/bin
	ldconfig

FORCE:
