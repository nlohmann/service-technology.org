#!/usr/bin/env bash

if test "$FIONA" = "not found"; then
  # The exit 77 is the magic value which tells Automake's `make check'
  # that this test was skipped.
  exit 77
fi

wendy T2.owfn --og --verbose -m2 &> result.log
fiona -tequivalence -m2 T2.og T2.fiona.og &>> result.log

exit $?
