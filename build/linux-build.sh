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
( cd build/ide; qmake; make )
echo Building FamiTracker...
( cd build/nes-emulator; qmake; make )
echo Building FamiPlayer...
( cd build/famiplayer; qmake; make )
echo Building NES Emulator...
( cd build/famitracker; qmake; make )
