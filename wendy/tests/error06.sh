#!/usr/bin/env bash

# wendy: error while parsing the reachability -- aborting [#06]

wendy error06.owfn --lola= &>result.log
$GREP -q "aborting \[#06\]" result.log
