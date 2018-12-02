#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  sudo add-apt-repository ppa:beineri/opt-qt562-xenial -y
  sudo add-apt-repository ppa:beineri/opt-qt591-xenial -y
  sudo add-apt-repository ppa:beineri/opt-qt-5.10.1-xenial -y
  sudo apt-get update -qq
elif [ "$1" == "install" ]; then
  sudo apt-get -y install qt56base
  sudo apt-get -y install qt59base
  sudo apt-get -y install qt510base
  sudo apt-get -y install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev
  source /opt/qt*/bin/qt*-env.sh
  exit 0
fi
