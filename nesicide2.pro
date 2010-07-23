# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
    opengl \
    webkit \
    xml

# multimedia
win32:QMAKE_LFLAGS += -static-libgcc
win32:LIBS += compiler/libpasm.a
unix:LIBS += compiler/libpasm.a
win32:LIBS += lua/liblua.a
unix:LIBS += lua/liblua.a
mac:LIBS += lua/
win32:LIBS += -L./libraries/SDL/ \
    -lsdl
unix:LIBS += `sdl-config \
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
    ./compiler \
    ./lua \
    ./debugger \
    ./designers/cartridge_editor \
    ./designers/code_editor \
    ./designers/new_project \
    ./designers/project_properties \
    ./designers/graphics_bank_editor \
    ./emulator \
    ./interfaces \
    ./project \
    ./plugins \
    ./resources \
    ./viewers \
    ./viewers/chr-rom \
    ./viewers/debugger \
    ./viewers/emulator \
    ./viewers/prg-rom \
    ./viewers/project_treeview
unix:INCLUDEPATH += /usr/include/SDL
mac:INCLUDEPATH += /Library/Frameworks/SDL.framework/Headers
TARGET = nesicide2
TEMPLATE = app
SOURCES += mainwindow.cpp \
    main.cpp \
    common/qtcolorpicker.cpp \
    common/cnessystempalette.cpp \
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
    emulator/cnesrommapper068.cpp \
    emulator/cnesrommapper065.cpp \
    emulator/cnesrommapper011.cpp \
    emulator/cnesrommapper010.cpp \
    emulator/cnesrommapper009.cpp \
    emulator/cnesrommapper007.cpp \
    emulator/cnesrommapper005.cpp \
    emulator/cnesrommapper004.cpp \
    emulator/cnesrommapper003.cpp \
    emulator/cnesrommapper002.cpp \
    emulator/cnesrommapper001.cpp \
    emulator/cnesrom.cpp \
    emulator/cnesppu.cpp \
    emulator/cnesmappers.cpp \
    emulator/cnesio.cpp \
    emulator/cnesapu.cpp \
    emulator/cnes6502.cpp \
    emulator/cnes.cpp \
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
    viewers/emulator/nesemulatorrenderer.cpp \
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
    emulator/nesemulatorthread.cpp \
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
    plugins/cpluginmanager.cpp
HEADERS += mainwindow.h \
    main.h \
    common/qtcolorpicker.h \
    common/defaultnespalette.h \
    common/cpaletteitemdelegate.h \
    common/cnessystempalette.h \
    common/cnesicidecommon.h \
    common/cgltexturemanager.h \
    common/defaultnespalette.h \
    common/cpaletteitemdelegate.h \
    common/cnessystempalette.h \
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
    emulator/cnesrommapper068.h \
    emulator/cnesrommapper065.h \
    emulator/cnesrommapper011.h \
    emulator/cnesrommapper010.h \
    emulator/cnesrommapper009.h \
    emulator/cnesrommapper007.h \
    emulator/cnesrommapper005.h \
    emulator/cnesrommapper004.h \
    emulator/cnesrommapper003.h \
    emulator/cnesrommapper002.h \
    emulator/cnesrommapper001.h \
    emulator/cnesrom.h \
    emulator/cnesppu.h \
    emulator/cnesmappers.h \
    emulator/cnesio.h \
    emulator/cnesapu.h \
    emulator/cnes6502.h \
    emulator/cnes.h \
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
    viewers/emulator/nesemulatorrenderer.h \
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
    emulator/nesemulatorthread.h \
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
    plugins/cpluginmanager.h
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
    environmentsettingsdialog.ui
RESOURCES += resource.qrc
