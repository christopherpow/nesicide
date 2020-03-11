# nesicide [![Build Status](https://travis-ci.org/christopherpow/nesicide.svg?branch=master)](https://travis-ci.org/christopherpow/nesicide) [![Build status](https://ci.appveyor.com/api/projects/status/284fj1df8dur76g5?svg=true)](https://ci.appveyor.com/project/christopherpow/nesicide)

nesicide is an Integrated Development Environment (IDE) for the 8-bit Nintendo Entertainment System (NES).

## Products in Project

This project contains the following products:

* NESICIDE (the IDE itself)
* A NES emulator (standalone package of the emulator used in the IDE)
* Qt FamiTracker (a fork of jsr's FamiTracker)
* FamiPlayer (a music player for .ftm files)

## Prerequisites

The following steps are required prior to building this project on any platform.

0. Install `git`
1. Install [Qt 5.12.6](http://download.qt.io/archive/qt/5.12/5.12.6/) as it was the latest kit to be verified to build NESICIDE properly without errors. Please do not use a kit later than this, or if you do, please create a pull request with necessary changes.
2. Make sure qmake is in your PATH.

At the time of writing this Mac OS Homewbrew contains Qt 5.14.1 and NESICIDE can also be built with that version of Qt. So, alternatively, you can install Qt using

   ```
   brew intall qt5
   ```

### Debian prerequisites

```sh
sudo apt-get install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev
```

### Arch Linux prerequisites

```sh
sudo pacman -S lua sdl mesa wine
```

### Windows prerequisites

Install [GnuWin32](https://sourceforge.net/projects/getgnuwin32/) and [wget](http://gnuwin32.sourceforge.net/packages/wget.htm).

## Building

This project builds for Linux, macOS, and Windows. To perform a build for any supported platform follow the instructions for the platform below.

> NOTE: The build process takes a while, so grab a snack while you wait. ;)

### Linux

0. `./build/linux-build.sh`
1. `./build/linux-deploy.sh local`

### macOS

If you installed Qt from Homebrew you need to set the following environment variable so that the build process can find qmake (the qt5 Homebrew formula is keg-only):

```
PATH="/usr/local/opt/qt/bin:$PATH"
```

0. `./build/osx-build.sh`
1. `./build/osx-deploy.sh local`

### Windows

NOTE: The build process uses GnuWin32 tools and MinGW tools. Install these and make sure that `mingw32-make` and `wget` are on your path.

0. `./build/win-build.sh`
1. `./build/win-deploy.sh local`

## Running

The `deploy.sh` creates artifacts of the following form.

### Linux

Separate AppImage executables for each product of the project. See https://appimage.org/ for information on AppImage containers.

### macOS

Separate Apple Disk Images (DMGs) for each product of the project.

### Windows

Monolithic bzipped tarball containing all products of the project, extractable to anywhere.

Enjoy!
