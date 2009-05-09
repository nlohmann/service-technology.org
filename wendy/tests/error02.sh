#!/usr/bin/env bash

#wendy:13: error near `Place': syntax error, unexpected IDENT,
#          expecting KEY_INTERFACE or KEY_PLACE -- aborting [#02]
          
wendy error02.owfn --lola= &> result.log
$GREP -q "aborting \[#02\]" result.log
