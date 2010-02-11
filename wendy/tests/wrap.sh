#!/bin/sh

#============================================================================#
# AUTOTEST WRAPPER SCRIPT FOR EXTERNAL TOOLS                                 #
# written by Niels Lohmann <niels.lohmann@uni-rostock.de>                    #
#============================================================================#

# This script is used to call executables in optional tests of the testsuite.
# In case the executable is not found, the test is skipped. This is realized
# by exiting with code 77 which is interpreted by Autotest, see
# http://www.gnu.org/software/autoconf/manual/autoconf.html#Writing-Testsuites.
# If the executable is found, it is executed with all given parameters and its
# exit code is passed to the caller of this script.


if test -f $1
  then eval $*
  else exit 77
fi
