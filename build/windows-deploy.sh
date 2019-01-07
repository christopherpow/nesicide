PATH=deps/Windows/GnuWin32/bin:$PATH

LIBDEPS="deps/rtmidi/release/rtmidi \
     deps/qscintilla2/Qt4Qt5/release/qscintilla2_qt5 \
     deps/Windows/SDL/SDL.dll \
     libs/nes/release/nes-emulator \
     libs/c64/release/c64-emulator \
     libs/famitracker/release/famitracker"

DEPLOYS_SRC="apps/ide/release/nesicide.exe \
        apps/famitracker/release/famitracker.exe \
        apps/famiplayer/release/famiplayer.exe \
        apps/nes-emulator/release/nes-emulator.exe"

DEPLOYS_DEST="./dist/nesicide.exe \
        ./dist/famitracker.exe \
        ./dist/famiplayer.exe \
        ./dist/nes-emulator.exe"

if "%1" == "local" (
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
  make -C deps/cc65; make -C deps/cc65 install prefix=$PWD/dist/cc65
  for DEPLOY in ${DEPLOYS_DEST}
  do
    DIST=$(basename $DEPLOY) 
    echo Deploying ${DIST}
    windeployqt ${DEPLOY} ${TARGARGS} -printsupport
  done
  tar cjvf nesicide-win-x64.tar.bz2 dist
) else (
  rsync $TRAVIS_BUILD_DIR/nesicide-win-x64.tar.bz2 cpow@162.243.126.83:/var/www/html/nesicide/
)
