#!/usr/bin/env bash

wendy --foo &> result.log
grep -q "aborting \[#07\]" result.log
