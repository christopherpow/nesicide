#!/bin/bash

ls -al $TRAVIS_BUILD_DIR
ls -al $TRAVIS_BUILD_DIR/build
ls -al $TRAVIS_BUILD_DIR/dist

rsync $TRAVIS_BUILD_DIR/dist/*.dmg cpow@162.243.126.83:/var/www/html/nesicide/
rsync $TRAVIS_BUILD_DIR/dist/*.AppImage cpow@162.243.126.83:/var/www/html/nesicide/

