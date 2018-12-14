#!/bin/bash

# check for deps
echo -n "Checking for dependencies package..."
if [ ! -d "../deps" ]; then
   echo "Fetching dependencies package..."
   wget https://knob.phreneticappsllc.com/nesicide/nesicide-deps.tar.bz2
   echo "Extracting dependencies package..."
   tar xjf nesicide-deps.tar.bz2 -C ..
   rm -f nesicide-deps.tar.bz2
else
   echo "found."
fi
# add CONFIG+=debug to qmake to build debug.
echo Building NESICIDE...
( cd ide; qmake -qt=qt5 -v; make )
echo Building FamiTracker...
( cd famitracker; qmake; make )
echo Building FamiPlayer...
( cd famiplayer; qmake; make )
echo Building NES Emulator...
( cd nes-emulator; qmake; make )

