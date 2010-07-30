# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
    opengl \
    webkit \
    xml

DEFINES += IDE_BUILD

system(cd compiler && make clean && make )
win32:system(cd lua && make mingw)
unix:!mac:system(cd lua && make linux)
mac:system(cd lua && make macosx)
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
   TARGET = debug_binary
} else {
   TARGET = release_binary
}

win32:QMAKE_LFLAGS += -static-libgcc
win32:LIBS += ../nesicide2-master/compiler/libpasm.a
unix:LIBS += compiler/libpasm.a
win32:LIBS += ../nesicide2-master/lua/liblua.a
unix:LIBS += lua/liblua.a
win32:LIBS += -L./libraries/SDL/ \
    -lsdl
unix:!mac:LIBS += `sdl-config \
    --libs`
mac:LIBS += -framework \
    SDL
win32:INCLUDEPATH = ./ \
    ./libraries/SDL
unix:INCLUDEPATH = ./ \
    /usr/include/SDL
mac:INCLUDEPATH = ./ \
    ./libraries/SDL
INCLUDEPATH += ./common \
    ../nesicide2-emulator/common \
    ./compiler \
    ./lua \
    ./debugger \
    ./designers/cartridge_editor \
    ./designers/code_editor \
    ./designers/new_project \
    ./designers/project_properties \
    ./designers/graphics_bank_editor \
    ./emulator \
    ../nesicide2-emulator/emulator \
    ./interfaces \
    ./project \
    ./plugins \
    ./resources \
    ./viewers \
    ./viewers/chr-rom \
    ./viewers/debugger \
    ./viewers/emulator \
    ../nesicide2-emulator/viewers/emulator \
    ./viewers/prg-rom \
    ./viewers/project_treeview
unix:INCLUDEPATH += /usr/include/SDL
mac:INCLUDEPATH += /Library/Frameworks/SDL.framework/Headers
TARGET = nesicide2
TEMPLATE = app
SOURCES += mainwindow.cpp \
    main.cpp \
    common/qtcolorpicker.cpp \
    ../nesicide2-emulator/common/cnessystempalette.cpp \
    common/cgltexturemanager.cpp \
    compiler/csourceassembler.cpp \
    compiler/cgraphicsassembler.cpp \
    compiler/ccartridgebuilder.cpp \
    designers/code_editor/csyntaxhighlighter.cpp \
    designers/code_editor/csyntaxhighlightedtextedit.cpp \
    designers/code_editor/codeeditorform.cpp \
    designers/new_project/newprojectdialog.cpp \
    designers/project_properties/projectpropertiesdialog.cpp \
    emulator/ctracer.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper068.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper065.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper011.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper010.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper009.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper007.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper005.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper004.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper003.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper002.cpp \
    ../nesicide2-emulator/emulator/cnesrommapper001.cpp \
    ../nesicide2-emulator/emulator/cnesrom.cpp \
    ../nesicide2-emulator/emulator/cnesppu.cpp \
    ../nesicide2-emulator/emulator/cnesmappers.cpp \
    ../nesicide2-emulator/emulator/cnesio.cpp \
    ../nesicide2-emulator/emulator/cnesapu.cpp \
    ../nesicide2-emulator/emulator/cnes6502.cpp \
    ../nesicide2-emulator/emulator/cnes.cpp \
    emulator/cjoypadlogger.cpp \
    emulator/ccodedatalogger.cpp \
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
    ../nesicide2-emulator/viewers/emulator/nesemulatorrenderer.cpp \
    viewers/emulator/nesemulatordialog.cpp \
    viewers/prg-rom/prgromdisplaydialog.cpp \
    viewers/project_treeview/cprojecttreeviewmodel.cpp \
    viewers/project_treeview/cprojecttreeview.cpp \
    common/cbuildertextlogger.cpp \
    debugger/chrmeminspector.cpp \
    debugger/oaminspector.cpp \
    debugger/oamdisplaydialog.cpp \
    debugger/coampreviewrenderer.cpp \
    debugger/nametabledisplaydialog.cpp \
    debugger/cnametablepreviewrenderer.cpp \
    debugger/nametableinspector.cpp \
    debugger/executiontracerdialog.cpp \
    project/cbinaryfile.cpp \
    project/cgraphics.cpp \
    project/cgraphicsbanks.cpp \
    project/cgraphicsbank.cpp \
    designers/graphics_bank_editor/graphicsbankeditorform.cpp \
    viewers/debugger/cdebuggerexecutiontracermodel.cpp \
    debugger/executioninspector.cpp \
    debugger/memorydisplaydialog.cpp \
    debugger/memoryinspector.cpp \
    viewers/debugger/cdebuggermemorydisplaymodel.cpp \
    debugger/registerdisplaydialog.cpp \
    viewers/debugger/cdebuggerregisterdisplaymodel.cpp \
    debugger/registerinspector.cpp \
    viewers/debugger/cdebuggerregistercomboboxdelegate.cpp \
    debugger/breakpointdialog.cpp \
    debugger/breakpointinspector.cpp \
    debugger/cbreakpointinfo.cpp \
    viewers/debugger/cbreakpointdisplaymodel.cpp \
    debugger/breakpointwatcherthread.cpp \
    ../nesicide2-emulator/emulator/nesemulatorthread.cpp \
    debugger/cregisterdata.cpp \
    debugger/codebrowserdialog.cpp \
    debugger/codeinspector.cpp \
    viewers/debugger/ccodebrowserdisplaymodel.cpp \
    common/inspectorregistry.cpp \
    viewers/debugger/csourcebrowserdisplaymodel.cpp \
    aboutdialog.cpp \
    designers/graphics_bank_editor/graphicsbankadditemsdialog.cpp \
    viewers/chr-rom/cchrromitemlistdisplaymodel.cpp \
    debugger/ppuinformationdialog.cpp \
    debugger/ppuinformationinspector.cpp \
    debugger/codedataloggerdialog.cpp \
    debugger/codedataloggerinspector.cpp \
    debugger/ccodedataloggerrenderer.cpp \
    project/cattributetables.cpp \
    debugger/executionvisualizer.cpp \
    debugger/executionvisualizerdialog.cpp \
    debugger/cexecutionvisualizerrenderer.cpp \
    emulator/cmarker.cpp \
    debugger/mapperinformationdialog.cpp \
    debugger/mapperinformationinspector.cpp \
    debugger/apuinformationdialog.cpp \
    debugger/apuinformationinspector.cpp \
    common/cgamedatabasehandler.cpp \
    common/cconfigurator.cpp \
    environmentsettingsdialog.cpp \
    plugins/cpluginmanager.cpp \
    startupsplashdialog.cpp
HEADERS += mainwindow.h \
    main.h \
    common/qtcolorpicker.h \
    common/defaultnespalette.h \
    common/cpaletteitemdelegate.h \
    ../nesicide2-emulator/common/cnessystempalette.h \
    common/cnesicidecommon.h \
    common/cgltexturemanager.h \
    compiler/csourceassembler.h \
    compiler/cgraphicsassembler.h \
    compiler/ccartridgebuilder.h \
    designers/code_editor/csyntaxhighlighter.h \
    designers/code_editor/csyntaxhighlightedtextedit.h \
    designers/code_editor/codeeditorform.h \
    designers/new_project/newprojectdialog.h \
    designers/project_properties/projectpropertiesdialog.h \
    emulator/ctracer.h \
    ../nesicide2-emulator/emulator/cnesrommapper068.h \
    ../nesicide2-emulator/emulator/cnesrommapper065.h \
    ../nesicide2-emulator/emulator/cnesrommapper011.h \
    ../nesicide2-emulator/emulator/cnesrommapper010.h \
    ../nesicide2-emulator/emulator/cnesrommapper009.h \
    ../nesicide2-emulator/emulator/cnesrommapper007.h \
    ../nesicide2-emulator/emulator/cnesrommapper005.h \
    ../nesicide2-emulator/emulator/cnesrommapper004.h \
    ../nesicide2-emulator/emulator/cnesrommapper003.h \
    ../nesicide2-emulator/emulator/cnesrommapper002.h \
    ../nesicide2-emulator/emulator/cnesrommapper001.h \
    ../nesicide2-emulator/emulator/cnesrom.h \
    ../nesicide2-emulator/emulator/cnesppu.h \
    ../nesicide2-emulator/emulator/cnesmappers.h \
    ../nesicide2-emulator/emulator/cnesio.h \
    ../nesicide2-emulator/emulator/cnesapu.h \
    ../nesicide2-emulator/emulator/cnes6502.h \
    ../nesicide2-emulator/emulator/cnes.h \
    emulator/cjoypadlogger.h \
    emulator/ccodedatalogger.h \
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
    ../nesicide2-emulator/viewers/emulator/nesemulatorrenderer.h \
    viewers/emulator/nesemulatordialog.h \
    viewers/prg-rom/prgromdisplaydialog.h \
    viewers/project_treeview/cprojecttreeviewmodel.h \
    viewers/project_treeview/cprojecttreeview.h \
    common/cbuildertextlogger.h \
    debugger/chrmeminspector.h \
    debugger/oaminspector.h \
    debugger/oamdisplaydialog.h \
    debugger/coampreviewrenderer.h \
    debugger/nametabledisplaydialog.h \
    debugger/cnametablepreviewrenderer.h \
    debugger/nametableinspector.h \
    debugger/executiontracerdialog.h \
    project/cbinaryfile.h \
    project/cgraphics.h \
    project/cgraphicsbanks.h \
    project/cgraphicsbank.h \
    designers/graphics_bank_editor/graphicsbankeditorform.h \
    viewers/debugger/cdebuggerexecutiontracermodel.h \
    debugger/executioninspector.h \
    debugger/memorydisplaydialog.h \
    debugger/memoryinspector.h \
    viewers/debugger/cdebuggermemorydisplaymodel.h \
    debugger/registerdisplaydialog.h \
    viewers/debugger/cdebuggerregisterdisplaymodel.h \
    debugger/registerinspector.h \
    viewers/debugger/cdebuggerregistercomboboxdelegate.h \
    debugger/breakpointdialog.h \
    debugger/breakpointinspector.h \
    debugger/cbreakpointinfo.h \
    viewers/debugger/cbreakpointdisplaymodel.h \
    debugger/breakpointwatcherthread.h \
    ../nesicide2-emulator/emulator/nesemulatorthread.h \
    debugger/cregisterdata.h \
    debugger/codebrowserdialog.h \
    debugger/codeinspector.h \
    viewers/debugger/ccodebrowserdisplaymodel.h \
    common/inspectorregistry.h \
    compiler/pasm_types.h \
    compiler/pasm_lib.h \
    viewers/debugger/csourcebrowserdisplaymodel.h \
    aboutdialog.h \
    interfaces/ichrrombankitem.h \
    designers/graphics_bank_editor/graphicsbankadditemsdialog.h \
    viewers/chr-rom/cchrromitemlistdisplaymodel.h \
    debugger/ppuinformationdialog.h \
    debugger/ppuinformationinspector.h \
    debugger/codedataloggerdialog.h \
    debugger/codedataloggerinspector.h \
    debugger/ccodedataloggerrenderer.h \
    project/cattributetables.h \
    debugger/executionvisualizer.h \
    debugger/executionvisualizerdialog.h \
    debugger/cexecutionvisualizerrenderer.h \
    emulator/cmarker.h \
    debugger/mapperinformationdialog.h \
    debugger/mapperinformationinspector.h \
    debugger/apuinformationdialog.h \
    debugger/apuinformationinspector.h \
    common/cgamedatabasehandler.h \
    common/cconfigurator.h \
    environmentsettingsdialog.h \
    plugins/cpluginmanager.h \
    startupsplashdialog.h
FORMS += mainwindow.ui \
    designers/code_editor/codeeditorform.ui \
    designers/new_project/newprojectdialog.ui \
    designers/project_properties/projectpropertiesdialog.ui \
    viewers/chr-rom/chrromdisplaydialog.ui \
    viewers/emulator/nesemulatordialog.ui \
    viewers/prg-rom/prgromdisplaydialog.ui \
    debugger/oamdisplaydialog.ui \
    debugger/nametabledisplaydialog.ui \
    debugger/executiontracerdialog.ui \
    designers/graphics_bank_editor/graphicsbankeditorform.ui \
    debugger/memorydisplaydialog.ui \
    debugger/registerdisplaydialog.ui \
    debugger/breakpointdialog.ui \
    debugger/codebrowserdialog.ui \
    aboutdialog.ui \
    designers/graphics_bank_editor/graphicsbankadditemsdialog.ui \
    debugger/ppuinformationdialog.ui \
    debugger/codedataloggerdialog.ui \
    debugger/executionvisualizerdialog.ui \
    debugger/mapperinformationdialog.ui \
    debugger/apuinformationdialog.ui \
    environmentsettingsdialog.ui \
    startupsplashdialog.ui
RESOURCES += resource.qrc
