#!/bin/sh

if test -f $1; then $*; else exit 77; fi
