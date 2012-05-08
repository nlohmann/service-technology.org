#!/bin/sh
#set -x

JSON=$1

TOOLNAME=`cat $JSON | jsawk 'return this.shortname'`


################
# get url/mail #
################

PACKAGE_BUGREPORT=`echo @PACKAGE_BUGREPORT@ | ../config.status --file=-`
PACKAGE_URL=`echo @PACKAGE_URL@ | ../config.status --file=-`


################
# get versions #
################

OFFICIALVERSION=$(curl -s http://download.gna.org/service-tech/$TOOLNAME/ | grep -oh "$TOOLNAME-[0-9.]*" | sort | uniq | sed "s/$TOOLNAME-//g" | sed 's/.$//g' | tail -n1)
LASTVERSION=`echo @PACKAGE_VERSION@ | ../config.status --file=-`


#################
# get comitters #
#################

COMMITTERS=$(svn log .. --quiet | grep '^r' | awk '{print $3}' | sort | uniq -c | sort -r | awk ' { print "{ \"user\": \"" $2 "\", \"commits\": " $1 " }"  } ' | tr '\n' ',' | sed 's/.$//')


#####################
# get prerequisites #
#####################

DIR=".."
REQ=""
REQ="${REQ}"$'\n'"`grep -Rh "AM_MISSING_PROG(" ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PATH_PROG("    ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_CC"       ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_CXX"      ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_LEX"      ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_YACC"     ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_LIBTOOL"  ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"

REQ_COMPILE=""
REQ_RUNTIME=""
REQ_TESTS=""
REQ_DOC=""
REQ_OTHER=""
REQ_MANIPULATION=""


# Compilation
if `echo "$REQ" | grep -q "AC_PROG_CC"`;      then REQ_COMPILE="${REQ_COMPILE}\"gcc\", "; fi
if `echo "$REQ" | grep -q "AC_PROG_CXX"`;     then REQ_COMPILE="${REQ_COMPILE}\"g++\", "; fi
if `echo "$REQ" | grep -q "AC_PROG_LIBTOOL"`; then REQ_COMPILE="${REQ_COMPILE}\"libtool\", "; fi

REQ_COMPILE=`echo "$REQ_COMPILE" | sed 's/.$//'`

# Tests
if `echo "$REQ" | grep -q "autom4te"`; then REQ_TESTS="${REQ_TESTS}\"autotest\", "; fi
if `echo "$REQ" | grep -q "lcov"`;     then REQ_TESTS="${REQ_TESTS}\"lcov\", "; fi
if `echo "$REQ" | grep -q "valgrind"`; then REQ_TESTS="${REQ_TESTS}\"valgrind\", "; fi

REQ_TESTS=`echo "$REQ_TESTS" | sed 's/.$//'`


# Code Manipulation
if `echo "$REQ" | grep -q AC_PROG_YACC`; then REQ_MANIPULATION="${REQ_MANIPULATION}\"bison\", "; fi
if `echo "$REQ" | grep -q AC_PROG_LEX`;  then REQ_MANIPULATION="${REQ_MANIPULATION}\"flex\", "; fi
if `echo "$REQ" | grep -q gengetopt`;    then REQ_MANIPULATION="${REQ_MANIPULATION}\"gengetopt\", "; fi
if `echo "$REQ" | grep -q kc++`;         then REQ_MANIPULATION="${REQ_MANIPULATION}\"kc++\", "; fi

REQ_MANIPULATION=`echo "$REQ_MANIPULATION" | sed 's/.$//'`


##############
# write file #
##############

touch generic.json
cat generic.json | jsawk "this.officialVersion = \"$OFFICIALVERSION\"; this.lastVersion = \"$LASTVERSION\"; this.url = \"$PACKAGE_URL\"; this.email = \"$PACKAGE_BUGREPORT\"; this.commits = [$COMMITTERS]; this.req_compile = [$REQ_COMPILE]; this.req_tests = [$REQ_TESTS]; this.req_edit = [$REQ_MANIPULATION]" > generic.json~
mv generic.json~ generic.json
