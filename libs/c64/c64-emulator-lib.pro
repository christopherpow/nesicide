#-------------------------------------------------
#
# Project created by QtCreator 2010-08-02T22:13:12
#
#-------------------------------------------------

TARGET = c64-emulator
TEMPLATE = lib

# Remove Qt libraries
QT =

TOP = ../..

# Build static library
#CONFIG += staticlib

# Remove crap we don't need!
CONFIG -= rtti exceptions

unix:!mac {
   PREFIX = $$(PREFIX)
   isEmpty (PREFIX) {
      PREFIX = /usr/local
   }

   BINDIR = $$(BINDIR)
   isEmpty (BINDIR) {
      BINDIR=$$PREFIX/lib
   }

   target.path = $$BINDIR
   INSTALLS += target
}

INCLUDEPATH += . \
               ./common \
               ./emulator \
               $$TOP/apps/ide/emulator

SOURCES += \
   $$TOP/common/cbreakpointinfo.cpp \
   c64_emulator_core.cpp \
   emulator/cc646502.cpp \
   emulator/cc64breakpointinfo.cpp \
   common/cc64systempalette.cpp \
   emulator/cc64sid.cpp

HEADERS +=\
   c64_emulator_core.h \
   emulator/cc646502.h \
   emulator/cc64breakpointinfo.h \
   common/cc64systempalette.h \
   emulator/cc64sid.h
