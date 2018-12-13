#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
#  sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
#  sudo apt-get update -qq

  sudo add-apt-repository -y ppa:beineri/opt-qt-5.10.1-xenial
  sudo apt-get -qy update 
  sudo apt purge qt4*

elif [ "$1" == "install" ]; then
  sudo apt-get -y install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev
#  sudo apt-get -y install qtbase5-dev qtdeclarative5-dev libqt5webkit5-dev libsqlite3-dev
#  sudo apt-get -y install qt5-default qttools5-dev-tools
  sudo apt-get -qy install qt510base qt510xmlpatterns
  export QMAKESPEC=linux-g++
  . /opt/qt510/bin/qt510-env.sh
fi
