#-------------------------------------------------
#
# Project created by QtCreator 2015-08-08T12:58:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = rtmidi
TEMPLATE = lib

TOP = ../..

macx {
    MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk
    if( !exists( $$MAC_SDK) ) {
        error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
    }
    macx:QMAKE_MAC_SDK = macosx10.10
}

CONFIG(release, debug|release) {
   DESTDIR = release
} else {
   DESTDIR = debug
}

OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR

DEFINES += RTMIDI_LIBRARY \
           __MACOSX_CORE__

SOURCES += RtMidi.cpp

HEADERS += RtMidi.h

LIBS +=  -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

unix {
    target.path = /usr/lib
    INSTALLS += target
}
