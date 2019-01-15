#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  echo "Downloading Qt..."
  curl -Lo ~/qt-unified-windows-x86-online.exe http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe 2>&1
  echo "Done downloading."
elif [ "$1" == "install" ]; then
  echo "Running Qt installer..."
  ~/qt-unified-windows-x86-online.exe --script build/windows-qt-installer.qs 2>&1
  echo "Done installing Qt."
fi
