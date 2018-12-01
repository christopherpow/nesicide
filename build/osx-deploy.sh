#!/bin/bash

LIBDEPS="../deps/rtmidi/release/librtmidi \
     ../deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     ../libs/nes/release/libnes-emulator \
     ../libs/c64/release/libc64-emulator \
     ../libs/famitracker/release/libfamitracker"

DEPLOYS_SRC="../apps/ide/release/nesicide.app \
        ../apps/famitracker/release/famitracker.app \
        ../apps/famiplayer/release/famiplayer.app \
        ../apps/nes-emulator/release/nes-emulator.app"

DEPLOYS_DEST="./dist/nesicide.app \
        ./dist/famitracker.app \
        ./dist/famiplayer.app \
        ./dist/nes-emulator.app"

TARGARGS=-dmg

rm -rf ./dist
mkdir ./dist
for DEPLOY in ${DEPLOYS_SRC}
do
   cp -r ${DEPLOY} ./dist/
done
for DEPLOY in ${DEPLOYS_DEST}
do
   echo Deploying ${DEPLOY}
   macdeployqt ${DEPLOY} -verbose=2 ${TARGARGS}
done
ls -al ./dist/
