###########################################################################
# Libraries
###########################################################################

# these libraries have to be built
noinst_LIBRARIES = libobj.a libdddmp.a libcudd.a libmtr.a \
	libst.a libutil.a libepd.a

# sources for libobj.a -- CUDD's C++ interface
libobj_a_SOURCES = obj/cuddObj.cc obj/cuddObj.hh \
	cudd/cudd.h

# sources for libdddmp.a -- Decision Diagram DuMP
libdddmp_a_SOURCES = dddmp/dddmpStoreBdd.c dddmp/dddmpStoreAdd.c \
	dddmp/dddmpStoreCnf.c dddmp/dddmpLoad.c dddmp/dddmpLoadCnf.c \
	dddmp/dddmpNodeBdd.c dddmp/dddmpNodeAdd.c dddmp/dddmpNodeCnf.c \
	dddmp/dddmpStoreMisc.c dddmp/dddmpUtil.c dddmp/dddmpBinary.c \
	dddmp/dddmpConvert.c dddmp/dddmpDbg.c \
	dddmp/dddmp.h dddmp/dddmpInt.h \
	cudd/cudd.h cudd/cuddInt.h

# sources for libcudd.a -- DD package
libcudd_a_SOURCES = cudd/cuddAPI.c cudd/cuddAddAbs.c cudd/cuddAddApply.c \
	cudd/cuddAddFind.c cudd/cuddAddIte.c cudd/cuddAddInv.c \
	cudd/cuddAddNeg.c cudd/cuddAddWalsh.c cudd/cuddAndAbs.c \
	cudd/cuddAnneal.c cudd/cuddApa.c cudd/cuddApprox.c \
	cudd/cuddBddAbs.c cudd/cuddBddCorr.c cudd/cuddBddIte.c \
	cudd/cuddBridge.c cudd/cuddCache.c cudd/cuddCheck.c \
	cudd/cuddClip.c cudd/cuddCof.c cudd/cuddCompose.c cudd/cuddDecomp.c \
	cudd/cuddEssent.c cudd/cuddExact.c cudd/cuddExport.c \
	cudd/cuddGenCof.c cudd/cuddGenetic.c cudd/cuddGroup.c \
	cudd/cuddHarwell.c cudd/cuddInit.c cudd/cuddInteract.c \
	cudd/cuddLCache.c cudd/cuddLevelQ.c cudd/cuddLinear.c \
	cudd/cuddLiteral.c cudd/cuddMatMult.c cudd/cuddPriority.c \
	cudd/cuddRead.c cudd/cuddRef.c cudd/cuddReorder.c cudd/cuddSat.c \
	cudd/cuddSign.c cudd/cuddSolve.c cudd/cuddSplit.c \
	cudd/cuddSubsetHB.c cudd/cuddSubsetSP.c cudd/cuddSymmetry.c \
	cudd/cuddTable.c cudd/cuddUtil.c cudd/cuddWindow.c \
	cudd/cuddZddCount.c cudd/cuddZddFuncs.c cudd/cuddZddGroup.c \
	cudd/cuddZddIsop.c cudd/cuddZddLin.c cudd/cuddZddMisc.c \
	cudd/cuddZddPort.c cudd/cuddZddReord.c cudd/cuddZddSetop.c \
	cudd/cuddZddSymm.c cudd/cuddZddUtil.c \
	cudd/cudd.h cudd/cuddInt.h

# sources for libmtr.a -- multiway-branching tree package
libmtr_a_SOURCES = mtr/mtrBasic.c mtr/mtrGroup.c mtr/mtr.h

# sources for libst.a -- hash table package
libst_a_SOURCES = st/st.c st/st.h

# sources for libutil.a -- miscellaneous utility routines
libutil_a_SOURCES = util/cpu_time.c util/cpu_stats.c util/getopt.c \
	util/safe_mem.c util/strsav.c util/texpand.c util/ptime.c \
	util/prtime.c util/pipefork.c util/pathsearch.c util/stub.c \
	util/tmpfile.c util/datalimit.c util/util.h \
	$(top_srcdir)/src/mynew.h $(top_srcdir)/src/newlogger.cc

# sources for libepd.a -- extended precision
libepd_a_SOURCES = epd/epd.c epd/epd.h


# additional headers that have to be distributed
EXTRA_DIST = include/cudd.h \
	include/cuddInt.h \
	include/cuddObj.hh \
	include/dddmp.h \
	include/epd.h \
	include/mnemosyne.h \
	include/mtr.h \
	include/st.h \
	include/util.h \
	mtr/mtrInt.h \
	testobj.sh \
	INSTALL LICENSE README RELEASE.NOTES


###########################################################################
# Flags
###########################################################################

# inherit the flags set by the configures script
AM_CFLAGS   = @CUDD_XFLAGS@ @configured_CFLAGS@
AM_CXXFLAGS = @CUDD_XFLAGS@ @configured_CXXFLAGS@
AM_LDFLAGS  = @CUDD_XFLAGS@ @configured_LDFLAGS@

# * Why do I need "nanotrav"? Does not compile anyway...
# * Why do I need "mtr"? "mtr.h" is distributed anyway...
# * To use "include", I have to take care of the linked files.
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/libs/cudd/include \
	-I$(top_srcdir)/libs/cudd/nanotrav \
	-I$(top_srcdir)/libs/cudd/mtr

# use the C++ compiler for CUDD's sources
CC = $(CXX)

###########################################################################
# Tests
###########################################################################

# test program for the C++ object-oriented encapsulation of CUDD
TESTS = testobj.sh
check_PROGRAMS = testobj
testobj_SOURCES = obj/testobj.cc include/cudd.h test-helper.cc
testobj_LDADD = libobj.a libdddmp.a libcudd.a libmtr.a \
	libst.a libutil.a libepd.a
