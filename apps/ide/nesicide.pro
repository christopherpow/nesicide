# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
      opengl \
      xml \
      widgets

greaterThan(QT_MAJOR_VERSION,5) {
    QT += core5compat
}

win32: LIBS += -lopengl32
macx {
  CONFIG += c++11
  QMAKE_CFLAGS += -mmacosx-version-min=10.7
  QMAKE_LFLAGS += -mmacosx-version-min=10.7
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

OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR

DEFINES -= UNICODE
DEFINES += NOMINMAX NULL=0

TOP = ../..

TARGET = "nesicide"

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

# Remove crap we don't need!
CONFIG -= exceptions

CONFIG(release, debug|release) {
    QSCINTILLA_NAME=qscintilla2_qt5
    win32:QHEXEDIT_NAME=qhexedit4
    else:QHEXEDIT_NAME=qhexedit
} else {
   QHEXEDIT_NAME=qhexeditd
   macx: QSCINTILLA_NAME=qscintilla2_qt5_debug
   else: QSCINTILLA_NAME=qscintilla2_qt5d

}

SCINTILLA_CXXFLAGS = -I$$DEPENDENCYROOTPATH/qscintilla2/src
macx {
    SCINTILLA_LIBS = -L$$DEPENDENCYROOTPATH/qscintilla2/src -l$$QSCINTILLA_NAME
}
unix {
    SCINTILLA_LIBS = -L$$DEPENDENCYROOTPATH/qscintilla2/src -l$$QSCINTILLA_NAME
}
else {
    SCINTILLA_LIBS = -L$$DEPENDENCYROOTPATH/qscintilla2/src/$$DESTDIR -l$$QSCINTILLA_NAME
}

HEXEDIT_CXXFLAGS = -I$$DEPENDENCYROOTPATH/qhexedit2/src
HEXEDIT_LIBS = -L$$DEPENDENCYROOTPATH/qhexedit2/lib -l$$QHEXEDIT_NAME

NES_CXXFLAGS = -I$$TOP/libs/nes -I$$TOP/libs/nes/emulator -I$$TOP/libs/nes/common
NES_LIBS = -L$$TOP/libs/nes/$$DESTDIR -lnes-emulator

C64_CXXFLAGS = -I$$TOP/libs/c64 -I$$TOP/libs/c64/emulator -I$$TOP/libs/c64/common
C64_LIBS = -L$$TOP/libs/c64/$$DESTDIR -lc64-emulator

FAMITRACKER_CXXFLAGS = -I$$TOP/libs/famitracker
FAMITRACKER_LIBS = -L$$TOP/libs/famitracker/$$DESTDIR -lfamitracker

RTMIDI_LIBS = -L$$DEPENDENCYROOTPATH/rtmidi/$$DESTDIR -lrtmidi
RTMIDI_CXXFLAGS = -I$$DEPENDENCYROOTPATH/rtmidi

win32 {
    contains(QT_ARCH, i386) {
        arch = x86
    } else {
        arch = x64
    }

   SDL_CXXFLAGS = -I$$DEPENDENCYPATH/SDL
   SDL_LIBS =  -L$$DEPENDENCYPATH/SDL/$$arch -lsdl

   LUA_CXXFLAGS = -I $$DEPENDENCYPATH/Lua
   LUA_LIBS = -L$$DEPENDENCYPATH/Lua -llua51

   QMAKE_LFLAGS += -static-libgcc
}

mac {
   SDL_CXXFLAGS = -I $$DEPENDENCYPATH/SDL.framework/Headers
   SDL_LIBS = -F $$DEPENDENCYPATH -framework SDL

   LUA_CXXFLAGS = -I $$DEPENDENCYPATH/Lua.framework/Headers
   LUA_LIBS = -F $$DEPENDENCYPATH -framework Lua

   QMAKE_PRE_LINK += mkdir -p $$DESTDIR/$${TARGET}.app/Contents/Frameworks $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$TOP/libs/famitracker/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libfamitracker.1.dylib \
       @executable_path/../Frameworks/libfamitracker.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$DEPENDENCYROOTPATH/rtmidi/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change librtmidi.1.dylib \
       @executable_path/../Frameworks/librtmidi.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$TOP/libs/nes/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnes-emulator.1.dylib \
       @executable_path/../Frameworks/libnes-emulator.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$TOP/libs/c64/$$DESTDIR/*.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libc64-emulator.1.dylib \
       @executable_path/../Frameworks/libc64-emulator.1.dylib \
       $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp -r $$DEPENDENCYPATH/SDL.framework \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp -r $$DEPENDENCYPATH/Lua.framework \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$DEPENDENCYROOTPATH/qscintilla2/src/lib$${QSCINTILLA_NAME}.13.2.1.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/lib$${QSCINTILLA_NAME}.13.dylib $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change lib$${QSCINTILLA_NAME}.13.dylib \
      @executable_path/../Frameworks/lib$${QSCINTILLA_NAME}.13.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_PRE_LINK += cp $$DEPENDENCYROOTPATH/qhexedit2/lib/lib$${QHEXEDIT_NAME}.4.1.0.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/Frameworks/lib$${QHEXEDIT_NAME}.4.dylib $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change lib$${QHEXEDIT_NAME}.4.dylib \
      @executable_path/../Frameworks/lib$${QHEXEDIT_NAME}.4.dylib \
      $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   QMAKE_POST_LINK += install_name_tool -add_rpath @loader_path/../Frameworks $$DESTDIR/$${TARGET}.app/Contents/MacOS/nesicide $$escape_expand(\n\t)

   ICON = mac/resources/nesicide.icns
}

unix:!mac {
   FAMITRACKER_LFLAGS = -Wl,-rpath=\"$$PWD/$$TOP/libs/famitracker\"
   NES_LFLAGS = -Wl,-rpath=\"$$PWD/$$TOP/libs/nes\"
   C64_LFLAGS = -Wl,-rpath=\"$$PWD/$$TOP/libs/nes\"

    # if the user didnt set cxxflags and libs then use defaults
    ###########################################################

    isEmpty (SDL_CXXFLAGS) {
       SDL_CXXFLAGS = $$system(sdl-config --cflags)
    }

    isEmpty (SDL_LIBS) {
            SDL_LIBS = $$system(sdl-config --libs)
    }

    # lua provides lua.pc, but several distros renamed it for slotting ( installing multiple versions )

    isEmpty (LUA_CXXFLAGS) {
            LUA_CXXFLAGS = $$system(pkg-config --silence-errors --cflags lua)
    }

    isEmpty (LUA_CXXFLAGS) {
            LUA_CXXFLAGS = $$system(pkg-config --silence-errors --cflags lua5.1)
    }

    isEmpty (LUA_CXXFLAGS) {
       LUA_CXXFLAGS = $$system(pkg-config --silence-errors --cflags lua-5.1)
    }

    isEmpty (LUA_LIBS) {
            LUA_LIBS = $$system(pkg-config --silence-errors --libs lua)
    }

    isEmpty (LUA_LIBS) {
            LUA_LIBS = $$system(pkg-config --silence-errors --libs lua5.1)
    }

    isEmpty (LUA_LIBS) {
       LUA_LIBS = $$system(pkg-config --silence-errors --libs lua-5.1)
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

unix {
   QMAKE_CFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
   QMAKE_CXXFLAGS += -I $$DEPENDENCYROOTPATH/wine/include -DWINE_UNICODE_NATIVE
}

QMAKE_CFLAGS += -w
QMAKE_CXXFLAGS += -w

QMAKE_CXXFLAGS += -DIDE \
                  $$NES_CXXFLAGS \
                  $$C64_CXXFLAGS \
                  $$FAMITRACKER_CXXFLAGS \
                  $$SDL_CXXFLAGS \
                  $$LUA_CXXFLAGS \
                  $$SCINTILLA_CXXFLAGS \
                  $$HEXEDIT_CXXFLAGS \
                  $$RTMIDI_CXXFLAGS
QMAKE_LFLAGS += $$FAMITRACKER_LFLAGS \
                $$NES_LFLAGS \
                $$C64_LFLAGS

LIBS += $$NES_LIBS \
        $$C64_LIBS \
        $$FAMITRACKER_LIBS \
        $$SDL_LIBS \
        $$LUA_LIBS \
        $$SCINTILLA_LIBS \
        $$HEXEDIT_LIBS \
        $$RTMIDI_LIBS

INCLUDEPATH += \
   $$TOP/common \
   common \
   nes/common \
   compilers \
   compilers/cc65 \
   nes/compilers \
   c64/compilers \
   debuggers \
   nes/debuggers \
   c64/debuggers \
   designers \
   nes/designers \
   emulator \
   nes/emulator \
   c64/emulator \
   interfaces \
   project \
   nes/project \
   plugins \
   resources \

SOURCES += \
   $$TOP/common/cbreakpointinfo.cpp \
   $$TOP/common/xmlhelpers.cpp \
   aboutdialog.cpp \
   common/cbuildertextlogger.cpp \
   common/cdockwidgetregistry.cpp \
   designers/filepropertiesdialog.cpp \
   nes/common/cgamedatabasehandler.cpp \
   common/checkboxlist.cpp \
   nes/common/chrbankitemstabwidget.cpp \
   nes/common/cimageconverters.cpp \
   nes/common/colorpushbutton.cpp \
   common/cprojecttabwidget.cpp \
   common/cpropertyitem.cpp \
   common/crendererbase.cpp \
   nes/emulator/nesemulatorcontrol.cpp \
   c64/emulator/c64emulatorcontrol.cpp \
   common/panzoomrenderer.cpp \
   common/qtcolorpicker.cpp \
   common/searchbar.cpp \
   common/searchwidget.cpp \
   common/searcherthread.cpp \
   common/sourcenavigator.cpp \
   nes/common/tilificationthread.cpp \
   compilers/cc65/ccc65interface.cpp \
   compilers/cc65/dbginfo.c \
   nes/compilers/ccartridgebuilder.cpp \
   nes/compilers/cgraphicsassembler.cpp \
   compilers/compilerthread.cpp \
   compilers/csourceassembler.cpp \
   nes/debuggers/apuinformationdockwidget.cpp \
   debuggers/breakpointdialog.cpp \
   debuggers/breakpointdockwidget.cpp \
   debuggers/breakpointwatcherthread.cpp \
   debuggers/cbreakpointdisplaymodel.cpp \
   debuggers/ccodebrowserdisplaymodel.cpp \
   debuggers/cdebuggerbase.cpp \
   debuggers/cdebuggercodeprofilermodel.cpp \
   debuggers/cdebuggerexecutiontracermodel.cpp \
   debuggers/cdebuggermemorydisplaymodel.cpp \
   debuggers/cdebuggernumericitemdelegate.cpp \
   debuggers/cdebuggerbitfieldcomboboxdelegate.cpp \
   debuggers/cdebuggerbitfielddisplaymodel.cpp \
   debuggers/cdebuggersymboldelegate.cpp \
   debuggers/cexecutionmarkerdisplaymodel.cpp \
   nes/debuggers/chrmeminspector.cpp \
   debuggers/codebrowserdockwidget.cpp \
   nes/debuggers/codedataloggerdockwidget.cpp \
   debuggers/codeprofilerdockwidget.cpp \
   debuggers/csymbolwatchmodel.cpp \
   nes/debuggers/dbg_cnes.cpp \
   nes/debuggers/dbg_cnes6502.cpp \
   nes/debuggers/dbg_cnesapu.cpp \
   nes/debuggers/dbg_cnesmappers.cpp \
   nes/debuggers/dbg_cnesppu.cpp \
   nes/debuggers/dbg_cnesrom.cpp \
   debuggers/debuggerupdatethread.cpp \
   debuggers/executioninspectordockwidget.cpp \
   nes/debuggers/executionvisualizerdockwidget.cpp \
   nes/debuggers/mapperinformationdockwidget.cpp \
   debuggers/memoryinspectordockwidget.cpp \
   nes/debuggers/nametablevisualizerdockwidget.cpp \
   nes/debuggers/oamvisualizerdockwidget.cpp \
   nes/debuggers/ppuinformationdockwidget.cpp \
   debuggers/registerinspectordockwidget.cpp \
   debuggers/symbolwatchdockwidget.cpp \
   nes/designers/attributetableeditorform.cpp \
   designers/cdesignereditorbase.cpp \
   designers/codeeditorform.cpp \
   designers/qscilexerca65.cpp \
   designers/qscilexerdefault.cpp \
   nes/designers/cchrrombankitemdelegate.cpp \
   nes/designers/graphicsbankeditorform.cpp \
   designers/newprojectdialog.cpp \
   designers/cpropertyenumlistmodel.cpp \
   designers/cpropertylistmodel.cpp \
   designers/cpropertysymboldelegate.cpp \
   designers/cpropertyvaluedelegate.cpp \
   designers/projectpropertiesdialog.cpp \
   designers/propertyeditordialog.cpp \
   nes/designers/ctilestamprenderer.cpp \
   nes/designers/tilestampeditorform.cpp \
   nes/emulator/nesemulatordockwidget.cpp \
   nes/emulator/nesemulatorrenderer.cpp \
   nes/emulator/nesemulatorthread.cpp \
   $$TOP/common/emulatorprefsdialog.cpp \
   c64/emulator/c64emulatorthread.cpp \
   environmentsettingsdialog.cpp \
   main.cpp \
   mainwindow.cpp \
   outputpanedockwidget.cpp \
   plugins/cpluginmanager.cpp \
   nes/project/cattributetable.cpp \
   nes/project/cattributetables.cpp \
   project/cbinaryfile.cpp \
   project/cbinaryfiles.cpp \
   nes/project/ccartridge.cpp \
   nes/project/cchrrombank.cpp \
   nes/project/cchrrombanks.cpp \
   nes/project/cgraphicsbank.cpp \
   nes/project/cgraphicsbanks.cpp \
   project/cnesicideproject.cpp \
   nes/project/cprgrombank.cpp \
   nes/project/cprgrombanks.cpp \
   project/cproject.cpp \
   project/cprojectbase.cpp \
   nes/project/cprojectprimitives.cpp \
   project/csourceitem.cpp \
   project/csources.cpp \
   nes/project/ctilestamp.cpp \
   nes/project/ctilestamps.cpp \
   project/projectbrowserdockwidget.cpp \
   common/qkeymapitemedit.cpp \
   startupsplashdialog.cpp \
   nes/emulator/testsuiteexecutivedialog.cpp \
   $$TOP/common/version.cpp \
   nes/debuggers/cchrromitemlistdisplaymodel.cpp \
   nes/debuggers/cchrromitemtabledisplaymodel.cpp \
   nes/debuggers/chrromdisplaydialog.cpp \
   nes/debuggers/prgromdisplaydialog.cpp \
   debuggers/cdebuggerregisterdisplaymodel.cpp \
   c64/compilers/cmachineimagebuilder.cpp \
   c64/debuggers/dbg_cc64.cpp \
   $$TOP/common/appeventfilter.cpp \
   $$TOP/common/cobjectregistry.cpp \
    nes/debuggers/joypadloggerdockwidget.cpp \
    model/cprojectmodel.cpp \
    model/csourcefilemodel.cpp \
    model/cfiltermodel.cpp \
    model/cbinaryfilemodel.cpp \
    model/ctilestampmodel.cpp \
    model/cattributemodel.cpp \
    model/cgraphicsbankmodel.cpp \
    model/ccartridgemodel.cpp \
    project/cprojecttreecontextmenu.cpp \
    project/cprojecttreeopenaction.cpp \
    project/cprojecttreewidget.cpp \
    designers/qscilexerbin.cpp \
    designers/qscilexercc65.cpp \
    common/cexpandablestatusbar.cpp \
    nes/designers/musiceditorform.cpp \
    model/cmusicmodel.cpp \
    nes/project/cmusics.cpp \
    nes/project/csounds.cpp \
    nes/project/cmusicitem.cpp \
    designers/newfiledialog.cpp \
    appsettings.cpp \
    designers/qsciapisca65.cpp

HEADERS += \
   aboutdialog.h \
   $$TOP/common/cbreakpointinfo.h \
   common/cbuildertextlogger.h \
   common/cdesignercommon.h \
   common/cdockwidgetregistry.h \
   designers/filepropertiesdialog.h \
   nes/common/cgamedatabasehandler.h \
   common/checkboxlist.h \
   nes/common/chrbankitemstabwidget.h \
   nes/common/cimageconverters.h \
   nes/common/colorpushbutton.h \
   nes/common/cpaletteitemdelegate.h \
   common/cprojecttabwidget.h \
   common/cpropertyitem.h \
   common/crendererbase.h \
   nes/emulator/nesemulatorcontrol.h \
   c64/emulator/c64emulatorcontrol.h \
   common/panzoomrenderer.h \
   common/qtcolorpicker.h \
   common/searchbar.h \
   common/searchwidget.h \
   common/searcherthread.h \
   common/sourcenavigator.h \
   nes/common/tilificationthread.h \
   compilers/cc65/ccc65interface.h \
   compilers/cc65/dbginfo.h \
   nes/compilers/ccartridgebuilder.h \
   nes/compilers/cgraphicsassembler.h \
   compilers/compilerthread.h \
   compilers/csourceassembler.h \
   nes/debuggers/apuinformationdockwidget.h \
   debuggers/breakpointdialog.h \
   debuggers/breakpointdockwidget.h \
   debuggers/breakpointwatcherthread.h \
   debuggers/cbreakpointdisplaymodel.h \
   debuggers/ccodebrowserdisplaymodel.h \
   debuggers/cdebuggerbase.h \
   debuggers/cdebuggercodeprofilermodel.h \
   debuggers/cdebuggerexecutiontracermodel.h \
   debuggers/cdebuggermemorydisplaymodel.h \
   debuggers/cdebuggernumericitemdelegate.h \
   debuggers/cdebuggerbitfieldcomboboxdelegate.h \
   debuggers/cdebuggerbitfielddisplaymodel.h \
   debuggers/cdebuggersymboldelegate.h \
   debuggers/cexecutionmarkerdisplaymodel.h \
   nes/debuggers/chrmeminspector.h \
   debuggers/codebrowserdockwidget.h \
   nes/debuggers/codedataloggerdockwidget.h \
   debuggers/codeprofilerdockwidget.h \
   debuggers/csymbolwatchmodel.h \
   nes/debuggers/dbg_cnes.h \
   nes/debuggers/dbg_cnes6502.h \
   nes/debuggers/dbg_cnesapu.h \
   nes/debuggers/dbg_cnesmappers.h \
   nes/debuggers/dbg_cnesppu.h \
   nes/debuggers/dbg_cnesrom.h \
   debuggers/debuggerupdatethread.h \
   debuggers/executioninspectordockwidget.h \
   nes/debuggers/executionvisualizerdockwidget.h \
   nes/debuggers/mapperinformationdockwidget.h \
   debuggers/memoryinspectordockwidget.h \
   nes/debuggers/nametablevisualizerdockwidget.h \
   nes/debuggers/oamvisualizerdockwidget.h \
   nes/debuggers/ppuinformationdockwidget.h \
   debuggers/registerinspectordockwidget.h \
   debuggers/symbolwatchdockwidget.h \
   nes/designers/attributetableeditorform.h \
   designers/cdesignereditorbase.h \
   designers/codeeditorform.h \
   designers/qscilexerca65.h \
   designers/qscilexerdefault.h \
   nes/designers/cchrrombankitemdelegate.h \
   nes/designers/graphicsbankeditorform.h \
   designers/newprojectdialog.h \
   designers/cpropertyenumlistmodel.h \
   designers/cpropertylistmodel.h \
   designers/cpropertysymboldelegate.h \
   designers/cpropertyvaluedelegate.h \
   designers/projectpropertiesdialog.h \
   designers/propertyeditordialog.h \
   nes/designers/ctilestamprenderer.h \
   nes/designers/tilestampeditorform.h \
   nes/emulator/nesemulatordockwidget.h \
   nes/emulator/nesemulatorrenderer.h \
   nes/emulator/nesemulatorthread.h \
   c64/emulator/c64emulatorthread.h \
   $$TOP/common/emulatorprefsdialog.h \
   environmentsettingsdialog.h \
   interfaces/icenterwidgetitem.h \
   interfaces/ichrrombankitem.h \
   interfaces/iprojecttreeviewitem.h \
   interfaces/ixmlserializable.h \
   main.h \
   mainwindow.h \
   outputpanedockwidget.h \
   plugins/cpluginmanager.h \
   nes/project/cattributetable.h \
   nes/project/cattributetables.h \
   project/cbinaryfile.h \
   project/cbinaryfiles.h \
   nes/project/ccartridge.h \
   nes/project/cchrrombank.h \
   nes/project/cchrrombanks.h \
   nes/project/cgraphicsbank.h \
   nes/project/cgraphicsbanks.h \
   project/cnesicideproject.h \
   nes/project/cprgrombank.h \
   nes/project/cprgrombanks.h \
   project/cproject.h \
   project/cprojectbase.h \
   nes/project/cprojectprimitives.h \
   project/csourceitem.h \
   project/csources.h \
   nes/project/ctilestamp.h \
   nes/project/ctilestamps.h \
   project/projectbrowserdockwidget.h \
   common/qkeymapitemedit.h \
   startupsplashdialog.h \
   nes/emulator/testsuiteexecutivedialog.h \
   nes/debuggers/cchrromitemlistdisplaymodel.h \
   nes/debuggers/cchrromitemtabledisplaymodel.h \
   nes/debuggers/chrromdisplaydialog.h \
   nes/debuggers/prgromdisplaydialog.h \
   debuggers/cdebuggerregisterdisplaymodel.h \
   c64/compilers/cmachineimagebuilder.h \
   c64/debuggers/dbg_cc64.h \
   $$TOP/common/cregisterdata.h \
   $$TOP/common/cmemorydata.h \
   $$TOP/common/appeventfilter.h \
   $$TOP/common/cobjectregistry.h \
    nes/debuggers/joypadloggerdockwidget.h \
    model/cprojectmodel.h \
    model/projectsearcher.h \
    model/csourcefilemodel.h \
    model/cfiltermodel.h \
    model/cbinaryfilemodel.h \
    model/ctilestampmodel.h \
    model/cattributemodel.h \
    model/iuuidvisitor.h \
    model/cgraphicsbankmodel.h \
    model/csubmodel.h \
    model/ccartridgemodel.h \
    project/cprojecttreewidget.h \
    project/cprojecttreecontextmenu.h \
    project/cprojecttreeopenaction.h \
    designers/qscilexerbin.h \
    designers/qscilexercc65.h \
    common/cexpandablestatusbar.h \
    nes/designers/musiceditorform.h \
    model/cmusicmodel.h \
    nes/project/cmusics.h \
    nes/project/csounds.h \
    nes/project/cmusicitem.h \
    designers/newfiledialog.h \
    appsettings.h \
    designers/qsciapisca65.h

FORMS += \
   aboutdialog.ui \
   designers/filepropertiesdialog.ui \
   nes/common/chrbankitemstabwidget.ui \
   nes/emulator/nesemulatorcontrol.ui \
   c64/emulator/c64emulatorcontrol.ui \
   common/panzoomrenderer.ui \
   common/searchbar.ui \
   common/searchwidget.ui \
   common/sourcenavigator.ui \
   nes/debuggers/apuinformationdockwidget.ui \
   debuggers/breakpointdialog.ui \
   debuggers/breakpointdockwidget.ui \
   debuggers/codebrowserdockwidget.ui \
   nes/debuggers/codedataloggerdockwidget.ui \
   debuggers/codeprofilerdockwidget.ui \
   debuggers/executioninspectordockwidget.ui \
   nes/debuggers/executionvisualizerdockwidget.ui \
   nes/debuggers/mapperinformationdockwidget.ui \
   debuggers/memoryinspectordockwidget.ui \
   nes/debuggers/nametablevisualizerdockwidget.ui \
   nes/debuggers/oamvisualizerdockwidget.ui \
   nes/debuggers/ppuinformationdockwidget.ui \
   debuggers/registerinspectordockwidget.ui \
   debuggers/symbolwatchdockwidget.ui \
   nes/designers/attributetableeditorform.ui \
   designers/codeeditorform.ui \
   nes/designers/graphicsbankeditorform.ui \
   designers/newprojectdialog.ui \
   designers/projectpropertiesdialog.ui \
   designers/propertyeditordialog.ui \
   nes/designers/tilestampeditorform.ui \
   nes/emulator/nesemulatordockwidget.ui \
   $$TOP/common/emulatorprefsdialog.ui \
   environmentsettingsdialog.ui \
   mainwindow.ui \
   outputpanedockwidget.ui \
   project/projectbrowserdockwidget.ui \
   startupsplashdialog.ui \
   nes/emulator/testsuiteexecutivedialog.ui \
   nes/debuggers/chrromdisplaydialog.ui \
   nes/debuggers/prgromdisplaydialog.ui \
    nes/debuggers/joypadloggerdockwidget.ui \
    common/cexpandablestatusbar.ui \
    nes/designers/musiceditorform.ui \
    designers/newfiledialog.ui

RESOURCES += \
    addons.qrc \
    templates.qrc \
    ../../common/resource.qrc
