#!/bin/bash

TARGET=$(basename `echo $0 | cut -d'-' -f 1`)
TARGARGS=

if [ "$TARGET" == 'mac' ]; then
   TARGARGS+=-dmg
fi

echo Deploying NESICIDE...
${TARGET}deployqt ../apps/ide/release/nesicide.app ${TARGARGS} 
echo Deploying FamiTracker...
${TARGET}deployqt ../apps/famitracker/release/famitracker.app ${TARGARGS}
echo Deploying FamiPlayer...
${TARGET}deployqt ../apps/famiplayer/release/famiplayer.app ${TARGARGS}
echo Deploying NES Emulator...
${TARGET}deployqt ../apps/nes-emulator/release/nes-emulator.app ${TARGARGS}

