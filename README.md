nesicide
========

Integrated Development Environment for the 8-bit Nintendo Entertainment System

Prerequisites
=============
0. Install Qt open source from:

   https://info.qt.io/download-qt-for-application-development

1. Install git

Getting the source
==================
0. Clone the git repository:

   git clone https://github.com/christopherpow/nesicide.git

1. Fetch dependencies package:

   wget https://knob.phreneticappsllc.com/nesicide/nesicide-deps.tar.bz2

Building
========
0. Extract dependencies into top level (nesicide/) folder:

   tar xjvf nesicide-deps.tar.bz2

1. Build nesicide using the build scripts:

   ( cd build && ./build.sh )

2. Create deployments:

   a. For OSX (Mac):
      
      mac-deploy.sh

   b. For Windows:

      win-deploy.sh

Running
=======
The applications can be found at the following locations:

   a. NESICIDE:

      apps/ide/release/

   b. NES Emulator (stand alone package of the emulator used in the IDE):

      apps/nes-emulator/release/

   c. FamiTracker:

      apps/famitracker/release/

   d. FamiPlayer:

      apps/famiplayer/release/

