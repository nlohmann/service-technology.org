#!/bin/bash

FILE=`cat $1`

rdom () {
	local IFS=\> ; read -d \< E C ;
}

trim() {
	echo "$1" | tr '\n' '§' | sed 's/^[§]*//;s/[§]*$//' | tr '§' '\n'
}

extract() {
	while rdom; do
	    if [[ $E = $1 ]]; then
	        trim "$C"
	        exit
	    fi
	done
}

get() {
	echo "$FILE" | extract $1
}

center() {
	echo "$1" | sed -e :a -e 's/^.\{1,77\}$/ &/;ta' -e 's/\( *\)\1/\1/'
}

noindent() {
	echo "$1" | fmt -w 78
}

indent() {
	echo "$1" | fmt -w 74 | sed 's/^/  /g' | sed 's/^  \*/*/g'
}

##############################################################################

# The name of the tool - used for the ASCII art
TOOLNAME=`get "toolname"`
TOOLNAME_FIGLET_CENTER=`figlet -c -w79 "$TOOLNAME"`

# The short name of the the tool - used for the URL.
# If no <shortname> tag is provided, use the toolname with lower characters.
SHORTNAME=`get "shortname"`
if [ "$SHORTNAME" == "" ]; then
	SHORTNAME=`echo "$TOOLNAME" | tr '[:upper:]' '[:lower:]'`
fi

# The tool URL and e-mail address.
URL="http://service-technology.org/$SHORTNAME"
URL_CENTER=`center $URL`
EMAIL="$SHORTNAME@service-technology.org"
EMAIL_CENTER=`center $EMAIL`

# The tagline.
TAGLINE=`get "tagline"`
TAGLINE_CENTER=`center "$TAGLINE"`

# The purpose.
PURPOSE=`get "purpose"`
PURPOSE_FORMATTED=`noindent "$PURPOSE"`

# The copyright.
COPYRIGHT=`get "copyright"`
COPYRIGHT_FORMATTED=`noindent "Copyright (c) $COPYRIGHT"`

# The license
LICENSE=`get "license"`
LICENSENAME="$LICENSE"
LICENSEURL=""
LICENSEVERSION=""
case $LICENSE in
	"AGPL3+")
		LICENSENAME="GNU Affero General Public License"
		LICENSEURL="http://www.gnu.org/licenses/agpl.html"
		LICENSEVERSION="version 3 or (at your option) any later version"
		;;
	"LGPL3")
		LICENSENAME="GNU Lesser General Public License"
		LICENSEURL="http://www.gnu.org/licenses/lgpl.html"
		LICENSEVERSION="version 3"
		;;
esac
LICENSETEXT="$TOOLNAME is free software: you can redistribute it and/or modify it under the terms of the $LICENSENAME $LICENSEVERSION as published by the Free Software Foundation, see <$LICENSEURL> or the file 'COPYING'.

$TOOLNAME is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the $LICENSENAME for more details.

Different licensing schemes may be negotiated with the author(s)."
LICENSETEXT_FORMATTED=`noindent "$LICENSETEXT"`

RUNTIME=`get "runtime"`
RUNTIMETEXT=""
echo "$RUNTIME" | while read line
do
	case "$line" in
		"lola-statespace")
			RUNTIMETEXT="foo"
			;;
	esac
done

# The authors.
AUTHORS=`get "authors"`
AUTHORLINES=`echo "${AUTHORS}" | wc -l | sed 's/ //g'`
#echo "$F"
if [ "${AUTHORLINES}" -eq 1 ]; then
	AUTHORS="${TOOLNAME} was written by ${AUTHORS}."
else
	AUTHORS=`indent "${AUTHORS}"`
fi

# The maintainer
MAINTAINER=`get "maintainer"`
MAINTAINERTEXT="${TOOLNAME} is maintained by ${MAINTAINER}. See file 'AUTHORS' for more information."


THANKS=`get "thanks"`


##############################################################################

# README
echo "$TOOLNAME_FIGLET_CENTER"
echo "$TAGLINE_CENTER"
echo "$URL_CENTER"
echo ""
echo ""
echo "Purpose"
echo "======="
echo ""
echo "$PURPOSE_FORMATTED"
echo ""
echo ""
echo "Copyright"
echo "========="
echo ""
echo "$COPYRIGHT_FORMATTED"
echo ""
echo ""
echo "Authors"
echo "======="
echo ""
echo "${AUTHORS}"
echo ""
echo "See file 'AUTHORS' for details."
echo ""
echo ""
echo "Maintainer"
echo "=========="
echo ""
noindent "${MAINTAINERTEXT}"
echo ""
echo ""
echo "License"
echo "======="
echo ""
echo "$LICENSETEXT_FORMATTED"
echo ""
echo ""
echo "Installation"
echo "============"
echo ""
noindent "$TOOLNAME can be easily compiled and installed. See file 'INSTALL' for instructions. Please also have a look at the file 'REQUIREMENTS' for information on the tools required to run, compile, and develop $TOOLNAME."
echo ""
echo ""

echo "Requirements"
echo "============"
echo ""
noindent "In order to run, generate the documentation, compile, or develop ${TOOLNAME}, several tools are required."
echo ""

# REQUIREMENTS

DIR=".."
REQ=""
REQ="${REQ}"$'\n'"`grep -Rh "AM_MISSING_PROG(" ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PATH_PROG("    ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_CC"       ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_CXX"      ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_LEX"      ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_YACC"     ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"
REQ="${REQ}"$'\n'"`grep -Rh "AC_PROG_LIBTOOL"  ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq`"

#echo "${REQ}"


REQ_COMPILE=""
REQ_RUNTIME=""
REQ_TESTS=""
REQ_DOC=""
REQ_OTHER=""
REQ_MANIPULATION=""

# Compilation
if `echo "$REQ" | grep -q "AC_PROG_CC"`; then REQ_COMPILE="${REQ_COMPILE}* A modern C compiler is required to compile ${TOOLNAME}. We develop using the GNU Compiler Collection (GCC), which should be also the standard compiler on your machine. Recent experiments with LLVM Clang (available at <http://clang.llvm.org>) were also promising."$'\n\n'; fi
if `echo "$REQ" | grep -q "AC_PROG_CXX"`; then REQ_COMPILE="${REQ_COMPILE}* A modern C++ compiler is required to compile ${TOOLNAME}. We develop using the GNU Compiler Collection (GCC), which should be also the standard compiler on your machine. Recent experiments with LLVM Clang (available at <http://clang.llvm.org>) were also promising."$'\n\n'; fi
REQ_COMPILE="${REQ_COMPILE}* GNU Make is required to process the Makefiles. Other versions of make may not work. GNU Make is available at <http://www.gnu.org/s/make>."$'\n\n'
if `echo "$REQ" | grep -q "AC_PROG_LIBTOOL"`; then REQ_COMPILE="${REQ_COMPILE}* Libtool is used to manage static or dynamic libraries. Libtool is available at <http://www.gnu.org/s/libtool>."$'\n\n'; fi

# Runtime
if `echo "$REQ" | grep -q "lola-statespace"`; then REQ_RUNTIME="${REQ_RUNTIME}* LoLA (lola-statespace) is used by ${TOOLNAME} to generate Petri net state spaces. Without LoLA, ${TOOLNAME} will not work. You can download LoLA at <http://service-technology.org/lola>. After downloading the tarball (<http://service-technology.org/files/lola/lola.tar.gz>), unpack it and execute './configure && make lola-statespace'."$'\n\n'; fi

# Documentation
if `grep -qR "groff" ${DIR}`; then REQ_DOC="${REQ_DOC}* Groff is used to generate a PDF version of the UNIX Manpage fro ${TOOLNAME}. Without Groff, 'make man-pdf' will not work. Groff can be downloaded at <http://www.gnu.org/s/groff>."$'\n\n'; fi
if `echo "$REQ" | grep -q help2man`; then REQ_DOC="${REQ_DOC}* help2man is used to generate a UNIX Manpage for ${TOOLNAME}. help2man can be downloaded at <http://www.gnu.org/s/help2man>."$'\n\n'; fi
if `grep -qR "_TEXINFOS" ${DIR}`; then REQ_DOC="${REQ_DOC}* Texinfo is used to geneate a documentation for ${TOOLNAME}. To generate a PDF version of the documentation (in the directory 'doc'), execute 'make pdf'. Texinfo can be downloaded at <http://www.gnu.org/s/texinfo>."$'\n\n'; fi

# Tests
if `echo "$REQ" | grep -q "autom4te"`; then REQ_TESTS="${REQ_TESTS}* Autoconf is used to generate an Autotest testsuite for ${TOOLNAME}. Without Autoconf, 'make check' will not work. Autoconf can be downloaded at <http://www.gnu.org/software/autoconf>."$'\n\n'; fi
if `echo "$REQ" | grep -q "\[candy\]"`; then REQ_TESTS="${REQ_TESTS}* Candy"$'\n\n'; fi
if `echo "$REQ" | grep -q "\[fiona\]"`; then REQ_TESTS="${REQ_TESTS}* Fiona is used in the test cases to synthesize reference partners or operating guidelines. Without Fiona, some test cases of 'make check' may be skipped. Fiona can be downloaded at <http://service-technology.org/fiona>."$'\n\n'; fi
if `echo "$REQ" | grep -q lcov`; then REQ_TESTS="${REQ_TESTS}* LCOV is used to determine the test case code coverage of ${TOOLNAME}. Without LCOV, 'make cover' will not work. LCOV can be downloaded at <http://ltp.sourceforge.net/coverage/lcov.php>."$'\n\n'; fi
if `echo "$REQ" | grep -q "\[marlene\]"`; then REQ_TESTS="${REQ_TESTS}* Marlene"$'\n\n'; fi
if `echo "$REQ" | grep -q "\[mia\]"`; then REQ_TESTS="${REQ_TESTS}* Mia"$'\n\n'; fi
if `echo "$REQ" | grep -q "\[petri\]"`; then REQ_TESTS="${REQ_TESTS}* The Petri Net API Frontend Tool 'Petri' is used in some test cases to transform service models. Without Petri, some test cases of 'make check' may be skipped. Petri can be downloaded at <http://service-technology.org/pnapi>."$'\n\n'; fi
if ([ `echo "$REQ" | grep -q "\[wendy2fiona\]"` ] || [ `echo "$REQ" | grep -q "\[wendyFormula2bits\]"` ]); then REQ_TESTS="${REQ_TESTS}* The service-technology.org compilers are used to translate operating guideline file formats during the tests. Without them, some test cases of 'make check' may be skipped."$'\n\n'; fi
if `echo "$REQ" | grep -q "\[valgrind\]"`; then REQ_TESTS="${REQ_TESTS}* Valgrind is used to check for memory leaks. Without Valgrind, some test cases of 'make check' may be skipped. Valgrind can be downloaded at <http://valgrind.org>."$'\n\n'; fi
if `echo "$REQ" | grep -q "\[wendy\]"`; then REQ_TESTS="${REQ_TESTS}* Wendy"$'\n\n'; fi

# Code Manipulation
if `echo "$REQ" | grep -q AC_PROG_YACC`; then REQ_MANIPULATION="${REQ_MANIPULATION}* Bison is needed to generate the parsers from '.y' or '.yy' files. Bison can be downloaded at <http://www.gnu.org/s/bison>."$'\n\n'; fi
if `echo "$REQ" | grep -q AC_PROG_LEX`; then REQ_MANIPULATION="${REQ_MANIPULATION}* Flex is needed to generate scanners from '.l' or '.ll' files. Flex can be downloaded at <http://flex.sourceforge.net>."$'\n\n'; fi
if `echo "$REQ" | grep -q gengetopt`; then REQ_MANIPULATION="${REQ_MANIPULATION}* Gengetopt is needed to generate the command-line parsers from '.ggo' files. Gengetopt can be downloaded at <http://www.gnu.org/s/gengetopt>."$'\n\n'; fi
if `echo "$REQ" | grep -q kc++`; then REQ_MANIPULATION="${REQ_MANIPULATION}* Kimwitu++ is needed to generate the abstract syntax tree files from '.k' files. Kimwitu++ can be downloaded at <http://savannah.nongnu.org/projects/kimwitu-pp>."$'\n\n'; fi

# Other
if `echo "$REQ" | grep -q wget`; then REQ_OTHER="${REQ_OTHER}* Wget may be needed to download additional files such as source tarballs. Wget can be downloaded at <http://www.gnu.org/s/wget>."$'\n\n'; fi

if [ "$REQ_COMPILE" != "" ]; then
	echo "Compilation"
	echo "-----------"
	echo ""
	noindent "${TOOLNAME} is written in standard C/C++ and can be compiled using a modern compiler. ${TOOLNAME} should compile on any UNIX-like system such as GNU/Linux, Mac OS X, BSD systems, or Solaris. A compilation under Windows is usually possible using Cygwin, which can be downloaded at <http://www.cygwin.com>."
	echo ""
	indent "${REQ_COMPILE}"
fi

if [ "$REQ_RUNTIME" != "" ]; then
	echo "Runtime"
	echo "-------"
	echo ""
	noindent "${TOOLNAME} requires several tools at runtime. Though it may be cumbersome to install several tools prior to ${TOOLNAME}, it follows the UNIX philosophy of 'one task - one tool' and greatly simplifies development. Please make sure these tools are installed (viz. available in the PATH) prior to executing './configure'."
	echo ""
	indent "${REQ_RUNTIME}"
fi

if [ "$REQ_DOC" != "" ]; then
	echo "Documentation"
	echo "-------------"
	echo ""
	noindent "There exists several documentations for ${TOOLNAME}. The manual can be generated by executing 'make pdf'. The source code is further documented using Doxygen tags and this Doxygen documentation can be generated by running Doxygen in the 'doc' directory. The following tools are required to generate the documentation. As some tools are invoked by the standard Makefile target 'all', they may be even required to compile ${TOOLNAME}."
	echo ""
	indent "${REQ_DOC}"
fi

if [ "$REQ_TESTS" != "" ]; then
	echo "Tests"
	echo "-----"
	echo ""
	noindent "To make sure ${TOOLNAME} works properly, you can run a test suite by executing 'make check'. The following tools are required to execute this test suite. In case a test fail, please mail the file 'testsuite.log' (in the directory 'tests') to <${EMAIL}>."
	echo ""
	indent "${REQ_TESTS}"
fi

if [ "$REQ_DOC" != "" ]; then
	echo "Code Manipulation"
	echo "-----------------"
	echo ""
	noindent "${TOOLNAME} uses several code generators that translate domain-specific languages into C/C++ code. Editing these generated files does not make much sense as they might be overwritten automatically. Hence, editing the input files and using the code generators is strongly encouraged. Note that these tools are only required if you intend to make changes to ${TOOLNAME}."
	echo ""
	indent "${REQ_MANIPULATION}"
fi

if [ "$REQ_OTHER" != "" ]; then
	echo "Miscellaneous"
	echo "-------------"
	echo ""
	indent "${REQ_OTHER}"
fi





# REQUIREMENTS

DIR=".."
CONFSWITCH=""
CONFSWITCH="${CONFSWITCH}"$'\n'"`grep -Rh "AC_HEADER_ASSERT" ${DIR}/m4/* ${DIR}/*.ac | sort | uniq`"
CONFSWITCH="${CONFSWITCH}"$'\n'"`grep -Rh "AC_ARG_ENABLE"    ${DIR}/m4/* ${DIR}/*.ac | sort | uniq`"
CONFSWITCH="${CONFSWITCH}"$'\n'"`grep -Rh "AC_ARG_WITH"      ${DIR}/m4/* ${DIR}/*.ac | sort | uniq`"

#echo "${CONFSWITCH}"

CONFSWITCH_TEXT=""

if `echo "$CONFSWITCH" | grep -q AC_HEADER_ASSERT`; then CONFSWITCH_TEXT="${CONFSWITCH_TEXT}* --disable-assert - This switch disables assertions."$'\n\n'; fi
if `echo "$CONFSWITCH" | grep -q syslog`; then CONFSWITCH_TEXT="${CONFSWITCH_TEXT}* --enable-syslog - Status messages are printed to standard output or standard error by default. Using this parameter, they are additionally added to the syslog."$'\n\n'; fi
if `echo "$CONFSWITCH" | grep -q pnapi`; then CONFSWITCH_TEXT="${CONFSWITCH_TEXT}* --without-pnapi - The configure script will try to link against a preinstalled version of the Petri Net API (libpnapi). If this fails, a shipped version (see 'libs/pnapi') will be used instead. This parameter overrides this check and always uses the shipped version."$'\n\n'; fi

if [ "$CONFSWITCH_TEXT" != "" ]; then
echo "Configuration Switches"
echo "======================"
echo ""
indent "${CONFSWITCH_TEXT}"
fi


CONTRIBUTORS=`svn log -q ../src | grep -v "\-\-" | awk '{ print $3 }' | sort | uniq -c | sort -nr | awk '{ print "* " $2 " (" $1 ")" }'`
CONTRIBUTORS=`echo "${CONTRIBUTORS}" | sed 's/al020/Andreas Lehmann/;s/bat/Manja Wolf/;s/bretschn/Jan Bretschneider/;s/cas/Christian Sura/;s/christianstahl/Christian Stahl/;s/danitz/Robert Danitz/;s/delia/Delia Arsinte/;s/fahland/Dirk Fahland/;s/georgstraube/Georgi Straube/;s/gierds/Christian Gierds/;s/heiden/Andreas Heiden/;s/hw138/Harro Wimmel/;s/karstenwolf/Karsten Wolf/;s/kaschner/Kathrin Kaschner/;s/kathrin/Kathrin Kaschner/;s/kern/Leonard Kern/;s/kschmidt/Karsten Wolf/;s/massuthe/Peter Massuthe/;s/nielslohmann/Niels Lohmann/;s/niels/Niels Lohmann/;s/nlohmann/Niels Lohmann/;s/oliviao/Olivia Oanea/;s/parnjai/Jarungjit Parnjai/;s/reinert/Dennis Reinert/;s/rimueller/Richard Müller/;s/rimuelle/Richard Müller/;s/stephan/Stephan Mennicke/;s/suermeli/Jan Sürmeli/;s/theidin/Thomas Heidinger/;s/waltemath/Robert Waltemath/;s/weinberg/Daniela Weinberg/;s/znamirowski/Martin Znamirowski/;s/znamirow/Martin Znamirowski/'`


echo "Contributors"
echo "============"
echo ""
echo "${CONTRIBUTORS}"
echo ""




