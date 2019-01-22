#!/bin/bash

LIBDEPS="deps/rtmidi/release/librtmidi \
     deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     libs/nes/release/libnes-emulator \
     libs/c64/release/libc64-emulator \
     libs/famitracker/release/libfamitracker"

DEPLOYS_SRC="apps/ide/release/nesicide.app \
        apps/famitracker/release/famitracker.app \
        apps/famiplayer/release/famiplayer.app \
        apps/nes-emulator/release/nes-emulator.app"

DEPLOYS_DEST="./dist/nesicide.app \
        ./dist/famitracker.app \
        ./dist/famiplayer.app \
        ./dist/nes-emulator.app"

GIT_REV=`git rev-parse --short HEAD`

TARGARGS=-dmg

if [ "$1" == "local" ]; then
  rm -rf ./dist
  mkdir ./dist
  for DEPLOY in ${DEPLOYS_SRC}
  do
    cp -r ${DEPLOY} ./dist/
  done
  for DEPLOY in ${DEPLOYS_DEST}
  do
    echo Deploying ${DEPLOY}
    if [ "$DEPLOY" == "./dist/nesicide.app" ]; then
      make -C deps/cc65/src all
      make -C deps/cc65/libsrc TARGETS="nes c64"
      make -C deps/cc65 install prefix=${DEPLOY}/Contents/MacOS/cc65 
    fi
    macdeployqt ${DEPLOY} ${TARGARGS}
    mv -v ${DEPLOY/%.app/.dmg} ${DEPLOY/%.app}-${GIT_REV}.dmg
  done
elif [ "$1" == "remote" ]; then
  rsync $TRAVIS_BUILD_DIR/dist/*.dmg cpow@162.243.126.83:/var/www/html/nesicide/media/downloads/
fi

