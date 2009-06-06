#!/bin/sh

for tool in autoconf autom4te automake bash bison dot fiona flex gcc gmake gengetopt help2man kc++ libtool lola-full make makeinfo petri sh; do
    echo "$tool (`which $tool || echo "not found"`)"
    echo "------------------------------------------------------------------------"
    echo `($tool --version &> /dev/null && $tool --version) || ($tool -V &> /dev/null && $tool -V) || echo "$tool was not found"` | fmt
    echo
done
