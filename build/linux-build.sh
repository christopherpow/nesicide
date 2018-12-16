#!/bin/bash

# Find Qt
. /opt/qt510/bin/qt510-env.sh

# check for deps
echo -n "Checking for dependencies package..."
if [ ! -d "deps" ]; then
   echo "Fetching dependencies package..."
   wget https://knob.phreneticappsllc.com/nesicide/nesicide-deps.tar.bz2
   echo "Extracting dependencies package..."
   tar xjf nesicide-deps.tar.bz2 
   rm -f nesicide-deps.tar.bz2
else
   echo "found."
fi

# add CONFIG+=debug to qmake to build debug.
echo Building NESICIDE...
qmake build/ide/ide.pro; make -C build/ide -f Makefile
echo Building FamiTracker...
qmake build/famitracker/famitracker.pro; make -C build/famitracker -f Makefile
echo Building FamiPlayer...
qmake build/famiplayer/famiplayer.pro; make -C build/famiplayer -f Makefile
echo Building NES Emulator...
qmake build/nes-emulator/nes-emulator.pro; make -C build/nes-emulator -f Makefile
