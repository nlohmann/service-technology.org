#!/usr/bin/env bash
# tests/cover.sh.  Generated from cover.sh.in by configure.

#############################################################################
# This script has two modes:
#
#  - if the environment variable COVER is set to TRUE, LCOV from the Linux
#    Test project (http://ltp.sourceforge.net/coverage/lcov.php) is used to
#    collect test case coverage results for the given executable
#
#  - otherwise, the given executable is called without LCOV
#############################################################################

# evaluate COVER variable
if test "$COVER" != "TRUE"; then
	$*
	exit $?
fi

#############################################################################

# LCOV should always be quiet
LCOV="/home/jeed/workspace/pnapi/tests/lcov/lcov --quiet"

# derive test name from current directory (assuming GNU Autotest); the
# info filename is suffixed with a random number to allow for multiple
# invokations of this script for the same test
dir=`pwd`
TESTNAME=`basename $dir`
INFOFILE=/home/jeed/workspace/pnapi/tests/cover/$TESTNAME-$RANDOM.info

# reset counters for LCOV
$LCOV --zerocounters -directory /home/jeed/workspace/pnapi

# execute executable and remember exit code
$*
result=$?

# evaluate coverage result of LCOV
$LCOV --capture --directory /home/jeed/workspace/pnapi --output-file $INFOFILE --test-name $TESTNAME

# remove profile information on C++ sources and headers
$LCOV --extract $INFOFILE "/home/jeed/workspace/pnapi/*" -o $INFOFILE

# exit with executable's exit code
exit $result
