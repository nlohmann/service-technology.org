#!/bin/bash
export PATH=~/Downloads/qt-static/bin:$PATH
which qmake
qmake -config release
make
cp ../src/bpel2owfn BPEL2oWFN.app/Contents/Resources
mkdir release
mv BPEL2oWFN.app release
make distclean
sudo /Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker -build -proj BPEL2oWFN.pmproj -p BPEL2oWFN.pkg
rm -fr release
