#!/usr/bin/env bash

wendy error02.owfn &> result.log
grep -q "aborting \[#02\]" result.log
