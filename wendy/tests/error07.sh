#!/usr/bin/env bash

# wendy: unrecognized option `--foo'
# wendy: invalid command-line parameter(s) -- aborting [#07]

wendy --foo &> result.log
$GREP -q "aborting \[#07\]" result.log
