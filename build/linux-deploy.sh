#!/bin/bash

# Find Qt
. /opt/qt510/bin/qt510-env.sh

DISTPATH=$PWD/nesicide-local
LIBDEPS="deps/rtmidi/release/librtmidi \
     deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     libs/nes/release/libnes-emulator \
     libs/c64/release/libc64-emulator \
     libs/famitracker/release/libfamitracker"

DEPLOYS="apps/ide/release/nesicide \
        apps/famitracker/release/famitracker \
        apps/famiplayer/release/famiplayer \
        apps/nes-emulator/release/nes-emulator"

TARGARGS="-verbose=0 -appimage -qmake=/opt/qt510/bin/qmake"

unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH

if [ "$1" == "local" ]; then
  if [ ! -f "./linuxdeployqt-continuous-x86_64.AppImage" ]; then
    wget -q -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
    chmod a+x linuxdeployqt*.AppImage
  fi

  for DEPLOY in ${DEPLOYS}
  do
    DIST=$(basename $DEPLOY) 
    echo Deploying ${DIST}
    rm -rf $DISTPATH
    mkdir -pv $DISTPATH
    cp -v ${DEPLOY} $DISTPATH/
    for f in ${LIBDEPS}
    do 
      sudo cp -v ${f}* /usr/lib/x86_64-linux-gnu/
    done
    if [ "$DEPLOY" == "apps/ide/release/nesicide" ]; then
      make -C deps/cc65/src all
      make -C deps/cc65/libsrc nes c64
      make -C deps/cc65 install PREFIX=$DISTPATH/cc65
      cp -rv deps/uc65-release-0.5-rc6 $DISTPATH/
      make -C tools/famitone2 all
      make -C tools/famitone2 install PREFIX=$DISTPATH/famitone2
    fi
    cp -v build/${DIST}.desktop $DISTPATH
    cp -v build/${DIST}.png $DISTPATH
    ./linuxdeployqt-continuous-x86_64.AppImage $DISTPATH/${DIST}.desktop ${TARGARGS}
  done
elif [ "$1" == "remote" ]; then
  rsync {fami,nes}*.AppImage cpow@162.243.126.83:/var/www/html/nesicide/media/downloads/
fi

exit 0
