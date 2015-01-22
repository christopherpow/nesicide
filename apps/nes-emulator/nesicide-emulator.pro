QT += core \
   gui \
   opengl \
   xml

TOP = ../..

macx {
    MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk
    if( !exists( $$MAC_SDK) ) {
        error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
    }
    macx:QMAKE_MAC_SDK = macosx10.10
}

TARGET = nes-emulator

win32 {
   DEPENDENCYPATH = $$TOP/deps/Windows
}
mac {
   DEPENDENCYPATH = $$TOP/deps/osx
}
#unix:mac {
#	DEPENDENCYPATH = $$TOP/deps/linux
#}

# Remove crap we do not need!
CONFIG -= rtti exceptions

isEmpty (NESICIDE_LIBS) {
   NESICIDE_LIBS = -lnes-emulator
}

# set platform specific cxxflags and libs
#########################################

CONFIG(release, debug|release) {
   LIB_BUILD_TYPE_DIR = release
} else {
   LIB_BUILD_TYPE_DIR = debug
}

win32 {
   SDL_CXXFLAGS = -I$$TOP/deps/Windows/SDL
   SDL_LIBS =  -L$$TOP/deps/Windows/SDL/ -lsdl

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

   SDL_CXXFLAGS = -I $$DEPENDENCYPATH/SDL.framework/Headers
   SDL_LIBS = -F $$DEPENDENCYPATH -framework SDL

   QMAKE_POST_LINK += mkdir -p $${DESTDIR}/$${TARGET}.app/Contents/Frameworks $$escape_expand(\n\t)

   QMAKE_POST_LINK += cp $$TOP/libs/nes/$${LIB_BUILD_TYPE_DIR}/libnes-emulator.1.0.0.dylib \
      $${DESTDIR}/$${TARGET}.app/Contents/Frameworks/libnes-emulator.1.dylib $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnes-emulator.1.dylib \
      @executable_path/../Frameworks/libnes-emulator.1.dylib \
      $${DESTDIR}/$${TARGET}.app/Contents/MacOS/nes-emulator $$escape_expand(\n\t)

   # SDL
   QMAKE_POST_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $${DESTDIR}/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $${DESTDIR}/$${TARGET}.app/Contents/MacOS/nes-emulator $$escape_expand(\n\t)

   ICON = $$TOP/common/resources/controller.icns
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
