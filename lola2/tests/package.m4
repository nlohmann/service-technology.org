# tests/package.m4.  Generated from package.m4.in by configure.

#============================================================================#
# AUTOTEST PACKAGE CONFIGURATION FILE                                        #
# skeleton written by Niels Lohmann <niels.lohmann@uni-rostock.de>           #
#============================================================================#

# This file configures the Autotest test suite (usually file testsuite.at). The
# configuration is read when generating the test scripts, and it is also used
# in the test cases. The variables set here are accessible in the test cases
# and usually hold the name of directories and tools.
#
# A configuration file like this usually consists of three parts:
#
# 1. A generic part that contains information that is not specific for this
#    package. These information include the package's name and version, the
#    current SVN revision, the absolute directory of the test files and the
#    directory where benchmark results are written to, and the full name of
#    the executables of the standard tools grep, awk, and sed. You must not
#    change this part.
#
# 2. The main executable. This is the actual program you built in this
#    package and that you now want to check. Usually, two variables are
#    defined: The first variable "TOOL" calls the main executable through a
#    wrapper script "cover.sh". This is required when code coverage should be
#    checked. A second variable "TOOL_RAW" does not use this wrapper script.
#    This is used if the executable is analyzed, for instance using Valgrind.
#    All you need to change in this part is the name of the main tool(s) of
#    your package.
#
# 3. Several helper tools that are used in the test script. Here, we
#    distinguish two type of tools: optional tools and mandatory tools. An
#    optional tool is defined by prefixing it with a script "wrap.sh". If the
#    tool was not found, an exit code is return to the calling test script that
#    yields skipping the current test case. Mandatory tools are called without
#    this script and yield failures if the tool is not found. This part is
#    optional. Add as many tools as you need. Please make sure you define the
#    variables surrounded by @'s in the file "configure.ac". This is usually
#    done by calling "AC_PATH_PROG(MIA, [mia], not found)".


#-------------------------------------#
# Part 1: Generic Package Information #
#-------------------------------------#

# Signature of the current package.
m4_define([AT_PACKAGE_NAME],      [LoLA])
m4_define([AT_PACKAGE_TARNAME],   [lola])
m4_define([AT_PACKAGE_VERSION],   [2.0-unreleased])
m4_define([AT_PACKAGE_STRING],    [LoLA 2.0-unreleased])
m4_define([AT_PACKAGE_BUGREPORT], [lola@service-technology.org])

# SVN Revision-URL
m4_define([SVNURL],               [http://svn.gna.org/viewcvs/service-tech?rev=8169M&view=rev])

# directories
m4_define([TESTFILES],            [/home/erik/lola2/tests/testfiles])
m4_define([BENCHRESULTS],         [/home/erik/lola2/tests/benchmarks-results])

# standard UNIX helper tools
m4_define([GREP],                 [/bin/grep])
m4_define([AWK],                  [gawk])
m4_define([SED],                  [/bin/sed])


#------------------------------------------#
# Part 2: The Package's Main Executable(s) #
#------------------------------------------#

# the main executable, called through a shell script for code coverage
m4_define([LOLA],       [cover.sh /home/erik/lola2/src/lola --nolog])
m4_define([LISTENER],   [cover.sh /home/erik/lola2/src/listener])
m4_define([KILLER],     [cover.sh /home/erik/lola2/src/killer])

# the main executable, directly called to allow for analysis
m4_define([LOLA_RAW],  [/home/erik/lola2/src/lola --nolog])


#---------------------------------------------#
# Part 3: Optional and Mandatory Helper Tools #
#---------------------------------------------#

m4_define([VALGRIND],             [wrap.sh /usr/bin/valgrind])

#---------------------------------#
# Part 4: Macros used by Autotest #
#---------------------------------#

# AT_ST_CHECK_MEMORY(label, file, check, store, encoder, threads)
m4_define([AT_ST_CHECK_MEMORY], [
  AT_SETUP([Checking for memory leaks: $1 (check: $3, store: $4, encoder: $5, threads: $6)])
  AT_CHECK([cp TESTFILES/$2 .])
  AT_CHECK([VALGRIND --leak-check=full --show-reachable=yes LOLA_RAW --check=$3 --store=$4 --encoder=$5 --threads=$6 $2],0,ignore,stderr)
  AT_CHECK([GREP -q -E "definitely lost: 0 bytes in 0 blocks|All heap blocks were freed -- no leaks are possible" stderr])
  AT_KEYWORDS(valgrind)
  AT_CLEANUP
])

# AT_LTL_CHECK_MEMORY(label, file, buechi, store)
m4_define([AT_LTL_CHECK_MEMORY], [
  AT_SETUP([Checking for memory leaks: $1 (check: $3, store: $4)])
  AT_CHECK([cp TESTFILES/$2 .])
  AT_CHECK([cp TESTFILES/$3 .])
  AT_CHECK([VALGRIND --leak-check=full --show-reachable=yes LOLA_RAW --check=ltl --store=$4 --buechi=$3 $2],0,ignore,stderr)
  AT_CHECK([GREP -q -E "definitely lost: 0 bytes in 0 blocks|All heap blocks were freed -- no leaks are possible" stderr])
  AT_KEYWORDS(valgrind)
  AT_CLEANUP
])

# AT_CHECK_RUNLOLA(label, file, check, store, encoder, hashing, threads, hashfunctions, result, markings, edges)
m4_define([AT_CHECK_RUNLOLA], [
  AT_SETUP([$1 (store $4, encoder $5, $7 threads)])
  AT_CHECK([cp TESTFILES/$2 .])
  AT_CHECK([LOLA --check=$3 --store=$4 --encoder=$5 $6 --threads=$7 --hashfunctions=$8 $2],0,ignore,stderr)
  AT_CHECK([GREP -q "lola: result: $9" stderr])
  AT_CHECK([GREP -q "$10 markings" stderr])
  AT_CHECK([GREP -q "$11 edges" stderr])
  AT_KEYWORDS(stores)
  AT_CLEANUP
])
