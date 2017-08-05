TEMPLATE = subdirs

SUBDIRS = rtmidi famitracker-lib famiplayer-app

rtmidi.file = ../../deps/rtmidi/rtmidi.pro
famitracker-lib.file = ../../libs/famitracker/famitracker-lib.pro
famiplayer-app.file = ../../apps/famiplayer/famiplayer.pro

famitracker-lib.depends = rtmidi
famiplayer-app.depends = famitracker-lib
