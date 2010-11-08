#!/bin/sh

if test -f $1; then eval $*; else exit 77; fi
