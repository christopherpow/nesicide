#!/bin/bash

#PATH=deps/Windows/GnuWin32/bin:$PATH
QTDIR=/c/Qt/5.11.3/mingw53_32
PATH=$PATH:$QTDIR/bin

ls -al /c/Qt/
ls -al /c/Qt/5.11.3/
ls -al /c/Qt/5.11.3/mingw53_32
ls -al /c/Qt/5.11.3/mingw53_32/bin

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
( cd build/ide; qmake; mingw32-make )
echo Building FamiTracker...
( cd build/famitracker; qmake; mingw32-make )
echo Building FamiPlayer...
( cd build/famiplayer; qmake; mingw32-make )
echo Building NES Emulator...
( cd build/nes-emulator; qmake; mingw32-make )
