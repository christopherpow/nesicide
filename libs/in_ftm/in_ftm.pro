#-------------------------------------------------
#
# Project created by QtCreator 2015-12-19T08:58:54
#
#-------------------------------------------------

TARGET = in_ftm
TEMPLATE = lib

QT += core gui

greaterThan(QT_MAJOR_VERSION,4) {
    QT += widgets
}

TOP = ../..

macx {
    MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
    if( !exists( $$MAC_SDK) ) {
        error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
    }
    macx:QMAKE_MAC_SDK = macosx10.11
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

win32 {
   DEPENDENCYROOTPATH = $$TOP/deps
   DEPENDENCYPATH = $$DEPENDENCYROOTPATH/Windows
}
mac {
   DEPENDENCYROOTPATH = $$TOP/deps
   DEPENDENCYPATH = $$DEPENDENCYROOTPATH/osx
}
unix:!mac {
   DEPENDENCYROOTPATH = $$TOP/deps
   DEPENDENCYPATH = $$DEPENDENCYROOTPATH/linux
}

DEFINES -= UNICODE
DEFINES += NOMINMAX NULL=0

WINAMP_SDK_BASE = "C:/Program Files (x86)/Winamp SDK/Winamp"
WINAMP_PLUGIN_BASE = "C:/Program Files (x86)/Winamp/Plugins"

FAMITRACKER_LIBS = -L$$TOP/libs/famitracker/$$DESTDIR -lfamitracker
FAMITRACKER_CXXFLAGS = -I$$TOP/libs/famitracker

QMAKE_CXXFLAGS += $$FAMITRACKER_CXXFLAGS
QMAKE_LFLAGS += $$FAMITRACKER_LFLAGS
LIBS += $$FAMITRACKER_LIBS

INCLUDEPATH += $$WINAMP_SDK_BASE \
               $$TOP/common \
               $$TOP/libs/famitracker

SOURCES += \
    in_ftm.c \
    ftm_iface.cpp \
    mainwindow.cpp

HEADERS += \
    ftm_iface.h \
    mainwindow.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    mainwindow.ui
