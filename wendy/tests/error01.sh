#!/usr/bin/env bash

wendy foo.owfn &> result.log
grep -q "aborting \[#01\]" result.log
