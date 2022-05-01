#-------------------------------------------------
#
# Project created by QtCreator 2010-07-27T21:40:46
#
#-------------------------------------------------

QT += core \
      gui

greaterThan(QT_MAJOR_VERSION,4) {
    QT += widgets
}

greaterThan(QT_MAJOR_VERSION,5) {
    QT += core5compat
}

TOP = ../..

#macx {
#    QMAKE_MAC_SDK = macosx10.16
#}

CONFIG(release, debug|release) {
   DESTDIR = release
} else {
   DESTDIR = debug
}

OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR

DEFINES -= UNICODE

TARGET = "famitracker"

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

FAMITRACKER_LIBS = -L$$TOP/libs/famitracker/$$DESTDIR -lfamitracker
FAMITRACKER_CXXFLAGS = -I$$TOP/libs/famitracker

RTMIDI_LIBS = -L$$DEPENDENCYROOTPATH/rtmidi/$$DESTDIR -lrtmidi

# fixme duplication (https://wiki.qt.io/Including_.pro_Files)
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

   QMAKE_PRE_LINK += cp $$TOP/libs/famitracker/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libfamitracker.1.dylib \
       @executable_path/../Frameworks/libfamitracker.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/famitracker $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$DEPENDENCYROOTPATH/rtmidi/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change librtmidi.1.dylib \
       @executable_path/../Frameworks/librtmidi.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/famitracker $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $$DESTDIR/$${TARGET}.app/Contents/MacOS/famitracker $$escape_expand(\n\t)
}

unix:!mac {
   FAMITRACKER_LFLAGS  = -Wl,-rpath=\"$$PWD/$$TOP/libs/famitracker\"

    # if the user didnt set cxxflags and libs then use defaults
    ###########################################################

    isEmpty (SDL_CXXFLAGS) {
       SDL_CXXFLAGS = $$system(sdl-config --cflags)
    }

    isEmpty (SDL_LIBS) {
            SDL_LIBS = $$system(sdl-config --libs)
    }

   PREFIX = $$(PREFIX)
   isEmpty (PREFIX) {
      PREFIX = /usr/local
   }

   isEmpty (BINDIR) {
                BINDIR=$$PREFIX/bin
   }

   target.path = $$BINDIR
   INSTALLS += target
}

QMAKE_CFLAGS += -w
QMAKE_CXXFLAGS += -w

QMAKE_CXXFLAGS += $$FAMITRACKER_CXXFLAGS \
                  $$SDL_CXXFLAGS

QMAKE_LFLAGS += $$FAMITRACKER_LFLAGS
LIBS += $$FAMITRACKER_LIBS \
        $$SDL_LIBS \
        $$RTMIDI_LIBS

unix {
   QMAKE_CFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
   QMAKE_CXXFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
}

INCLUDEPATH += \
   $$TOP/common

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    $$TOP/common/resource.qrc

win32-msvc* {
QMAKE_LFLAGS_EXE -= \"/MANIFESTDEPENDENCY:type=\'win32\' name=\'Microsoft.Windows.Common-Controls\' version=\'6.0.0.0\' publicKeyToken=\'6595b64144ccf1df\' language=\'*\' processorArchitecture=\'*\'\"
# link.exe includes 32-bit by default. this loads 64-bit styles, causing SXS conflict.
}
