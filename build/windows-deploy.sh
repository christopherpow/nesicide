PATH=deps/Windows/GnuWin32/bin:$PATH

DISTPATH=$PWD/nesicide-local
REMOTEPATH=nesicide-win-x86
if [ "$APPVEYOR_REPO_COMMIT" != "" ]; then
     COMMIT_SHORT=${APPVEYOR_REPO_COMMIT:0:7}
fi
if [ "$COMMIT_SHORT" != "" ]; then
     DISTPATH=$PWD/nesicide-$COMMIT_SHORT
     REMOTEPATH+=-$COMMIT_SHORT
fi

REMOTEPATH+=.tar.bz2

LIBDEPS="deps/rtmidi/release/rtmidi \
     deps/qscintilla2/Qt4Qt5/release/qscintilla2_qt5 \
     deps/qhexedit2/lib/qhexedit \
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
  make -C deps/cc65 install PREFIX=$DISTPATH/cc65
  cp -rv deps/uc65-release-0.5-rc6 $DISTPATH/
  make -C tools/famitone2 all
  make -C tools/famitone2 install PREFIX=$DISTPATH/famitone2/bin

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
