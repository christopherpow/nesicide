PATH=deps/Windows/GnuWin32/bin:$PATH

DISTPATH=./nesicide-local
REMOTEPATH=nesicide-win-x86
if [ "$APPVEYOR_REPO_COMMIT" != "" ]; then
     DISTPATH=./nesicide-$APPVEYOR_REPO_COMMIT
     REMOTEPATH+=-$APPVEYOR_REPO_COMMIT
fi

REMOTEPATH+=.tar.bz2

LIBDEPS="deps/rtmidi/release/rtmidi \
     deps/qscintilla2/Qt4Qt5/release/qscintilla2_qt5 \
     deps/Windows/Lua/lua51.dll \
     libs/nes/release/nes-emulator \
     libs/c64/release/c64-emulator \
     libs/famitracker/release/famitracker"

#OSTYPE=`wmic os get osarchitecture`
#if [[ $OSTYPE =~ .*64*. ]]; then
#	LIBDEPS+=" deps/Windows/SDL/x64/SDL.dll"
#else
	LIBDEPS+=" deps/Windows/SDL/x86/SDL.dll"
#fi

DEPLOYS_SRC="apps/ide/release/nesicide.exe \
        apps/famitracker/release/famitracker.exe \
        apps/famiplayer/release/famiplayer.exe \
        apps/nes-emulator/release/nes-emulator.exe"

DEPLOYS_DEST="$DISTPATH/nesicide.exe \
        $DISTPATH/famitracker.exe \
        $DISTPATH/famiplayer.exe \
        $DISTPATH/nes-emulator.exe"

if [ "$1" == "local" ]; then
  rm -rf $DISTPATH
  mkdir -pv $DISTPATH
  for DEPLOY in ${DEPLOYS_SRC}
  do
    cp -v ${DEPLOY} $DISTPATH/
  done
  for f in ${LIBDEPS}
  do 
    cp -v ${f}* $DISTPATH/ 
  done
  make -C deps/cc65/src all
  make -C deps/cc65/libsrc nes c64
  make -C deps/cc65 install PREFIX=$PWD/$DISTPATH/cc65
  for DEPLOY in ${DEPLOYS_DEST}
  do
    DIST=$(basename $DEPLOY) 
    echo Deploying ${DIST}
    windeployqt ${DEPLOY} ${TARGARGS} -printsupport
  done
  tar cjvf $REMOTEPATH $DISTPATH
elif [ "$1" == "remote" ]; then
  echo "Nothing to do here. Plese disperse!"
fi
