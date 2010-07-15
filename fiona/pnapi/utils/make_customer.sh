#!/usr/bin/env bash

#############################################################
# This script will generate a new Makefile.am.customer ($1) #
# out of a given Makefile.am ($2).                          #
#############################################################

# used tools
SED="/bin/sed"
GREP="/bin/grep"
HEAD="/usr/bin/head"
TAIL="/usr/bin/tail"
PACKAGE="Petri Net API 4.02-unreleased"


# markers in Makefile.am
SRC_BEGIN=`${GREP} -n "\[CUSTOMER_B_SRC\]" $2 | ${SED} -e "s/:.*//"`
SRC_END=`${GREP} -n "\[CUSTOMER_E_SRC\]" $2 | ${SED} -e "s/:.*//"`
GEN_BEGIN=`${GREP} -n "\[CUSTOMER_B_GEN\]" $2 | ${SED} -e "s/:.*//"`
GEN_END=`${GREP} -n "\[CUSTOMER_E_GEN\]" $2 | ${SED} -e "s/:.*//"`


echo "##################################" > $1
echo "# This file has been generated   #" >> $1
echo "# automaticall by compiling      #" >> $1
echo "# ${PACKAGE} #" >> $1
echo "#                                #" >> $1
echo "# You MUST NOT change this file. #" >> $1
echo "##################################" >> $1
echo "" >> $1

echo "noinst_LIBRARIES = libpnapi.a" >> $1
echo "" >> $1
${HEAD} -n ${SRC_END} $2 | ${TAIL} -n +${SRC_BEGIN} | ${SED} -e "s/_la_/_a_/" | ${SED} -e "s/\([^ ]*\).yy/\1.h \1.cc \1-location.h \1-position.h \1-stack.h/g" | ${SED} -e "s/\([^ ]*\).ll/\1.cc/g" >> $1
echo "" >> $1
echo "# to find customer's config.h header" >> $1
echo "libpnapi_a_CPPFLAGS = -I\$(top_srcdir)/src" >> $1
echo "" >> $1

result=0
