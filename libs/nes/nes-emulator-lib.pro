#-------------------------------------------------
#
# Project created by QtCreator 2010-08-02T22:13:12
#
#-------------------------------------------------

TARGET = "nes-emulator"

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

DEFINES += XML_SAVE_STATE

INCLUDEPATH += . \
               ./common \
               ./emulator \
               $$TOP/common

SOURCES += \
   $$TOP/common/cbreakpointinfo.cpp \
   emulator/cnesrommapper068.cpp \
   emulator/cnesrommapper065.cpp \
   emulator/cnesrommapper011.cpp \
   emulator/cnesrommapper010.cpp \
   emulator/cnesrommapper009.cpp \
   emulator/cnesrommapper007.cpp \
   emulator/cnesrommapper005.cpp \
   emulator/cnesrommapper004.cpp \
   emulator/cnesrommapper003.cpp \
   emulator/cnesrommapper002.cpp \
   emulator/cnesrommapper001.cpp \
   emulator/cnesrom.cpp \
   emulator/cnesppu.cpp \
   emulator/cnesmappers.cpp \
   emulator/cnesio.cpp \
   emulator/cnesapu.cpp \
   emulator/cnes.cpp \
   emulator/cnes6502.cpp \
   common/cnessystempalette.cpp \
   nes_emulator_core.cpp \
   emulator/cmarker.cpp \
   emulator/cjoypadlogger.cpp \
   emulator/ccodedatalogger.cpp \
   emulator/ctracer.cpp \
   emulator/cnesbreakpointinfo.cpp \
   emulator/cnesios.cpp \
   emulator/cnesrommapper033.cpp \
   emulator/cnesrommapper069.cpp \
   emulator/cnesrommapper075.cpp \
   emulator/cnesrommapper034.cpp \
    emulator/cnesrommapper028.cpp \
    emulator/cnesrommapper013.cpp \
    emulator/cnesrommapper022.cpp \
    emulator/cnesrommapper023.cpp \
    emulator/cnesrommapper021.cpp \
    emulator/cnesrommapper024.cpp \
    emulator/cnesrommapper026.cpp \
    emulator/cnesrommapper025.cpp \
    emulator/cnesrommapper019.cpp \
    emulator/cnesrommapper018.cpp \
    emulator/cnesrommapper073.cpp \
    emulator/cnesrommapper016.cpp

HEADERS +=\
   emulator/cnesrommapper068.h \
   emulator/cnesrommapper065.h \
   emulator/cnesrommapper011.h \
   emulator/cnesrommapper010.h \
   emulator/cnesrommapper009.h \
   emulator/cnesrommapper007.h \
   emulator/cnesrommapper005.h \
   emulator/cnesrommapper004.h \
   emulator/cnesrommapper003.h \
   emulator/cnesrommapper002.h \
   emulator/cnesrommapper001.h \
   emulator/cnesrom.h \
   emulator/cnesppu.h \
   emulator/cnesmappers.h \
   emulator/cnesio.h \
   emulator/cnesapu.h \
   emulator/cnes.h \
   emulator/cnes6502.h \
   nes_emulator_core.h \
   common/cnessystempalette.h \
   emulator/cmarker.h \
   emulator/cjoypadlogger.h \
   emulator/ccodedatalogger.h \
   emulator/ctracer.h \
   emulator/cnesios.h \
   emulator/cnesrommapper033.h \
   emulator/cnesrommapper069.h \
   emulator/cnesrommapper075.h \
   emulator/cnesbreakpointinfo.h \
   emulator/cnesrommapper034.h \
    emulator/cnesrommapper028.h \
    emulator/cnesrommapper013.h \
    emulator/cnesrommapper022.h \
    emulator/cnesrommapper023.h \
    emulator/cnesrommapper021.h \
    emulator/cnesrommapper024.h \
    emulator/cnesrommapper026.h \
    emulator/cnesrommapper025.h \
    emulator/cnesrommapper019.h \
    emulator/cnesrommapper018.h \
    emulator/cnesrommapper073.h \
    emulator/cnesrommapper016.h
