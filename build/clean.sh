#!/bin/bash

function really_clean() {
  for f in apps/{ide,fami*,nes*} libs/{nes,c64,fami*} deps/{rtmidi,qscintilla2/Qt4Qt5}; do echo $f; ( rm -rfv $f/debug $f/release $f/Makefile* $f/.qmake* ); done
}

echo Cleaning NESICIDE...
( cd build/ide; make distclean )
echo Cleaning FamiTracker...
( cd build/famitracker; make distclean )
echo Cleaning FamiPlayer...
( cd build/famiplayer; make distclean )
echo Cleaning NES Emulator...
( cd build/nes-emulator; make distclean )
echo Making sure all build debris is GONE...
really_clean

if [ "$1" == "deps" ]; then
  echo Removing deps...
  rm -rf deps
fi
find . -name 'Makefile.*' -exec rm {} ';'
