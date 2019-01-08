#!/bin/bash
set -ev

PATH=deps/Windows/GnuWin32/bin:$PATH

# check for deps
echo -n "Checking for dependencies package..."
if [ ! -d "deps" ]; then
   echo "Fetching dependencies package..."
   wget https://knob.phreneticappsllc.com/nesicide/nesicide-deps.tar.bz2
   echo "Extracting dependencies package..."
   tar --help
   tar xjf --checkpoint=1000 --checkpoint-action=dot nesicide-deps.tar.bz2
   rm -f nesicide-deps.tar.bz2
else
   echo "found."
fi
# add CONFIG+=debug to qmake to build debug.
echo Building NESICIDE...
( cd build/ide; qmake; make )
echo Building FamiTracker...
( cd build/famitracker; qmake; make )
echo Building FamiPlayer...
( cd build/famiplayer; qmake; make )
echo Building NES Emulator...
( cd build/nes-emulator; qmake; make )

