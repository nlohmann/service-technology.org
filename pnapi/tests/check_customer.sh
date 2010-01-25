#!/usr/bin/env bash
# this script will get a customer tool of the PNAPI,
# replace its version of the API with the present one
# and executes make check

TOOL="$5"
LIB="${TOOL}/libs/pnapi"

WGET="$1"
TAR="$2"
SRC="$3"
MAKE="$4"

TARBALL="http://esla.informatik.uni-rostock.de:8080/job/${TOOL}/lastSuccessfulBuild/artifact/${TOOL}/${TOOL}.tar.gz"

# get customer tool
${WGET} ${TARBALL}
${TAR} xzf ${TOOL}.tar.gz

# clear customer's library directory
rm -rf ${LIB}/*

# copy files
cp ${SRC}/*.cc ${SRC}/*.h ${SRC}/*.yy ${SRC}/*.ll ${LIB}
cp ${SRC}/Makefile.am.customer ${LIB}/Makefile.am

# remove unnecessary files again
rm -rf ${LIB}/config.h

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
