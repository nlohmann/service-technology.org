#!/bin/bash
export PATH=~/Downloads/qt-static/bin:$PATH
which qmake
qmake -config release
make
mv BPEL2oWFN.app release
make distclean
