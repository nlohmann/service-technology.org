#!/bin/sh

mv InterfaceMarking.cc backup.cc
mv InterfaceMarking.h backup.h
mv FIFO.cc InterfaceMarking.cc
mv FIFO.h InterfaceMarking.h
mv backup.cc FIFO.cc
mv backup.h FIFO.h

test ! -f wendy-InterfaceMarking.o || rm wendy-InterfaceMarking.o

if `grep -q "queue" InterfaceMarking.h`; then
	echo "switched to FIFO"
else
	echo "switched to InterfaceMarking"
fi
