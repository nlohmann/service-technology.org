#!/usr/bin/env bash
# this script will get a customer tool of the PNAPI,
# replace its version of the API with the present one
# and executes make check

TOOL="$5"
LIB="${TOOL}/libs/pnapi"
SVN="$1"
SRC="$2"
REPOSITORY="$3"
MAKE="$4"


# get customer tool
${SVN} co ${REPOSITORY}/${TOOL} ${TOOL}

# clear customer's library directory
rm -rf ${LIB}/*

# copy files
cp ${SRC}/*.cc ${SRC}/*.h ${SRC}/*.yy ${SRC}/*.ll ${LIB}
cp ${SRC}/Makefile.am.customer ${LIB}/Makefile.am

# remove unnecessary files again
rm -rf ${LIB}/config.h
rm -rf ${LIB}/parser-*.cc
rm -rf ${LIB}/parser-*.h

# change to the temp dir and build tool
cd ${TOOL}
autoreconf -i
./configure
${MAKE}
${MAKE} check
result=$?
cd ..

echo ""

exit $result
