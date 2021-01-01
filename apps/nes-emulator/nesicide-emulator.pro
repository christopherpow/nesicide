#-------------------------------------------------
#
# Project created by QtCreator 2010-07-27T21:40:46
#
#-------------------------------------------------

QT += core \
   gui \
   opengl \
   xml

# Qt 5.5 requires this?!
win32: LIBS += -lopengl32

greaterThan(QT_MAJOR_VERSION,4) {
    QT += widgets
}

TOP = ../..

#macx {
#    QMAKE_MAC_SDK = macosx10.15
#}

CONFIG(release, debug|release) {
   DESTDIR = release
} else {
   DESTDIR = debug
}

# Remove crap we do not need!
CONFIG -= rtti exceptions

OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR

DEFINES -= UNICODE
DEFINES += XML_SAVE_STATE

TARGET = "nes-emulator"

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

TEMPLATE = app

# set platform specific cxxflags and libs
#########################################

NESICIDE_CXXFLAGS = -I$$TOP/libs/nes -I$$TOP/libs/nes/emulator
NESICIDE_LIBS = -L$$TOP/libs/nes/$$DESTDIR -lnes-emulator

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

win32 {
    contains(QT_ARCH, i386) {
        arch = x86
    } else {
        arch = x64
    }

   SDL_CXXFLAGS = -I$$DEPENDENCYPATH/SDL
   SDL_LIBS =  -L$$DEPENDENCYPATH/SDL/$$arch -lsdl

   QMAKE_LFLAGS += -static-libgcc
}

mac {
   SDL_CXXFLAGS = -I$$DEPENDENCYPATH/SDL.framework/Headers
   SDL_LIBS = -F$$DEPENDENCYPATH -framework SDL

   ICON = $$TOP/common/resources/controller.icns

   QMAKE_PRE_LINK += mkdir -p $$DESTDIR/$${TARGET}.app/Contents/Frameworks $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$TOP/libs/nes/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnes-emulator.1.dylib \
       @executable_path/../Frameworks/libnes-emulator.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/nes-emulator $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $$DESTDIR/$${TARGET}.app/Contents/MacOS/nes-emulator $$escape_expand(\n\t)
}

unix:!mac {
   SDL_CXXFLAGS = $$system(sdl-config --cflags)
   SDL_LIBS = $$system(sdl-config --libs)

   PREFIX = $$(PREFIX)
   isEmpty (PREFIX) {
      PREFIX = /usr/local
   }

   BINDIR = $$(BINDIR)
   isEmpty (BINDIR) {
      BINDIR=$$PREFIX/bin
   }

   target.path = $$BINDIR
   INSTALLS += target
}

QMAKE_CXXFLAGS += $$NESICIDE_CXXFLAGS $$SDL_CXXFLAGS
LIBS += $$NESICIDE_LIBS $$SDL_LIBS

INCLUDEPATH += \
   $$TOP/common \
   common \
   emulator \
   interfaces \
   project

SOURCES += \
   $$TOP/common/xmlhelpers.cpp \
   main.cpp \
   mainwindow.cpp \
   project/ccartridge.cpp \
   aboutdialog.cpp \
   emulator/nesemulatorthread.cpp \
   $$TOP/common/emulatorprefsdialog.cpp \
   qkeymapitemedit.cpp \
   $$TOP/common/version.cpp \
   emulator/nesemulatorrenderer.cpp \
   common/emulatorcontrol.cpp \
   emulator/nesemulatordockwidget.cpp \
   $$TOP/common/appeventfilter.cpp \
   $$TOP/common/cobjectregistry.cpp

HEADERS += \
   mainwindow.h \
   project/ccartridge.h \
   main.h \
   aboutdialog.h \
   emulator/nesemulatorthread.h \
   $$TOP/common/emulatorprefsdialog.h \
   qkeymapitemedit.h \
   emulator/nesemulatorrenderer.h \
   common/emulatorcontrol.h \
   emulator/nesemulatordockwidget.h \
   interfaces/ixmlserializable.h \
   $$TOP/common/appeventfilter.h \
   $$TOP/common/cobjectregistry.h

FORMS += \
   mainwindow.ui \
   aboutdialog.ui \
   $$TOP/common/emulatorprefsdialog.ui \
   common/emulatorcontrol.ui \
   emulator/nesemulatordockwidget.ui

RESOURCES += \
    ../../common/resource.qrc
