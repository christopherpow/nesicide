#!/bin/sh
cd $(git rev-parse --show-toplevel)

python tools/rc2qt/rc2qt.py \
       libs/famitracker/FamiTracker.rc \
       > libs/famitracker/cqtmfc_famitracker.cpp
