#!/bin/bash

TARGET=$(basename `echo $0 | cut -d'-' -f 1`)
TARGARGS=
LIBDIRECT=

LIBDEPS="../deps/rtmidi/release/librtmidi \
     ../deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     ../libs/nes/release/libnes-emulator \
     ../libs/c64/release/libc64-emulator \
     ../libs/famitracker/release/libfamitracker"

DEPLOYS="../apps/ide/release/nesicide.app \
     ../apps/famitracker/release/famitracker.app \
     ../apps/famiplayer/release/famiplayer.app \
     ../apps/nes-emulator/release/nes-emulator.app"
TARGARGS+=-dmg

for DEPLOY in ${DEPLOYS}
do
   echo Deploying ${DEPLOY}
   macdeployqt ${DEPLOY} ${TARGARGS}
done
tar cjvf nesicide-osx.tar.bz2 fami*.app nes*.app
