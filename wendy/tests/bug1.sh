#!/usr/bin/env bash

if test "$FIONA" = "not found"; then
  # The exit 77 is the magic value which tells Automake's `make check'
  # that this test was skipped.
  exit 77
fi

wendy bug1.owfn --og --verbose &> result.log
fiona -tequivalence bug1.og bug1.fiona.og &>> result.log
