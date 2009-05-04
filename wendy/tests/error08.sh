#!/usr/bin/env bash

wendy --reportFrequency=-10 &> result.log
grep -q "aborting \[#08\]" result.log
