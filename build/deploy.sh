#!/bin/bash

TARGET=$(basename `echo $0 | cut -d'-' -f 1`)
TARGARGS=
LIBDIRECT=

LIBDEPS="../deps/rtmidi/release/librtmidi \
     ../deps/qscintilla2/Qt4Qt5/libqscintilla2_qt5 \
     ../libs/nes/release/libnes-emulator \
     ../libs/c64/release/libc64-emulator \
     ../libs/famitracker/release/libfamitracker"

if [ "$TARGET" == 'osx' ]; then
   DEPLOYS="../apps/ide/release/nesicide.app \
        ../apps/famitracker/release/famitracker.app \
        ../apps/famiplayer/release/famiplayer.app \
        ../apps/nes-emulator/release/nes-emulator.app"
   TARGARGS+=-dmg
else
   DEPLOYS="../apps/ide/release/nesicide \
        ../apps/famitracker/release/famitracker \
        ../apps/famiplayer/release/famiplayer \
        ../apps/nes-emulator/release/nes-emulator"
fi

if [ "$TARGET" == 'linux' ]; then
   if [ ! -f "./linuxdeployqt-continuous-x86_64.AppImage" ]; then
      wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
      chmod a+x linuxdeployqt*.AppImage
   fi
   unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH

   TARGARGS+="-verbose=0 -appimage"
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
      cp -v ${DIST}.desktop ./dist
      cp -v ${DIST}.png ./dist
      ./linuxdeployqt-continuous-x86_64.AppImage ${DIST}.desktop ${TARGARGS}
   done
   tar cjvf nesicide-linux.tar.bz2 fami*.AppImage nes*.AppImage
   curl --upload-file nesicide-linux.tar.bz2 https://knob.phreneticappsllc.com/nesicide/
elif [ "$TARGET" == 'osx' ]; then
   for DEPLOY in ${DEPLOYS}
   do
      echo Deploying ${DEPLOY}
      macdeployqt ${DEPLOY} ${TARGARGS}
   done
   tar cjvf nesicide-osx.tar.bz2 fami*.App nes*.App
   curl --upload-file nesicide-osx.tar.bz2 https://knob.phreneticappsllc.com/nesicide/
else
   for DEPLOY in ${DEPLOYS}
   do
      echo Deploying ${DEPLOY}
      macdeployqt ${DEPLOY} ${TARGARGS}
   done
   tar cjvf nesicide-${TARGET}.tar.bz2 famitracker famiplayer nesicide nes-emulator
   curl --upload-file nesicide-${TARGET}.tar.bz2 https://knob.phreneticappsllc.com/nesicide/
fi

