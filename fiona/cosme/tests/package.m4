# tests/package.m4.  Generated from package.m4.in by configure.
# Signature of the current package.
m4_define([AT_PACKAGE_NAME],      [Cosme])
m4_define([AT_PACKAGE_TARNAME],   [cosme])
m4_define([AT_PACKAGE_VERSION],   [0.7])
m4_define([AT_PACKAGE_STRING],    [Cosme 0.7])
m4_define([AT_PACKAGE_BUGREPORT], [cosme@service-technology.org])

# the main executable, called through a shell script for code coverage
m4_define([COSME],                [cover.sh /home/jeed/workspace/cosme/src/cosme])
m4_define([COSME_RAW],            [/home/jeed/workspace/cosme/src/cosme])

# directories
m4_define([TESTFILES],            [/home/jeed/workspace/cosme/tests/testfiles])

# UNIX helper tools
m4_define([GREP],                 [/bin/grep])
m4_define([AWK],                  [gawk])
m4_define([SED],                  [/bin/sed])

# tools needed by the testscript
m4_define([LOLA],                 [/usr/local/bin/lola-statespace])
m4_define([VALGRIND],             [wrap.sh /usr/bin/valgrind])

