TEMPLATE = subdirs

SUBDIRS = rtmidi famitracker-lib famitracker-app

rtmidi.file = ../../deps/rtmidi/rtmidi.pro
famitracker-lib.file = ../../libs/famitracker/famitracker-lib.pro
famitracker-app.file = ../../apps/famitracker/famitracker-app.pro

famitracker-lib.depends = rtmidi
famitracker-app.depends = famitracker-lib
