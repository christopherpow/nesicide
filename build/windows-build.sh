#!/bin/bash

PATH=deps/Windows/GnuWin32/bin:$PATH
PATH=$PATH:"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.12.25827\\bin\\Hostx64\\x64"
QTDIR=/c/Qt/5.12.2/mingw73_64
PATH=$PATH:$QTDIR/bin:/c/MinGW/bin

# check for deps
echo -n "Checking for dependencies package..."
if [ ! -d "deps" ]; then
   echo "Fetching dependencies package..."
   curl -fsS -o nesicide-deps.tar.bz2 https://knob.phreneticappsllc.com/nesicide/nesicide-deps.tar.bz2 --insecure
   echo "Extracting dependencies package..."
   tar --checkpoint=100 --checkpoint-action=dot -xjf nesicide-deps.tar.bz2
   rm -f nesicide-deps.tar.bz2
else
   echo "found."
fi
# add CONFIG+=debug to qmake to build debug.
echo Building NESICIDE...
( cd build/ide; qmake -spec win32-msvc; cmake )
echo Building FamiTracker...
( cd build/famitracker; qmake -spec win32-msvc; make )
echo Building FamiPlayer...
( cd build/famiplayer; qmake -spec win32-msvc; make )
echo Building NES Emulator...
( cd build/nes-emulator; qmake -spec win32-msvc; make )
