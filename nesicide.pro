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
    designers/tile_stamp_editor \
    emulator \
    interfaces \
    project \
    plugins \
    resources \
    viewers \
    viewers/chr-rom \
    viewers/prg-rom \
    viewers/project_treeview

SOURCES += mainwindow.cpp \
    main.cpp \
    common/qtcolorpicker.cpp \
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
    viewers/prg-rom/prgromdisplaydialog.cpp \
    viewers/project_treeview/cprojecttreeviewmodel.cpp \
    viewers/project_treeview/cprojecttreeview.cpp \
    common/cbuildertextlogger.cpp \
    project/cbinaryfile.cpp \
    project/cgraphicsbanks.cpp \
    project/cgraphicsbank.cpp \
    designers/graphics_bank_editor/graphicsbankeditorform.cpp \
    aboutdialog.cpp \
    viewers/chr-rom/cchrromitemlistdisplaymodel.cpp \
    viewers/chr-rom/cchrromitemtabledisplaymodel.cpp \
    project/cattributetables.cpp \
    common/cgamedatabasehandler.cpp \
    environmentsettingsdialog.cpp \
    plugins/cpluginmanager.cpp \
    startupsplashdialog.cpp \
    compiler/compilerthread.cpp \
    emulatorprefsdialog.cpp \
    qkeymapitemedit.cpp \
    project/cprojectbase.cpp \
    designers/attribute_table_editor/attributetableeditorform.cpp \
    project/cattributetable.cpp \
    common/sourcenavigator.cpp \
    projectbrowserdockwidget.cpp \
    outputpanedockwidget.cpp \
    common/cdockwidgetregistry.cpp \
    compiler/cc65/ccc65interface.cpp \
    compiler/cc65/dbginfo.c \
    common/xmlhelpers.cpp \
    testsuiteexecutivedialog.cpp \
    designers/code_editor/qscilexerca65.cpp \
    designers/cdesignereditorbase.cpp \
    common/emulatorcontrol.cpp \
    common/cprojecttabwidget.cpp \
    designers/graphics_bank_editor/cchrrombankitemdelegate.cpp \
    version.cpp \
    common/searcherthread.cpp \
    common/searchbar.cpp \
    debugger/symbolwatchdockwidget.cpp \
    debugger/registerinspectordockwidget.cpp \
    debugger/ppuinformationdockwidget.cpp \
    debugger/oamvisualizerdockwidget.cpp \
    debugger/nametablevisualizerdockwidget.cpp \
    debugger/memoryinspectordockwidget.cpp \
    debugger/mapperinformationdockwidget.cpp \
    debugger/executionvisualizerdockwidget.cpp \
    debugger/executioninspectordockwidget.cpp \
    debugger/dbg_cnesrom.cpp \
    debugger/dbg_cnesppu.cpp \
    debugger/dbg_cnesmappers.cpp \
    debugger/dbg_cnesapu.cpp \
    debugger/dbg_cnes6502.cpp \
    debugger/dbg_cnes.cpp \
    debugger/csymbolwatchmodel.cpp \
    debugger/codedataloggerdockwidget.cpp \
    debugger/codebrowserdockwidget.cpp \
    debugger/chrmeminspector.cpp \
    debugger/cexecutionmarkerdisplaymodel.cpp \
    debugger/cdebuggersymboldelegate.cpp \
    debugger/cdebuggerregisterdisplaymodel.cpp \
    debugger/cdebuggerregistercomboboxdelegate.cpp \
    debugger/cdebuggernumericitemdelegate.cpp \
    debugger/cdebuggermemorydisplaymodel.cpp \
    debugger/cdebuggerexecutiontracermodel.cpp \
    debugger/cdebuggerbase.cpp \
    debugger/ccodebrowserdisplaymodel.cpp \
    debugger/cbreakpointdisplaymodel.cpp \
    debugger/breakpointwatcherthread.cpp \
    debugger/breakpointdockwidget.cpp \
    debugger/breakpointdialog.cpp \
    debugger/apuinformationdockwidget.cpp \
    emulator/nesemulatorthread.cpp \
    emulator/nesemulatordockwidget.cpp \
    common/searchdockwidget.cpp \
    common/cimageconverters.cpp \
    debugger/codeprofilerdockwidget.cpp \
    debugger/cdebuggercodeprofilermodel.cpp \
    designers/code_editor/qscilexerdefault.cpp \
    designers/tile_stamp_editor/ctilestamprenderer.cpp \
    designers/tile_stamp_editor/tilestampeditorform.cpp \
    project/ctilestamps.cpp \
    project/ctilestamp.cpp \
    common/colorpushbutton.cpp \
    designers/project_properties/propertyeditordialog.cpp \
    designers/project_properties/cpropertylistmodel.cpp \
    designers/project_properties/cpropertyenumlistmodel.cpp \
    designers/project_properties/cpropertyvaluedelegate.cpp \
    designers/project_properties/cpropertysymboldelegate.cpp \
    common/cpropertyitem.cpp \
    common/checkboxlist.cpp \
    debugger/debuggerupdatethread.cpp \
    common/crendererbase.cpp \
    common/panzoomrenderer.cpp

HEADERS += mainwindow.h \
    main.h \
    common/qtcolorpicker.h \
    common/cpaletteitemdelegate.h \
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
    viewers/prg-rom/prgromdisplaydialog.h \
    viewers/project_treeview/cprojecttreeviewmodel.h \
    viewers/project_treeview/cprojecttreeview.h \
    common/cbuildertextlogger.h \
    project/cbinaryfile.h \
    project/cgraphicsbanks.h \
    project/cgraphicsbank.h \
    designers/graphics_bank_editor/graphicsbankeditorform.h \
    aboutdialog.h \
    interfaces/ichrrombankitem.h \
    viewers/chr-rom/cchrromitemlistdisplaymodel.h \
    viewers/chr-rom/cchrromitemtabledisplaymodel.h \
    project/cattributetables.h \
    common/cgamedatabasehandler.h \
    environmentsettingsdialog.h \
    plugins/cpluginmanager.h \
    startupsplashdialog.h \
    compiler/compilerthread.h \
    emulatorprefsdialog.h \
    qkeymapitemedit.h \
    project/cprojectbase.h \
    designers/attribute_table_editor/attributetableeditorform.h \
    project/cattributetable.h \
    common/sourcenavigator.h \
    projectbrowserdockwidget.h \
    outputpanedockwidget.h \
    common/cdockwidgetregistry.h \
    compiler/cc65/ccc65interface.h \
    compiler/cc65/dbginfo.h \
    testsuiteexecutivedialog.h \
    designers/code_editor/qscilexerca65.h \
    designers/cdesignereditorbase.h \
    interfaces/icenterwidgetitem.h \
    common/emulatorcontrol.h \
    common/cprojecttabwidget.h \
    designers/graphics_bank_editor/cchrrombankitemdelegate.h \
    common/searcherthread.h \
    common/searchbar.h \
    debugger/symbolwatchdockwidget.h \
    debugger/registerinspectordockwidget.h \
    debugger/ppuinformationdockwidget.h \
    debugger/oamvisualizerdockwidget.h \
    debugger/nametablevisualizerdockwidget.h \
    debugger/memoryinspectordockwidget.h \
    debugger/mapperinformationdockwidget.h \
    debugger/executionvisualizerdockwidget.h \
    debugger/executioninspectordockwidget.h \
    debugger/dbg_cnesrom.h \
    debugger/dbg_cnesppu.h \
    debugger/dbg_cnesmappers.h \
    debugger/dbg_cnesapu.h \
    debugger/dbg_cnes6502.h \
    debugger/dbg_cnes.h \
    debugger/csymbolwatchmodel.h \
    debugger/codedataloggerdockwidget.h \
    debugger/codebrowserdockwidget.h \
    debugger/chrmeminspector.h \
    debugger/cexecutionmarkerdisplaymodel.h \
    debugger/cdebuggersymboldelegate.h \
    debugger/cdebuggerregisterdisplaymodel.h \
    debugger/cdebuggerregistercomboboxdelegate.h \
    debugger/cdebuggernumericitemdelegate.h \
    debugger/cdebuggermemorydisplaymodel.h \
    debugger/cdebuggerexecutiontracermodel.h \
    debugger/cdebuggerbase.h \
    debugger/ccodebrowserdisplaymodel.h \
    debugger/cbreakpointdisplaymodel.h \
    debugger/breakpointwatcherthread.h \
    debugger/breakpointdockwidget.h \
    debugger/breakpointdialog.h \
    debugger/apuinformationdockwidget.h \
    emulator/nesemulatorthread.h \
    emulator/nesemulatordockwidget.h \
    common/searchdockwidget.h \
    common/cimageconverters.h \
    debugger/codeprofilerdockwidget.h \
    debugger/cdebuggercodeprofilermodel.h \
    designers/code_editor/qscilexerdefault.h \
    designers/tile_stamp_editor/ctilestamprenderer.h \
    designers/tile_stamp_editor/tilestampeditorform.h \
    project/ctilestamps.h \
    project/ctilestamp.h \
    common/colorpushbutton.h \
    common/cdesignercommon.h \
    designers/project_properties/propertyeditordialog.h \
    designers/project_properties/cpropertylistmodel.h \
    common/cpropertyitem.h \
    designers/project_properties/cpropertyenumlistmodel.h \
    designers/project_properties/cpropertyvaluedelegate.h \
    designers/project_properties/cpropertysymboldelegate.h \
    common/checkboxlist.h \
    debugger/debuggerupdatethread.h \
    common/crendererbase.h \
    common/panzoomrenderer.h

FORMS += mainwindow.ui \
    designers/code_editor/codeeditorform.ui \
    designers/new_project/newprojectdialog.ui \
    designers/project_properties/projectpropertiesdialog.ui \
    viewers/chr-rom/chrromdisplaydialog.ui \
    viewers/prg-rom/prgromdisplaydialog.ui \
    designers/graphics_bank_editor/graphicsbankeditorform.ui \
    aboutdialog.ui \
    environmentsettingsdialog.ui \
    startupsplashdialog.ui \
    emulatorprefsdialog.ui \
    designers/attribute_table_editor/attributetableeditorform.ui \
    common/sourcenavigator.ui \
    projectbrowserdockwidget.ui \
    outputpanedockwidget.ui \
    testsuiteexecutivedialog.ui \
    common/emulatorcontrol.ui \
    common/searchbar.ui \
    debugger/symbolwatchdockwidget.ui \
    debugger/registerinspectordockwidget.ui \
    debugger/ppuinformationdockwidget.ui \
    debugger/oamvisualizerdockwidget.ui \
    debugger/nametablevisualizerdockwidget.ui \
    debugger/memoryinspectordockwidget.ui \
    debugger/mapperinformationdockwidget.ui \
    debugger/executionvisualizerdockwidget.ui \
    debugger/executioninspectordockwidget.ui \
    debugger/codedataloggerdockwidget.ui \
    debugger/codebrowserdockwidget.ui \
    debugger/breakpointdockwidget.ui \
    debugger/breakpointdialog.ui \
    debugger/apuinformationdockwidget.ui \
    emulator/nesemulatordockwidget.ui \
    common/searchdockwidget.ui \
    debugger/codeprofilerdockwidget.ui \
    designers/tile_stamp_editor/tilestampeditorform.ui \
    designers/project_properties/propertyeditordialog.ui \
    common/panzoomrenderer.ui

RESOURCES += resource.qrc
