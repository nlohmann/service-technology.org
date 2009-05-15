#!/usr/bin/env bash

if test "$FIONA" = "not found"; then
  # The exit 77 is the magic value which tells Automake's `make check'
  # that this test was skipped.
  exit 77
fi

wendy smtp3.owfn --fionaFormat --og --verbose &>result.log
fiona -tequivalence smtp3.og smtp3.fiona.og >>result.log 2>&1
