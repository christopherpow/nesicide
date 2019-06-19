#!/bin/bash
set -v

if [ "$1" == "before_install" ]; then
  echo "Nothing to do before_install..."
elif [ "$1" == "install" ]; then
  echo "Getting make..."
fi
