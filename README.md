# nesicide [![Build Status](https://travis-ci.org/christopherpow/nesicide.svg?branch=master)](https://travis-ci.org/christopherpow/nesicide)

nesicide is an Integrated Development Environment (IDE) for the 8-bit Nintendo Entertainment System (NES).

This project contains:

* NESICIDE (the IDE itself)
* A NES emulator (standalone package of the emulator used in the IDE)
* Qt FamiTracker (a fork of jsr's FamiTracker)
* FamiPlayer (a music player for .ftm files)

## Prerequisites

The following steps are required prior to building this project on any platform.

0. Install `git`
1. Install [Qt 5.6.2](https://download.qt.io/official_releases/qt/5.6/5.6.2/) as it was the latest kit to be verified to build NESICIDE properly with not errors. Please do not use a kit later than this, or if you do, please create a pull request with necessary changes.

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

0. `cd build`
1. `./build.sh`
2. Once the build process is complete run: `./linux-deploy.sh`

### macOS

0. `cd build`
1. `./build.sh`
2. Once the build process is complete run: `./osx-deploy.sh`

### Windows

0. `cd build`

NOTE: The build process uses GnuWin32 tools and MinGW tools. Install these and make sure that `mingw32-make` and `wget` are on your path.

1. `./win-build.sh`

2. Once the build process is complete run: `./win-deploy.sh`

## Running

The `deploy.sh` creates a tarball containing all of the executables and dependent libraries. Extract the tarball to your location of choice and execute.

Enjoy!
