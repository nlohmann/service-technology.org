#!/usr/bin/env bash

wendy error04*.owfn &> result.log
grep -q "aborting \[#04\]" result.log
