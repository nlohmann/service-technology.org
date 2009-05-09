#!/usr/bin/env bash

# wendy: message bound must be between 1 and 255 -- aborting [#09]

wendy --messagebound=400 --lola=foo &>result.log
$GREP -q "aborting \[#09\]" result.log
