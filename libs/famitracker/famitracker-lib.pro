#-------------------------------------------------
#
# Project created by QtCreator 2013-02-05T19:20:22
#
#-------------------------------------------------

TARGET = famitracker
TEMPLATE = lib

TOP = ../..

DEFINES += FAMITRACKER_LIBRARY

INCLUDEPATH += \
   $$TOP/common

SOURCES += \
    TrackerChannel.cpp \
    SoundGen.cpp \
    Settings.cpp \
    Sequence.cpp \
    PatternEditor.cpp \
    PatternData.cpp \
    PatternCompiler.cpp \
    PatternAction.cpp \
    MainFrame.cpp \
    InstrumentVRC7.cpp \
    InstrumentVRC6.cpp \
    InstrumentS5B.cpp \
    InstrumentN163.cpp \
    InstrumentFDS.cpp \
    Instrument2A03.cpp \
    Instrument.cpp \
    Graphics.cpp \
    FrameEditor.cpp \
    FrameAction.cpp \
    famitrackermodulepropertiesdialog.cpp \
    famitrackermoduleimportdialog.cpp \
    FamiTrackerDoc.cpp \
    DocumentFile.cpp \
    cmusicfamitrackerinstrumentsmodel.cpp \
    cmusicfamitrackerframesmodel.cpp \
    ChannelsVRC7.cpp \
    ChannelsVRC6.cpp \
    ChannelsS5B.cpp \
    ChannelsN163.cpp \
    ChannelsMMC5.cpp \
    ChannelsFDS.cpp \
    Channels2A03.cpp \
    ChannelMap.cpp \
    ChannelHandler.cpp \
    Action.cpp \
    APU/VRC6.CPP \
    APU/N163.CPP \
    APU/MMC5.CPP \
    APU/FDS.CPP \
    APU/DPCM.CPP \
    APU/APU.CPP \
    APU/VRC7.cpp \
    APU/Triangle.cpp \
    APU/Square.cpp \
    APU/S5B.cpp \
    APU/Noise.cpp \
    APU/Mixer.cpp \
    APU/FDSSound.cpp \
    APU/emu2413.c \
    APU/emu2149.c \
    Blip_Buffer/Blip_Buffer.cpp \
    FFT/Fft.cpp \
    ../../common/cqtmfc.cpp

HEADERS += \
    TrackerChannel.h \
    SoundGen.h \
    Settings.h \
    Sequence.h \
    PatternEditor.h \
    PatternData.h \
    PatternCompiler.h \
    MainFrame.h \
    Instrument.h \
    Graphics.h \
    FrameEditor.h \
    FamiTrackerTypes.h \
    famitrackermodulepropertiesdialog.h \
    famitrackermoduleimportdialog.h \
    FamiTrackerDoc.h \
    Driver.h \
    DocumentFile.h \
    CustomExporterInterfaces.h \
    Compiler.h \
    Common.h \
    ColorScheme.h \
    cmusicfamitrackerinstrumentsmodel.h \
    cmusicfamitrackerframesmodel.h \
    Chunk.h \
    ChannelsVRC7.h \
    ChannelsVRC6.h \
    ChannelsS5B.h \
    ChannelsN163.h \
    ChannelsMMC5.h \
    ChannelsFDS.h \
    Channels2A03.h \
    ChannelMap.h \
    ChannelHandler.h \
    Action.h \
    APU/vrc7tone.h \
    APU/VRC7.h \
    APU/VRC6.H \
    APU/Triangle.h \
    APU/Square.h \
    APU/S5B.h \
    APU/Noise.h \
    APU/N163.H \
    APU/MMC5.H \
    APU/Mixer.h \
    APU/FDSSound.h \
    APU/FDS.H \
    APU/External.h \
    APU/emu2413.h \
    APU/emu2149.h \
    APU/DPCM.h \
    APU/Channel.h \
    APU/APU.h \
    APU/2413tone.h \
    Blip_Buffer/Blip_Buffer.h \
    drivers/drv_vrc7.h \
    drivers/drv_vrc6.h \
    drivers/drv_n163.h \
    drivers/drv_mmc5.h \
    drivers/drv_fds.h \
    drivers/drv_2a03.h \
    drivers/config.h \
    FFT/Fft.h \
    FFT/Complex.h \
    ../../common/cqtmfc.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEABB3021
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = famitracker.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    famitracker-lib.pro

FORMS += \
    PatternEditor.ui \
    MainFrame.ui \
    FrameEditor.ui \
    famitrackermodulepropertiesdialog.ui \
    famitrackermoduleimportdialog.ui

RESOURCES += \
    $$TOP/common/resource.qrc
