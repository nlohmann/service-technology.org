#!/usr/bin/env bash

# wendy: could not open file `foo.owfn' -- aborting [#01]

wendy foo.owfn --lola= &>result.log
$GREP -q "aborting \[#01\]" result.log
