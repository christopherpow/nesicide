# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
    opengl \
    webkit \
    xml \
    multimedia
INCLUDEPATH = ./emulator \
              ./debugger \
              ./compiler \
              ./designers
TARGET = nesicide2
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    cnesicideproject.cpp \
    cprojecttreeviewmodel.cpp \
    cprojecttreeview.cpp \
    cprojectprimitives.cpp \
    cprojectpalettes.cpp \
    cprojectpaletteitem.cpp \
    paletteeditorwindow.cpp \
    projectpropertiesdialog.cpp \
    newprojectdialog.cpp \
    ccartridge.cpp \
    cprgrombanks.cpp \
    cprgrombank.cpp \
    prgromdisplaydialog.cpp \
    cchrrombanks.cpp \
    cchrrombank.cpp \
    chrromdisplaydialog.cpp \
    cchrrompreviewrenderer.cpp \
    qtcolorpicker.cpp \
    nesemulatordialog.cpp \
    ctracer.cpp \
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
    emulator/cnes.cpp \
    emulator/cnesmappers.cpp \
    cjoypadlogger.cpp \
    emulator/cnesio.cpp \
    ccodedatalogger.cpp \
    emulator/cnesapu.cpp \
    emulator/cnes6502.cpp \
    cnessystempalette.cpp \
    nesemulatorrenderer.cpp \
    nesemulatorthread.cpp \
    cgltexturemanager.cpp \
    cproject.cpp \
    csources.cpp \
    csourceitem.cpp \
    codeeditorform.cpp \
    csyntaxhighlightedtextedit.cpp \
    csyntaxhighlighter.cpp \
    csourceassembler.cpp \
    cgraphicsassembler.cpp \
    ccartridgebuilder.cpp
HEADERS += mainwindow.h \
    cnesicideproject.h \
    ixmlserializable.h \
    cprojecttreeviewmodel.h \
    cprojecttreeview.h \
    iprojecttreeviewitem.h \
    cprojectprimitives.h \
    cprojectpalettes.h \
    cprojectpaletteitem.h \
    paletteeditorwindow.h \
    projectpropertiesdialog.h \
    cpaletteitemdelegate.h \
    newprojectdialog.h \
    ccartridge.h \
    cprgrombanks.h \
    cprgrombank.h \
    prgromdisplaydialog.h \
    cchrrombanks.h \
    cchrrombank.h \
    chrromdisplaydialog.h \
    cchrrompreviewrenderer.h \
    qtcolorpicker.h \
    defaultnespalette.h \
    nesemulatordialog.h \
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
    emulator/cnes.h \
    emulator/cnesmappers.h \
    cjoypadlogger.h \
    emulator/cnesio.h \
    ccodedatalogger.h \
    emulator/cnesapu.h \
    emulator/cnes6502.h \
    ctracer.h \
    cnessystempalette.h \
    cnesicidecommon.h \
    nesemulatorrenderer.h \
    nesemulatorthread.h \
    main.h \
    cgltexturemanager.h \
    cproject.h \
    csources.h \
    csourceitem.h \
    codeeditorform.h \
    csyntaxhighlightedtextedit.h \
    csyntaxhighlighter.h \
    csourceassembler.h \
    cgraphicsassembler.h \
    ccartridgebuilder.h
FORMS += mainwindow.ui \
    paletteeditorwindow.ui \
    projectpropertiesdialog.ui \
    newprojectdialog.ui \
    prgromdisplaydialog.ui \
    chrromdisplaydialog.ui \
    nesemulatordialog.ui \
    codeeditorform.ui
RESOURCES += resource.qrc
OTHER_FILES += 
