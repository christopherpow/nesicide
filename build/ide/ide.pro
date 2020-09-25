TEMPLATE = subdirs

SUBDIRS = c64 nes rtmidi famitracker qscintilla2 qhexedit2 ide

c64.file = ../../libs/c64/c64-emulator-lib.pro
nes.file = ../../libs/nes/nes-emulator-lib.pro
famitracker.file = ../../libs/famitracker/famitracker-lib.pro
rtmidi.file = ../../deps/rtmidi/rtmidi.pro
qscintilla2.file = ../../deps/qscintilla2/Qt4Qt5/qscintilla.pro
qhexedit2.file = ../../deps/qhexedit2/src/qhexedit.pro
ide.file = ../../apps/ide/nesicide.pro

famitracker.depends = rtmidi
ide.depends = c64 nes famitracker qscintilla2 qhexedit2

RESOURCES += \
    ../../common/resource.qrc
