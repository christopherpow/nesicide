#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 762E3157
  sudo add-apt-repository -y ppa:beineri/opt-qt-5.10.1-xenial
  sudo apt-get -qy update 
  sudo apt purge qt4*

elif [ "$1" == "install" ]; then
  sudo apt-get -y install build-essential libasound2-dev liblua5.1-dev libsdl1.2-dev libgl1-mesa-dev wine-dev
  sudo apt-get -qy install qt510base qt510xmlpatterns
fi
