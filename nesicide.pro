# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
   opengl \
   webkit \
   xml

TARGET = "nesicide"

# Remove crap we don't need!
CONFIG -= exceptions

isEmpty (NESICIDE_LIBS) {
   NESICIDE_LIBS = -lnes-emulator
}

isEmpty (SCINTILLA_LIBS) {
   SCINTILLA_LIBS = -lqscintilla2
}

# set platform specific cxxflags and libs
#########################################

win32 {

   SDL_CXXFLAGS = -I../nesicide/libraries/SDL
   SDL_LIBS =  -L../nesicide/libraries/SDL/ -lsdl

   SCINTILLA_CXXFLAGS = -I../nesicide/libraries/Qscintilla

   CONFIG(release, debug|release) {
      SCINTILLA_LIBS = -L../nesicide/libraries/Qscintilla/release -lqscintilla2
   } else {
      SCINTILLA_LIBS = -L../nesicide/libraries/Qscintilla/debug -lqscintilla2
   }

   LUA_CXXFLAGS = -I../nesicide/libraries/Lua
   LUA_LIBS = ../nesicide/libraries/Lua/liblua.a

   NES_CXXFLAGS = -I../nes-emulator-lib -I../nes-emulator-lib/emulator -I../nes-emulator-lib/common
   C64_CXXFLAGS = -I../c64-emulator-lib -I../c64-emulator-lib/emulator -I../c64-emulator-lib/common

   QMAKE_LFLAGS += -static-libgcc

   CONFIG(release, debug|release) {
      NES_LIBS = -L../nes-emulator-lib-build-desktop/release -lnes-emulator
      C64_LIBS = -L../c64-emulator-lib-build-desktop/release -lc64-emulator
   } else {
      NES_LIBS = -L../nes-emulator-lib-build-desktop/debug -lnes-emulator
      C64_LIBS = -L../c64-emulator-lib-build-desktop/debug -lc64-emulator
   }
}

mac {
   NES_CXXFLAGS = -I ../nes-emulator-lib -I ../nes-emulator-lib/emulator -I ../nes-emulator-lib/common
   NES_LIBS = -L../nes-emulator-lib-build-desktop -lnes-emulator

   SDL_CXXFLAGS = -framework SDL
   SDL_LIBS = -framework SDL

   LUA_CXXFLAGS = -F.. -framework Lua
   LUA_LIBS = -F.. -framework Lua

   SCINTILLA_CXXFLAGS = -I../nesicide/libraries/Qscintilla

   CONFIG(release, debug|release) {
      SCINTILLA_LIBS = -L../nesicide/libraries/Qscintilla/release -lqscintilla2
   } else {
      SCINTILLA_LIBS = -L../nesicide/libraries/Qscintilla/debug -lqscintilla2
   }

   TARGET = "NESICIDE"

   QMAKE_POST_LINK += mkdir -p $$TARGET.app/Contents/Frameworks $$escape_expand(\n\t)
   QMAKE_POST_LINK += cp ../nes-emulator-lib-build-desktop/libnes-emulator.1.0.0.dylib \
      $$TARGET.app/Contents/Frameworks/libnes-emulator.1.dylib $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnes-emulator.1.dylib \
      @executable_path/../Frameworks/libnes-emulator.1.dylib \
      $$TARGET.app/Contents/MacOS/NESICIDE $$escape_expand(\n\t)
   QMAKE_POST_LINK += cp -r ../Lua.framework \
      $$TARGET.app/Contents/Frameworks/ $$escape_expand(\n\t)
}

unix:!mac {
   NES_CXXFLAGS = -I ../nes-emulator-lib -I ../nes-emulator-lib/emulator -I ../nes-emulator-lib/common
   NES_LIBS = -L../nes-emulator-lib-build-desktop -lnes-emulator

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

   isEmpty (PREFIX) {
      PREFIX = /usr/local
   }

   isEmpty (BINDIR) {
                BINDIR=$$PREFIX/bin
   }

   target.path = $$BINDIR
   INSTALLS += target
}

QMAKE_CXXFLAGS += $$NES_CXXFLAGS $$C64_CXXFLAGS $$SDL_CXXFLAGS $$LUA_CXXFLAGS $$SCINTILLA_CXXFLAGS
LIBS += $$NES_LIBS $$C64_LIBS $$SDL_LIBS $$LUA_LIBS $$SCINTILLA_LIBS

INCLUDEPATH += \
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
    aboutdialog.cpp \
    emulator/cbreakpointinfo.cpp \
    common/cbuildertextlogger.cpp \
    common/cdockwidgetregistry.cpp \
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
    common/searchdockwidget.cpp \
    common/searcherthread.cpp \
    common/sourcenavigator.cpp \
    nes/common/tilificationthread.cpp \
    common/xmlhelpers.cpp \
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
    nes/emulator/emulatorprefsdialog.cpp \
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
    version.cpp \
    nes/debuggers/cchrromitemlistdisplaymodel.cpp \
    nes/debuggers/cchrromitemtabledisplaymodel.cpp \
    nes/debuggers/chrromdisplaydialog.cpp \
    nes/debuggers/prgromdisplaydialog.cpp \
    project/cprojecttreeview.cpp \
    project/cprojecttreeviewmodel.cpp \
    debuggers/cdebuggerregisterdisplaymodel.cpp \
    common/cthreadregistry.cpp \
    c64/compilers/cmachineimagebuilder.cpp \
    c64/debuggers/dbg_cc64.cpp

HEADERS += \
    aboutdialog.h \
    emulator/cbreakpointinfo.h \
    common/cbuildertextlogger.h \
    common/cdesignercommon.h \
    common/cdockwidgetregistry.h \
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
    common/searchdockwidget.h \
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
    nes/emulator/emulatorprefsdialog.h \
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
    project/cprojecttreeview.h \
    project/cprojecttreeviewmodel.h \
    debuggers/cdebuggerregisterdisplaymodel.h \
    common/cthreadregistry.h \
    c64/compilers/cmachineimagebuilder.h \
    c64/debuggers/dbg_cc64.h

FORMS += \
    aboutdialog.ui \
    nes/common/chrbankitemstabwidget.ui \
    nes/emulator/nesemulatorcontrol.ui \
    c64/emulator/c64emulatorcontrol.ui \
    common/panzoomrenderer.ui \
    common/searchbar.ui \
    common/searchdockwidget.ui \
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
    nes/emulator/emulatorprefsdialog.ui \
    environmentsettingsdialog.ui \
    mainwindow.ui \
    outputpanedockwidget.ui \
    project/projectbrowserdockwidget.ui \
    startupsplashdialog.ui \
    nes/emulator/testsuiteexecutivedialog.ui \
    nes/debuggers/chrromdisplaydialog.ui \
    nes/debuggers/prgromdisplaydialog.ui \

RESOURCES += resource.qrc
