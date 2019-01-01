#!/bin/bash

rsync --delete-after $TRAVIS_BUILD_DIR/build/dist/*.dmg cpow@162.243.126.83:var/www/html/nesicide/
rsync --delete-after $TRAVIS_BUILD_DIR/build/dist/*.AppImage cpow@162.243.126.83:var/www/html/nesicide/

