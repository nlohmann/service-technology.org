#!/usr/bin/env bash

wendy error06.owfn --lola= &> result.log
grep -q "aborting \[#06\]" result.log
