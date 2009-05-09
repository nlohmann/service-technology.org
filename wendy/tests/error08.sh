#!/usr/bin/env bash

# wendy: report frequency must not be negative -- aborting [#08]

wendy --reportFrequency=-10 &>result.log
$GREP -q "aborting \[#08\]" result.log
