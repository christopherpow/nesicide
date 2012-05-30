#! /bin/bash

#
source="tmpdistdir"
backgroundPictureName="background.png"
title=nesicide
applicationName="${title}.app"
finalDMGName="${title}.dmg"
systemSDLPath="/Library/Frameworks"
filePath="../.."

if [ -e ${finalDMGName} ]; then
	echo "\"${finalDMGName}\" already exist, please remove first"
	exit -1
fi

#
mkdir -p ${source}/.background
cp ${backgroundPictureName} ${source}/.background/
if [ -e ${filePath}/release/${applicationName} ]; then
    indir=${filePath}/release/${applicationName}
elif [ -e ${filePath}/${applicationName} ]; then
    echo "Can't find ${filePath}/release/${applicationName}, using ${filePath}/${applicationName} instead"
    indir=${filePath}/${applicationName}
else
    echo "Can't find ${applicationName}, please build it first";
    rm -rf ${source}
    exit -1;
fi

echo "Copy ${indir}"
cp -r ${indir} ${source}/

cd ${source}
#strip bin
echo "Stripping binary from unnecessary Qt stuff"
macdeployqt ${applicationName}
#add sdl
echo "Adding SDL"
cp -R ${systemSDLPath}/SDL.framework ${applicationName}/Contents/Frameworks/SDL.framework
echo "Removing the SDL headers from ${applicationName}"
rm -rf ${applicationName}/Contents/Frameworks/Headers
rm -rf ${applicationName}/Contents/Frameworks/Versions/A/Headers
cd -

#fix libqscintilla paths
echo "Fixing lib paths for QScintilla"
install_name_tool -change "QtGui.framework/Versions/4/QtGui" \
   "@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui" \
   "$source/$applicationName/Contents/Frameworks/libqscintilla2.6.dylib"
install_name_tool -change "QtCore.framework/Versions/4/QtCore" \
   "@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore" \
   "$source/$applicationName/Contents/Frameworks/libqscintilla2.6.dylib"

#
size=200000
#size=$(du -s -k tmpdistdir | awk '{print $1}')
#size=$(stat -f%z "${source}")

#
echo "Creating temporary disk image"
hdiutil create -srcfolder "${source}" -volname "${title}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${size}k pack.temp.dmg

# wait for the file to be created...
while [ ! -e pack.temp.dmg ]; do
    # Sleep until file does exists/is created
    echo "Waiting for pack.temp.dmg"
    sleep 1
done

if [ -e "/Volumes/${title}" ]; then
    echo "A volume named ${title} is already mounted, unmounts it..."
    hdiutil eject /Volumes/${title}
    # wait untill the disk has been unmounted
    while [ -e "/Volumes/${title}" ]; do
        # Sleep until file does exists/is created
        echo "Waiting for ${title} to be unmounted..."
        sleep 1
    done
fi

#
echo "Attaching temporary disk image"
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')

# wait untill the disk has been mounted
while [ ! -e /Volumes/${title} ]; do
    # Sleep until file does exists/is created
    echo "Waiting for ${title} to be mounted"
    sleep 1
done 

#
echo "Creating folder design..."
echo '
   tell application "Finder"
     tell disk "'${title}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {400, 100, 880, 440}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 128
           set background picture of theViewOptions to file ".background:'${backgroundPictureName}'"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "'${applicationName}'" of container window to {100, 190}
           set position of item "Applications" of container window to {380, 190}
           close
           open
           update without registering applications
           delay 4
     end tell
   end tell
' | osascript

#
echo "Removing unnessary files created by Mac OS X"
rm -rf /Volumes/"${title}"/.Trashes
rm -rf /Volumes/"${title}"/.fseventsd

chmod -Rf go-w /Volumes/"${title}"
sync
sync
echo "Unmounts temporary disk image"
hdiutil detach -Force ${device}
while [ -e ${device} ]; do
    echo "Waiting for ${device} to be ejected"
    sleep 1
done
echo "Converts temporary disk image to the final compressed image"
sleep 10
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "${finalDMGName}"

echo "Removes temporary files"
rm -f pack.temp.dmg
rm -rf ${source}
