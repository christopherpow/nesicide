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
	DEPENDENCYPATH = $$TOP/deps/Windows
}
mac {
	DEPENDENCYPATH = $$TOP/deps/osx
}
#unix:mac {
#	DEPENDENCYPATH = $$TOP/deps/linux
#}

TEMPLATE = app

# set platform specific cxxflags and libs
#########################################

FAMITRACKER_LIBS = -L$$TOP/libs/famitracker/$$DESTDIR -lfamitracker
FAMITRACKER_CXXFLAGS = -I$$TOP/libs/famitracker

win32 {
   SDL_CXXFLAGS = -I$$TOP/deps/Windows/SDL
   SDL_LIBS =  -L$$TOP/deps/Windows/SDL/ -lsdl

   QMAKE_LFLAGS += -static-libgcc
}

mac {
   SDL_CXXFLAGS = -I$$DEPENDENCYPATH/SDL.framework/Headers
   SDL_LIBS = -F$$DEPENDENCYPATH -framework SDL

   ICON = $$TOP/common/resources/controller.icns

   QMAKE_POST_LINK += mkdir -p $${DESTDIR}/$${TARGET}.app/Contents/Frameworks $$escape_expand(\n\t)

   # copy lib from debug/release or base
#   QMAKE_POST_LINK += cp $$TOP/libs/famitracker/$$DESTDIR/libfamitracker.1.0.0.dylib \
#      $${DESTDIR}/$${TARGET}.app/Contents/Frameworks/libfamitracker.1.dylib || true $$escape_expand(\n\t)

#   QMAKE_POST_LINK += ln -s $$TOP/libs/famitracker/$$DESTDIR/libfamitracker.1.0.0.dylib \
#                      $${DESTDIR}/$${TARGET}.app/Contents/Frameworks/libfamitracker.1.dylib || true $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -change libfamitracker.1.dylib \
       $$TOP/../../../../libs/famitracker/$$DESTDIR/libfamitracker.1.0.0.dylib \
       $${DESTDIR}/$${TARGET}.app/Contents/MacOS/famiplayer $$escape_expand(\n\t)

#   QMAKE_POST_LINK += install_name_tool -change libfamitracker.1.dylib \
#      @executable_path/../Frameworks/libfamitracker.1.dylib \
#      $${DESTDIR}/$${TARGET}.app/Contents/MacOS/famitracker $$escape_expand(\n\t)

   # SDL
   QMAKE_POST_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $${DESTDIR}/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $${DESTDIR}/$${TARGET}.app/Contents/MacOS/famiplayer $$escape_expand(\n\t)
   #QMAKE_POST_LINK += install_name_tool -change SDL \
   #   @executable_path/../Frameworks/SDL.framework/SDL \
   #   $${DESTDIR}/$${TARGET}.app/Contents/MacOS/famitracker $$escape_expand(\n\t)
}

unix:!mac {
   FAMITRACKER_CXXFLAGS += -I/usr/include/wine/windows/
   FAMITRACKER_LFLAGS = -Wl,-rpath=\"$$PWD/$$TOP/libs/famitracker\"

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
                  $$SDL_CXXFLAGS
QMAKE_LFLAGS = $$FAMITRACKER_LFLAGS
LIBS += $$FAMITRACKER_LIBS \
        $$SDL_LIBS

unix:mac {
	QMAKE_CFLAGS += -I $$DEPENDENCYPATH/wine/include -DWINE_UNICODE_NATIVE
	QMAKE_CXXFLAGS += -I $$DEPENDENCYPATH/wine/include -DWINE_UNICODE_NATIVE
}

unix:!mac {
    QMAKE_CFLAGS += -DWINE_UNICODE_NATIVE
    QMAKE_CXXFLAGS += -DWINE_UNICODE_NATIVE
}

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
