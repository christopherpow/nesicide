# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
   opengl \
   webkit \
   xml

TARGET = "nesicide"

isEmpty (NESICIDE_LIBS) {
   NESICIDE_LIBS = -lnesicide-emulator
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

   NESICIDE_CXXFLAGS = -I../nesicide-emulator-lib -I../nesicide-emulator-lib/emulator -I../nesicide-emulator-lib/common

   QMAKE_LFLAGS += -static-libgcc

   CONFIG(release, debug|release) {
      NESICIDE_LIBS = -L../nesicide-emulator-lib-build-desktop/release -lnesicide-emulator
   } else {
      NESICIDE_LIBS = -L../nesicide-emulator-lib-build-desktop/debug -lnesicide-emulator
   }
}

mac {
   NESICIDE_CXXFLAGS = -I ../nesicide-emulator-lib -I ../nesicide-emulator-lib/emulator -I ../nesicide-emulator-lib/common
   NESICIDE_LIBS = -L../nesicide-emulator-lib-build-desktop -lnesicide-emulator

   SDL_CXXFLAGS = -framework SDL
   SDL_LIBS = -framework SDL

   LUA_CXXFLAGS = -F.. -framework Lua
   LUA_LIBS = -F.. -framework Lua

   TARGET = "NESICIDE"

   QMAKE_POST_LINK += mkdir -p $$TARGET.app/Contents/Frameworks $$escape_expand(\n\t)
   QMAKE_POST_LINK += cp ../nesicide-emulator-lib-build-desktop/libnesicide-emulator.1.0.0.dylib \
      $$TARGET.app/Contents/Frameworks/libnesicide-emulator.1.dylib $$escape_expand(\n\t)
   QMAKE_POST_LINK += install_name_tool -change libnesicide-emulator.1.dylib \
      @executable_path/../Frameworks/libnesicide-emulator.1.dylib \
      $$TARGET.app/Contents/MacOS/NESICIDE $$escape_expand(\n\t)
   QMAKE_POST_LINK += cp -r ../Lua.framework \
      $$TARGET.app/Contents/Frameworks/ $$escape_expand(\n\t)
}

unix:!mac {
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

QMAKE_CXXFLAGS += $$NESICIDE_CXXFLAGS $$SDL_CXXFLAGS $$LUA_CXXFLAGS $$SCINTILLA_CXXFLAGS
LIBS += $$NESICIDE_LIBS $$SDL_LIBS $$LUA_LIBS $$SCINTILLA_LIBS

INCLUDEPATH += common \
    compiler \
    compiler/cc65 \
    debugger \
    designers/ \
    designers/attribute_table_editor \
    designers/cartridge_editor \
    designers/code_editor \
    designers/new_project \
    designers/project_properties \
    designers/graphics_bank_editor \
    emulator \
    interfaces \
    project \
    plugins \
    resources \
    viewers \
    viewers/chr-rom \
    viewers/debugger \
    viewers/emulator \
    viewers/prg-rom \
    viewers/project_treeview

SOURCES += mainwindow.cpp \
    main.cpp \
    common/qtcolorpicker.cpp \
    common/cgltexturemanager.cpp \
    compiler/csourceassembler.cpp \
    compiler/cgraphicsassembler.cpp \
    compiler/ccartridgebuilder.cpp \
    designers/code_editor/codeeditorform.cpp \
    designers/new_project/newprojectdialog.cpp \
    designers/project_properties/projectpropertiesdialog.cpp \
    project/csources.cpp \
    project/csourceitem.cpp \
    project/cprojectprimitives.cpp \
    project/cproject.cpp \
    project/cprgrombanks.cpp \
    project/cprgrombank.cpp \
    project/cnesicideproject.cpp \
    project/cchrrombanks.cpp \
    project/cchrrombank.cpp \
    project/ccartridge.cpp \
    project/cbinaryfiles.cpp \
    viewers/chr-rom/chrromdisplaydialog.cpp \
    viewers/chr-rom/cchrrompreviewrenderer.cpp \
    viewers/emulator/nesemulatorrenderer.cpp \
    viewers/prg-rom/prgromdisplaydialog.cpp \
    viewers/project_treeview/cprojecttreeviewmodel.cpp \
    viewers/project_treeview/cprojecttreeview.cpp \
    common/cbuildertextlogger.cpp \
    debugger/chrmeminspector.cpp \
    debugger/coampreviewrenderer.cpp \
    debugger/cnametablepreviewrenderer.cpp \
    project/cbinaryfile.cpp \
    project/cgraphicsbanks.cpp \
    project/cgraphicsbank.cpp \
    designers/graphics_bank_editor/graphicsbankeditorform.cpp \
    viewers/debugger/cdebuggerexecutiontracermodel.cpp \
    viewers/debugger/cdebuggermemorydisplaymodel.cpp \
    viewers/debugger/cdebuggerregisterdisplaymodel.cpp \
    viewers/debugger/cdebuggerregistercomboboxdelegate.cpp \
    debugger/breakpointdialog.cpp \
    viewers/debugger/cbreakpointdisplaymodel.cpp \
    debugger/breakpointwatcherthread.cpp \
    emulator/nesemulatorthread.cpp \
    viewers/debugger/ccodebrowserdisplaymodel.cpp \
    aboutdialog.cpp \
    viewers/chr-rom/cchrromitemlistdisplaymodel.cpp \
    debugger/ccodedataloggerrenderer.cpp \
    project/cattributetables.cpp \
    debugger/cexecutionvisualizerrenderer.cpp \
    common/cgamedatabasehandler.cpp \
    environmentsettingsdialog.cpp \
    plugins/cpluginmanager.cpp \
    startupsplashdialog.cpp \
    debugger/dbg_cnes6502.cpp \
    debugger/dbg_cnesmappers.cpp \
    debugger/dbg_cnesppu.cpp \
    debugger/dbg_cnes.cpp \
    debugger/dbg_cnesapu.cpp \
    debugger/dbg_cnesrom.cpp \
    debugger/dbg_cnesrommapper004.cpp \
    debugger/dbg_cnesrommapper001.cpp \
    compiler/compilerthread.cpp \
    emulatorprefsdialog.cpp \
    qkeymapitemedit.cpp \
    project/cprojectbase.cpp \
    designers/attribute_table_editor/attributetableeditorform.cpp \
    project/cattributetable.cpp \
    common/sourcenavigator.cpp \
    projectbrowserdockwidget.cpp \
    debugger/memoryinspectordockwidget.cpp \
    debugger/registerinspectordockwidget.cpp \
    debugger/executioninspectordockwidget.cpp \
    debugger/codebrowserdockwidget.cpp \
    debugger/codedataloggerdockwidget.cpp \
    debugger/apuinformationdockwidget.cpp \
    debugger/ppuinformationdockwidget.cpp \
    debugger/executionvisualizerdockwidget.cpp \
    debugger/mapperinformationdockwidget.cpp \
    debugger/nametablevisualizerdockwidget.cpp \
    debugger/oamvisualizerdockwidget.cpp \
    outputpanedockwidget.cpp \
    debugger/breakpointdockwidget.cpp \
    emulator/nesemulatordockwidget.cpp \
    common/cdockwidgetregistry.cpp \
    compiler/cc65/ccc65interface.cpp \
    compiler/cc65/dbginfo.c \
    common/xmlhelpers.cpp \
    testsuiteexecutivedialog.cpp \
    designers/code_editor/qscilexerca65.cpp \
    designers/cdesignereditorbase.cpp \
    common/emulatorcontrol.cpp \
    debugger/symbolwatchdockwidget.cpp \
    viewers/debugger/csymbolwatchmodel.cpp \
    viewers/debugger/cdebuggernumericitemdelegate.cpp \
    viewers/debugger/cdebuggersymboldelegate.cpp \
    common/cprojecttabwidget.cpp \
    designers/graphics_bank_editor/cchrrombankitemdelegate.cpp \
    version.cpp \
    debugger/cdebuggerbase.cpp \
    debugger/executionmarkerdockwidget.cpp \
    viewers/debugger/cexecutionmarkerdisplaymodel.cpp

HEADERS += mainwindow.h \
    main.h \
    common/qtcolorpicker.h \
    common/cpaletteitemdelegate.h \
    common/cgltexturemanager.h \
    compiler/csourceassembler.h \
    compiler/cgraphicsassembler.h \
    compiler/ccartridgebuilder.h \
    designers/code_editor/codeeditorform.h \
    designers/new_project/newprojectdialog.h \
    designers/project_properties/projectpropertiesdialog.h \
    interfaces/ixmlserializable.h \
    interfaces/iprojecttreeviewitem.h \
    project/csources.h \
    project/csourceitem.h \
    project/cprojectprimitives.h \
    project/cproject.h \
    project/cprgrombanks.h \
    project/cprgrombank.h \
    project/cnesicideproject.h \
    project/cchrrombanks.h \
    project/cchrrombank.h \
    project/ccartridge.h \
    project/cbinaryfiles.h \
    viewers/chr-rom/chrromdisplaydialog.h \
    viewers/chr-rom/cchrrompreviewrenderer.h \
    viewers/emulator/nesemulatorrenderer.h \
    viewers/prg-rom/prgromdisplaydialog.h \
    viewers/project_treeview/cprojecttreeviewmodel.h \
    viewers/project_treeview/cprojecttreeview.h \
    common/cbuildertextlogger.h \
    debugger/chrmeminspector.h \
    debugger/coampreviewrenderer.h \
    debugger/cnametablepreviewrenderer.h \
    emulator/nesemulatorthread.h \
    project/cbinaryfile.h \
    project/cgraphicsbanks.h \
    project/cgraphicsbank.h \
    designers/graphics_bank_editor/graphicsbankeditorform.h \
    viewers/debugger/cdebuggerexecutiontracermodel.h \
    viewers/debugger/cdebuggermemorydisplaymodel.h \
    viewers/debugger/cdebuggerregisterdisplaymodel.h \
    viewers/debugger/cdebuggerregistercomboboxdelegate.h \
    debugger/breakpointdialog.h \
    viewers/debugger/cbreakpointdisplaymodel.h \
    debugger/breakpointwatcherthread.h \
    viewers/debugger/ccodebrowserdisplaymodel.h \
    aboutdialog.h \
    interfaces/ichrrombankitem.h \
    viewers/chr-rom/cchrromitemlistdisplaymodel.h \
    debugger/ccodedataloggerrenderer.h \
    project/cattributetables.h \
    debugger/cexecutionvisualizerrenderer.h \
    common/cgamedatabasehandler.h \
    environmentsettingsdialog.h \
    plugins/cpluginmanager.h \
    startupsplashdialog.h \
    debugger/dbg_cnes6502.h \
    debugger/dbg_cnesmappers.h \
    debugger/dbg_cnesppu.h \
    debugger/dbg_cnes.h \
    debugger/dbg_cnesrom.h \
    debugger/dbg_cnesapu.h \
    debugger/dbg_cnesrommapper001.h \
    debugger/dbg_cnesrommapper004.h \
    compiler/compilerthread.h \
    emulatorprefsdialog.h \
    qkeymapitemedit.h \
    project/cprojectbase.h \
    designers/attribute_table_editor/attributetableeditorform.h \
    project/cattributetable.h \
    common/sourcenavigator.h \
    projectbrowserdockwidget.h \
    debugger/memoryinspectordockwidget.h \
    debugger/registerinspectordockwidget.h \
    debugger/executioninspectordockwidget.h \
    debugger/codebrowserdockwidget.h \
    debugger/codedataloggerdockwidget.h \
    debugger/apuinformationdockwidget.h \
    debugger/ppuinformationdockwidget.h \
    debugger/executionvisualizerdockwidget.h \
    debugger/mapperinformationdockwidget.h \
    debugger/nametablevisualizerdockwidget.h \
    debugger/oamvisualizerdockwidget.h \
    outputpanedockwidget.h \
    debugger/breakpointdockwidget.h \
    emulator/nesemulatordockwidget.h \
    common/cdockwidgetregistry.h \
    compiler/cc65/ccc65interface.h \
    compiler/cc65/dbginfo.h \
    testsuiteexecutivedialog.h \
    designers/code_editor/qscilexerca65.h \
    designers/cdesignereditorbase.h \
    interfaces/icenterwidgetitem.h \
    common/emulatorcontrol.h \
    debugger/symbolwatchdockwidget.h \
    viewers/debugger/csymbolwatchmodel.h \
    viewers/debugger/cdebuggernumericitemdelegate.h \
    viewers/debugger/cdebuggersymboldelegate.h \
    common/cprojecttabwidget.h \
    designers/graphics_bank_editor/cchrrombankitemdelegate.h \
    debugger/cdebuggerbase.h \
    debugger/executionmarkerdockwidget.h \
    viewers/debugger/cexecutionmarkerdisplaymodel.h

FORMS += mainwindow.ui \
    designers/code_editor/codeeditorform.ui \
    designers/new_project/newprojectdialog.ui \
    designers/project_properties/projectpropertiesdialog.ui \
    viewers/chr-rom/chrromdisplaydialog.ui \
    viewers/prg-rom/prgromdisplaydialog.ui \
    designers/graphics_bank_editor/graphicsbankeditorform.ui \
    debugger/breakpointdialog.ui \
    aboutdialog.ui \
    environmentsettingsdialog.ui \
    startupsplashdialog.ui \
    emulatorprefsdialog.ui \
    designers/attribute_table_editor/attributetableeditorform.ui \
    common/sourcenavigator.ui \
    projectbrowserdockwidget.ui \
    debugger/memoryinspectordockwidget.ui \
    debugger/registerinspectordockwidget.ui \
    debugger/executioninspectordockwidget.ui \
    debugger/codebrowserdockwidget.ui \
    debugger/codedataloggerdockwidget.ui \
    debugger/apuinformationdockwidget.ui \
    debugger/ppuinformationdockwidget.ui \
    debugger/executionvisualizerdockwidget.ui \
    debugger/mapperinformationdockwidget.ui \
    debugger/nametablevisualizerdockwidget.ui \
    debugger/oamvisualizerdockwidget.ui \
    outputpanedockwidget.ui \
    debugger/breakpointdockwidget.ui \
    emulator/nesemulatordockwidget.ui \
    testsuiteexecutivedialog.ui \
    common/emulatorcontrol.ui \
    debugger/symbolwatchdockwidget.ui \
    debugger/executionmarkerdockwidget.ui

RESOURCES += resource.qrc
