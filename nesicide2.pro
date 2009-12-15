# -------------------------------------------------
# Project created by QtCreator 2009-12-07T20:35:20
# -------------------------------------------------
QT += network \
    opengl \
    webkit \
    xml \
    multimedia
TARGET = nesicide2
TEMPLATE = app
LIBS += ./libfmodex.a
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
    Tracer.cpp \
    ROMMapper068.cpp \
    ROMMapper065.cpp \
    ROMMapper011.cpp \
    ROMMapper010.cpp \
    ROMMapper009.cpp \
    ROMMapper007.cpp \
    ROMMapper005.cpp \
    ROMMapper004.cpp \
    ROMMapper003.cpp \
    ROMMapper002.cpp \
    ROMMapper001.cpp \
    ROM.cpp \
    PPU.cpp \
    NES.cpp \
    Mappers.cpp \
    JoypadLogger.cpp \
    IO.cpp \
    CodeDataLogger.cpp \
    APU.cpp \
    6502.cpp \
    BasePalette.cpp \
    nesemulatorrenderer.cpp \
    nesemulatorthread.cpp \
    cgltexturemanager.cpp \
    cproject.cpp
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
    ROMMapper068.h \
    ROMMapper065.h \
    ROMMapper011.h \
    ROMMapper010.h \
    ROMMapper009.h \
    ROMMapper007.h \
    ROMMapper005.h \
    ROMMapper004.h \
    ROMMapper003.h \
    ROMMapper002.h \
    ROMMapper001.h \
    ROM.h \
    PPU.h \
    NES.h \
    Mappers.h \
    JoypadLogger.h \
    IO.h \
    CodeDataLogger.h \
    APU.h \
    6502.h \
    Tracer.h \
    BasePalette.h \
    NESICIDECommon.h \
    nesemulatorrenderer.h \
    nesemulatorthread.h \
    main.h \
    cgltexturemanager.h \
    cproject.h \
    fmod_output.h \
    fmod_memoryinfo.h \
    fmod_errors.h \
    fmod_dsp.h \
    fmod_codec.h \
    fmod.hpp \
    fmod.h
FORMS += mainwindow.ui \
    paletteeditorwindow.ui \
    projectpropertiesdialog.ui \
    newprojectdialog.ui \
    prgromdisplaydialog.ui \
    chrromdisplaydialog.ui \
    nesemulatordialog.ui
RESOURCES += resource.qrc
OTHER_FILES += 
