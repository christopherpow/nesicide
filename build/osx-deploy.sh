#!/bin/bash

DISTPATH=$PWD/nesicide-local
LIBDEPS="deps/rtmidi/release/librtmidi \
     deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     deps/qhexedit2/src/libqhexedit \
     libs/nes/release/libnes-emulator \
     libs/c64/release/libc64-emulator \
     libs/famitracker/release/libfamitracker"

DEPLOYS_SRC="apps/ide/release/nesicide.app \
        apps/famitracker/release/famitracker.app \
        apps/famiplayer/release/famiplayer.app \
        apps/nes-emulator/release/nes-emulator.app"

DEPLOYS_DEST="$DISTPATH/nesicide.app \
        $DISTPATH/famitracker.app \
        $DISTPATH/famiplayer.app \
        $DISTPATH/nes-emulator.app"

GIT_REV=`git rev-parse --short HEAD`

TARGARGS=-dmg

if [ "$1" == "local" ]; then
  rm -rf $DISTPATH
  mkdir $DISTPATH
  for DEPLOY in ${DEPLOYS_SRC}
  do
    cp -r ${DEPLOY} $DISTPATH/
  done
  for DEPLOY in ${DEPLOYS_DEST}
  do
    echo Deploying ${DEPLOY}
    if [ "$DEPLOY" == "$DISTPATH/nesicide.app" ]; then
      make -C deps/cc65/src all
      make -C deps/cc65/libsrc nes c64
      make -C deps/cc65 install PREFIX=${DEPLOY}/Contents/MacOS/cc65 
      cp -rv deps/uc65-release-0.5-rc6 ${DEPLOY}/Contents/MacOS/
      make -C tools/famitone2 all
      make -C tools/famitone2 install PREFIX=$DEPLOY/Contents/MacOS/famitone2/bin
    fi
    macdeployqt ${DEPLOY} ${TARGARGS}
    mv -v ${DEPLOY/%.app/.dmg} ${DEPLOY/%.app}-${GIT_REV}.dmg
  done
elif [ "$1" == "remote" ]; then
  rsync $DISTPATH/*.dmg cpow@162.243.126.83:/var/www/html/nesicide/media/downloads/
fi

