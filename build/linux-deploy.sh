#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

# Find Qt
#. /opt/qt510/bin/qt510-env.sh

LIBDEPS="deps/rtmidi/release/librtmidi \
     deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     libs/nes/release/libnes-emulator \
     libs/c64/release/libc64-emulator \
     libs/famitracker/release/libfamitracker"

DEPLOYS="apps/ide/release/nesicide \
        apps/famitracker/release/famitracker \
        apps/famiplayer/release/famiplayer \
        apps/nes-emulator/release/nes-emulator"

TARGARGS="-verbose=0 -appimage -qmake=/usr/bin/qmake -unsupported-allow-new-glibc"

unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH
echo 'OI! - pass "local" for a local install'
if [ "$1" == "local" ]; then
  if [ ! -f "./linuxdeployqt-continuous-x86_64.AppImage" ]; then
    wget -q -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
    chmod a+x linuxdeployqt*.AppImage
  fi

  for DEPLOY in ${DEPLOYS}
  do
    DIST=$(basename $DEPLOY) 
    echo Deploying ${DIST}
    rm -rf ./dist
    mkdir -pv ./dist
    cp -v ${DEPLOY} ./dist/
    for f in ${LIBDEPS}
    do 
      sudo cp -v ${f}* /usr/lib/x86_64-linux-gnu/
    done
    if [ "$DEPLOY" == "apps/ide/release/nesicide" ]; then
      make -C deps/cc65/src all
      make -C deps/cc65/libsrc nes c64
      make -C deps/cc65 install PREFIX=$TRAVIS_BUILD_DIR/dist/cc65
    fi
    cp -v build/${DIST}.desktop ./dist
    cp -v build/${DIST}.png ./dist
    ./linuxdeployqt-continuous-x86_64.AppImage ./dist/${DIST}.desktop ${TARGARGS}
  done
elif [ "$1" == "remote" ]; then
  rsync $TRAVIS_BUILD_DIR/{fami,nes}*.AppImage cpow@162.243.126.83:/var/www/html/nesicide/media/downloads/
fi

echo 'done'
exit 0
