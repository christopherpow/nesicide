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
    qtcolorpicker.cpp
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
    qtcolorpicker.h
FORMS += mainwindow.ui \
    paletteeditorwindow.ui \
    projectpropertiesdialog.ui \
    newprojectdialog.ui \
    prgromdisplaydialog.ui \
    chrromdisplaydialog.ui
RESOURCES += resource.qrc
