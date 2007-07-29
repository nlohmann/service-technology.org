#!/bin/bash
export PATH=~/Downloads/qt-static/bin:$PATH
which qmake
qmake -config release
make
rm -fr release
mkdir release
mv BPEL2oWFN.app release
make distclean
