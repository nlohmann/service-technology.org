#!/usr/bin/env bash

wendy error03.owfn &> result.log
grep -q "aborting \[#03\]" result.log
