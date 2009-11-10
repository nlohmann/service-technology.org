#!/usr/bin/env bash

NET=$1
WENDY=wendy
SA2SM=sa2sm
SAYO=sayo

${WENDY} --og=${NET}.og --formula=bits ${NET} &> /dev/null

${SAYO} -i ${NET}.og -o ${NET}.sa

${SA2SM} ${NET}.sa ${NET}.pv.owfn

