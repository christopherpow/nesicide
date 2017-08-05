TEMPLATE = subdirs

SUBDIRS = nes-emulator-lib nes-emulator-app

nes-emulator-lib.file = ../../libs/nes/nes-emulator-lib.pro
nes-emulator-app.file = ../../apps/nes-emulator/nesicide-emulator.pro

nes-emulator-app.depends = nes-emulator-lib
