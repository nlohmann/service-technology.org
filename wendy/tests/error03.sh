#!/usr/bin/env bash

# wendy: the input open net must be normal -- aborting [#03]

wendy error03.owfn --lola= &>result.log
$GREP -q "aborting \[#03\]" result.log
