QT += core \
   gui \
   opengl \
   xml

TOP = ../..

TARGET = nes-emulator

# Remove crap we do not need!
CONFIG -= rtti exceptions

isEmpty (NESICIDE_LIBS) {
   NESICIDE_LIBS = -lnes-emulator
}

win32 {
   SDL_CXXFLAGS = -I$$TOP/deps/SDL
   SDL_LIBS =  -L$$TOP/deps/SDL/ -lsdl

   NESICIDE_CXXFLAGS = -I$$TOP/libs/nes -I$$TOP/libs/nes/emulator

   CONFIG(release, debug|release) {
      NESICIDE_LIBS = -L$$TOP/libs/nes/release -lnes-emulator
   } else {
      NESICIDE_LIBS = -L$$TOP/libs/nes/debug -lnes-emulator
   }
}

mac {
   CONFIG(release, debug|release) {
      DESTDIR = release
      OBJECTS_DIR = release
      QMAKE_CXXFLAGS_RELEASE -= -O2
      QMAKE_CXXFLAGS_RELEASE += -Os
   } else {
      DESTDIR = debug
      OBJECTS_DIR = debug
   }
   QMAKE_CFLAGS += -macx

   NESICIDE_CXXFLAGS = -I $$TOP/libs/nes -I $$TOP/libs/nes/emulator
   CONFIG(release, debug|release) {
      BUILD_DIR = release
   } else {
      BUILD_DIR = debug
   }
   NESICIDE_LIBS = -L$$TOP/libs/nes/$$BUILD_DIR -lnes-emulator

   SDL_CXXFLAGS = -I/Library/Frameworks/SDL.framework/Headers
   SDL_LIBS = -framework SDL

  QMAKE_POST_LINK += mkdir -p \'$${BUILD_DIR}/$${TARGET}.app/Contents/Frameworks\' $$escape_expand(\n\t)
   QMAKE_POST_LINK += cp \'$$TOP/libs/nes/$${BUILD_DIR}/libnes-emulator.1.0.0.dylib\' \
   \'$${BUILD_DIR}/$${TARGET}.app/Contents/Frameworks/libnes-emulator.1.dylib\' $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnes-emulator.1.dylib \
      @executable_path/../Frameworks/libnes-emulator.1.dylib \'$${BUILD_DIR}/$${TARGET}.app/Contents/MacOS/$${TARGET}\' $$escape_expand(\n\t)

   ICON = ./Resources/controller.icns
}

unix:!mac {
   NESICIDE_CXXFLAGS = -I $$TOP/libs/nes -I $$TOP/libs/nes/emulator -I $$TOP/libs/nes/common
   NESICIDE_LIBS = -L$$TOP/libs/nes -lnes-emulator

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
   emulator/emulatorprefsdialog.cpp \
   qkeymapitemedit.cpp \
   version.cpp \
   emulator/nesemulatorrenderer.cpp \
   common/emulatorcontrol.cpp \
   emulator/nesemulatordockwidget.cpp \
   $$TOP/common/appeventfilter.cpp \
   $$TOP/common/cthreadregistry.cpp

HEADERS += \
   mainwindow.h \
   project/ccartridge.h \
   main.h \
   aboutdialog.h \
   emulator/nesemulatorthread.h \
   emulator/emulatorprefsdialog.h \
   qkeymapitemedit.h \
   emulator/nesemulatorrenderer.h \
   common/emulatorcontrol.h \
   emulator/nesemulatordockwidget.h \
   interfaces/ixmlserializable.h \
   $$TOP/common/appeventfilter.h \
   $$TOP/common/cthreadregistry.h

FORMS += \
   mainwindow.ui \
   aboutdialog.ui \
   emulator/emulatorprefsdialog.ui \
   common/emulatorcontrol.ui \
   emulator/nesemulatordockwidget.ui

RESOURCES += \
    resource.qrc
