PATH=deps/Windows/GnuWin32/bin:$PATH

LIBDEPS="deps/rtmidi/release/rtmidi \
     deps/qscintilla2/Qt4Qt5/release/qscintilla2_qt5 \
     deps/Windows/Lua/lua51.dll \
     libs/nes/release/nes-emulator \
     libs/c64/release/c64-emulator \
     libs/famitracker/release/famitracker"

OSTYPE=`wmic os get osarchitecture`
#if [[ $OSTYPE =~ .*64*. ]]; then
#	LIBDEPS+=" deps/Windows/SDL/x64/SDL.dll"
#else
	LIBDEPS+=" deps/Windows/SDL/x86/SDL.dll"
#fi

DEPLOYS_SRC="apps/ide/release/nesicide.exe \
        apps/famitracker/release/famitracker.exe \
        apps/famiplayer/release/famiplayer.exe \
        apps/nes-emulator/release/nes-emulator.exe"

DEPLOYS_DEST="./dist/nesicide.exe \
        ./dist/famitracker.exe \
        ./dist/famiplayer.exe \
        ./dist/nes-emulator.exe"

if [ "$1" == "local" ]; then
  rm -rf ./dist
  mkdir -pv ./dist
  for DEPLOY in ${DEPLOYS_SRC}
  do
    cp -v ${DEPLOY} ./dist/
  done
  for f in ${LIBDEPS}
  do 
    cp -v ${f}* dist/ 
  done
  make -C deps/cc65/src all
  make -C deps/cc65/libsrc nes c64
  make -C deps/cc65 install PREFIX=$PWD/dist/cc65
  for DEPLOY in ${DEPLOYS_DEST}
  do
    DIST=$(basename $DEPLOY) 
    echo Deploying ${DIST}
    windeployqt ${DEPLOY} ${TARGARGS} -printsupport
  done
  tar cjvf nesicide-win-x86.tar.bz2 dist
elif [ "$1" == "remote" ]; then
  echo "BinTray"
fi
