# Signature of the current package.
m4_define([AT_PACKAGE_NAME],      [Petri Net API])
m4_define([AT_PACKAGE_TARNAME],   [pnapi])
m4_define([AT_PACKAGE_VERSION],   [4.02-unreleased])
m4_define([AT_PACKAGE_STRING],    [Petri Net API 4.02-unreleased])
m4_define([AT_PACKAGE_BUGREPORT], [pnapi@service-technology.org])

# testfile directory
m4_define([TESTFILES],            [/home/jeed/workspace/pnapi/tests/testfiles])

# customer tool test script
m4_define([AT_CHECK_CUSTOMER],    [/home/jeed/workspace/pnapi/tests/check_customer.sh])

# wrap tools called by the testscript
m4_define([GREP],                 [/bin/grep])
m4_define([FIONA],                [wrap-tool.sh /usr/local/bin/fiona])
m4_define([VALGRIND],             [wrap-tool.sh /usr/bin/valgrind --leak-check=full --show-reachable=yes])

# pass tool paths to tests below
m4_define([DOT],                  [/usr/bin/dot])
m4_define([GENET],                [not found])
m4_define([LOLA],                 [/usr/local/bin/lola-statespace])
m4_define([PETRIFY],              [/usr/local/bin/petrify])
m4_define([WGET],                 [/usr/bin/wget])

# check if several tools were found by the configure script
m4_define(AT_CHECK_DOT, [AT_CHECK([if test "DOT" == "not found"; then exit 77; fi])])
m4_define(AT_CHECK_GENET, [AT_CHECK([if test "GENET" == "not found"; then exit 77; fi])])
m4_define(AT_CHECK_LOLA, [AT_CHECK([if test "LOLA" == ""; then exit 77; fi])])
m4_define(AT_CHECK_PETRIFY, [AT_CHECK([if test "PETRIFY" == "not found"; then exit 77; fi])])
m4_define(AT_CHECK_WGET, [AT_CHECK([if test "WGET" == "not found"; then exit 77; fi])])

# tools to be tested
m4_define([CDNF],                 [AT_CHECK_LOLA
                                   cover.sh /home/jeed/workspace/pnapi/utils/cdnf])
m4_define([CONFIGURATOR],         [cover.sh /home/jeed/workspace/pnapi/utils/configurator])
m4_define([GENERATOR],            [cover.sh /home/jeed/workspace/pnapi/utils/generator])
m4_define([PETRI],                [cover.sh /home/jeed/workspace/pnapi/utils/petri])
m4_define([UNFOLDER],             [cover.sh /home/jeed/workspace/pnapi/utils/unfolder/unfolder])

# makro used in testsuite
m4_define([TEST_CDNF], [AT_CHECK([cp TESTFILES/NET.owfn .])
                        AT_CHECK([CDNF NET.owfn])
                        AT_CHECK([FIONA -t equivalence NET.owfn NET.cdnf.owfn], 0, ignore, ignore)
                        AT_KEYWORDS(cdnf)])

m4_define([TEST_LEAK], [AT_CHECK([cp TESTFILES/NET .])
                        AT_CHECK([VALGRIND /home/jeed/workspace/pnapi/utils/petri PARAMS NET], 0, ignore, stderr)
                        AT_CHECK([GREP -q "All heap blocks were freed -- no leaks are possible" stderr])
                        AT_KEYWORDS(leak)])

