#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
  sudo apt-get update -qq

  #sudo add-apt-repository ppa:beineri/opt-qt-5.11.2-xenial -y
  #sudo apt-get update -qq
elif [ "$1" == "install" ]; then
  sudo apt-get -y install qtbase5-dev qtdeclarative5-dev libqt5webkit5-dev libsqlite3-dev
  sudo apt-get -y install qt5-default qttools5-dev-tools
  #sudo apt-get -y install qt511base
  #sudo apt-get -y install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev
  source /opt/qt*/bin/qt*-env.sh
  exit 0
fi
