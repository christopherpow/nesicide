#-------------------------------------------------
#
# Project created by QtCreator 2010-08-02T22:13:12
#
#-------------------------------------------------

TARGET = "c64-emulator"

TEMPLATE = lib

# Remove Qt libraries
QT =

TOP = ../..

macx {
    QMAKE_MAC_SDK = macosx10.14
}

CONFIG(release, debug|release) {
   DESTDIR = release
} else {
   DESTDIR = debug
}

# Remove crap we don't need!
CONFIG -= rtti exceptions

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

unix:!mac {
   PREFIX = $$(PREFIX)
   isEmpty (PREFIX) {
      PREFIX = /usr/local
   }

   BINDIR = $$(BINDIR)
   isEmpty (BINDIR) {
      BINDIR=$$PREFIX/lib
   }

   target.path = $$BINDIR
   INSTALLS += target
}

INCLUDEPATH += . \
               ./common \
               ./emulator \
               $$TOP/common

SOURCES += \
   $$TOP/common/cbreakpointinfo.cpp \
   c64_emulator_core.cpp \
   emulator/cc646502.cpp \
   emulator/cc64breakpointinfo.cpp \
   common/cc64systempalette.cpp \
   emulator/cc64sid.cpp

HEADERS +=\
   c64_emulator_core.h \
   emulator/cc646502.h \
   emulator/cc64breakpointinfo.h \
   common/cc64systempalette.h \
   emulator/cc64sid.h
