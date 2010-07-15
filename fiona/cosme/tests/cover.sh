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
case $COVER in
  "ZCOV")
    # ZCOV should always be quiet
    ZCOV="/home/jeed/workspace/cosme/tests/zcov/zcov-scan"

    # derive test name from current directory (assuming GNU Autotest); the
    # info filename is suffixed with a random number to allow for multiple
    # invokations of this script for the same test
    dir=`pwd`
    TESTNAME=`basename $dir`

    # execute executable and remember exit code
    $*
    result=$?

    # scan directory
    $ZCOV /home/jeed/workspace/cosme/tests/cover/$TESTNAME.zcov /home/jeed/workspace/cosme &> /dev/null

    # exit with executable's exit code
    exit $result  
  ;;

  "LCOV")
    # LCOV should always be quiet
    LCOV="/home/jeed/workspace/cosme/tests/lcov/lcov --quiet"

    # derive test name from current directory (assuming GNU Autotest); the
    # info filename is suffixed with a random number to allow for multiple
    # invokations of this script for the same test
    dir=`pwd`
    TESTNAME=`basename $dir`
    INFOFILE=/home/jeed/workspace/cosme/tests/cover/$TESTNAME-$RANDOM.info

    # reset counters for LCOV
    $LCOV --zerocounters -directory /home/jeed/workspace/cosme

    # execute executable and remember exit code
    $*
    result=$?

    # evaluate coverage result of LCOV
    $LCOV --capture --directory /home/jeed/workspace/cosme --output-file $INFOFILE --test-name $TESTNAME

    # remove profile information on C++ sources and headers
    $LCOV --extract $INFOFILE "/home/jeed/workspace/cosme/*" -o $INFOFILE

    # exit with executable's exit code
    exit $result  
  ;;

  *)
    $*
    exit $?
  ;;
esac

