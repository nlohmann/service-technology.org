#!/usr/bin/env bash

if test "$FIONA" = "not found"; then
  # The exit 77 is the magic value which tells Automake's `make check'
  # that this test was skipped.
  exit 77
fi

wendy zeus.owfn --og --verbose &> result.log
fiona -tequivalence zeus.og zeus.fiona.og &>> result.log

exit $?
