#!/usr/bin/env bash

# wendy: at most one input file must be given -- aborting [#04]

wendy error04*.owfn &> result.log
$GREP -q "aborting \[#04\]" result.log
