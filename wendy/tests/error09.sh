#!/usr/bin/env bash

wendy --messagebound=400 &> result.log
grep -q "aborting \[#09\]" result.log
