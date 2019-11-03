#-------------------------------------------------
#
# Project created by QtCreator 2010-07-27T21:40:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION,4) {
    QT += widgets
}

TOP = ../..

macx {
    QMAKE_MAC_SDK = macosx10.15
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

DEFINES -= UNICODE

TARGET = "famiplayer"

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

win32 {
#    contains(QT_ARCH, i386) {
#        arch = x86
#    } else {
#        arch = x64
#    }
   arch = x86

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
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/famiplayer $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$DEPENDENCYROOTPATH/rtmidi/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change librtmidi.1.dylib \
       @executable_path/../Frameworks/librtmidi.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/famiplayer $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $$DESTDIR/$${TARGET}.app/Contents/MacOS/famiplayer $$escape_expand(\n\t)
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

QMAKE_CXXFLAGS += $$FAMITRACKER_CXXFLAGS \
                  $$SDL_CXXFLAGS \
                  $$RTMIDI_CXXFLAGS
QMAKE_LFLAGS += $$FAMITRACKER_LFLAGS
LIBS += $$FAMITRACKER_LIBS \
        $$SDL_LIBS \
        $$RTMIDI_LIBS

unix {
   QMAKE_CFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
   QMAKE_CXXFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
}

QMAKE_CFLAGS += -w
QMAKE_CXXFLAGS += -w

INCLUDEPATH += \
   $$TOP/common

SOURCES += main.cpp\
        mainwindow.cpp \
    playlisteditordialog.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    playlisteditordialog.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    playlisteditordialog.ui \
    aboutdialog.ui

RESOURCES += \
    $$TOP/common/resource.qrc
