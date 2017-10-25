[![Build Status](https://travis-ci.org/christopherpow/nesicide.svg?branch=master)](https://travis-ci.org/christopherpow/nesicide)

nesicide
========
nesicide is an Integrated Development Environment (IDE) for the 8-bit Nintendo Entertainment System (NES).

This project contains:
* NESICIDE (the IDE itself)
* A NES emulator (standalone package of the emulator used in the IDE)
* FamiTrackerQT (a fork of jsr's FamiTracker)
* FamiPlayer (a music player for .ftm files)

Prerequisites
=============
To be able to build NESICIDE you need to do the following:

0. Install `git`

1. Install `Qt 5.6.2` [https://download.qt.io/official_releases/qt/5.6/5.6.2/](https://download.qt.io/official_releases/qt/5.6/5.6.2/) as it was the latest kit to be verified to build NESICIDE properly with not errors. Please do not use a kit later than this, or if you do, please create a pull request with nexessary changes. 

Known issues:
    * C++11 support required

### Debian prerequisites
`sudo apt-get install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev`

### Arch-Linux prerequisites
`sudo pacman -S lua sdl mesa wine`

Building
========
To build the project follow the instructions below:

NOTE: The build process takes a while, so grab a snack while you wait. ;)

## Linux
0. `cd build`

1. `./build.sh`

2. Once the build process is complete run: `./linux-deploy.sh`

## MacOSX
0. `cd build`

1. `./build.sh`

2. Once the build process is complete run: `./osx-deploy.sh`

## Windows
0. `cd build`

1. `./build.sh`

2. Once the build process is complete run: `./win-deploy.sh`

Running
=======
The `deploy.sh` creates executables for all platforms at the following locations:

* NESICIDE:
`apps/ide/release/nesicide`

* NES Emulator:
`apps/nes-emulator/release/nes-emulator`

* FamiTrackerQT:
`apps/famitracker/release/famitracker`

* FamiPlayer:
`apps/famiplayer/release/famiplayer`

