###########################################################################
# Libraries
###########################################################################

# these libraries have to be built
noinst_LIBRARIES = libmnem.a libcudd.a \
                   libdddmp.a libmtr.a \
                   libst.a libutil.a \
                   libepd.a libobj.a

#---------------------------------------------------------------------------

libmnem_a_SOURCES = mnemosyne/mnemalyse.c \
                    mnemosyne/mnemosyne.c \
                    mnemosyne/mnemosyne.h \
                    mnemosyne/mnemconf.h

libmnem_a_CFLAGS = -DMALLOC_IS_VOIDSTAR @CUDD_XFLAGS@

#---------------------------------------------------------------------------

libcudd_a_SOURCES = cudd/cuddAPI.c \
                    cudd/cuddAddAbs.c \
                    cudd/cuddAddApply.c \
                    cudd/cuddAddFind.c \
                    cudd/cuddAddIte.c \
                    cudd/cuddAddInv.c \
                    cudd/cuddAddNeg.c \
                    cudd/cuddAddWalsh.c \
                    cudd/cuddAndAbs.c \
                    cudd/cuddAnneal.c \
                    cudd/cuddApa.c \
                    cudd/cuddApprox.c \
                    cudd/cuddBddAbs.c \
                    cudd/cuddBddCorr.c \
                    cudd/cuddBddIte.c \
                    cudd/cuddBridge.c \
                    cudd/cuddCache.c \
                    cudd/cuddCheck.c \
                    cudd/cuddClip.c \
                    cudd/cuddCof.c \
                    cudd/cuddCompose.c \
                    cudd/cuddDecomp.c \
                    cudd/cuddEssent.c \
                    cudd/cuddExact.c \
                    cudd/cuddExport.c \
                    cudd/cuddGenCof.c \
                    cudd/cuddGenetic.c \
                    cudd/cuddGroup.c \
                    cudd/cuddHarwell.c \
                    cudd/cuddInit.c \
                    cudd/cuddInteract.c \
                    cudd/cuddLCache.c \
                    cudd/cuddLevelQ.c \
                    cudd/cuddLinear.c \
                    cudd/cuddLiteral.c \
                    cudd/cuddMatMult.c \
                    cudd/cuddPriority.c \
                    cudd/cuddRead.c \
                    cudd/cuddRef.c \
                    cudd/cuddReorder.c \
                    cudd/cuddSat.c \
                    cudd/cuddSign.c \
                    cudd/cuddSolve.c \
                    cudd/cuddSplit.c \
                    cudd/cuddSubsetHB.c \
                    cudd/cuddSubsetSP.c \
                    cudd/cuddSymmetry.c \
                    cudd/cuddTable.c \
                    cudd/cuddUtil.c \
                    cudd/cuddWindow.c \
                    cudd/cuddZddCount.c \
                    cudd/cuddZddFuncs.c \
                    cudd/cuddZddGroup.c \
                    cudd/cuddZddIsop.c \
                    cudd/cuddZddLin.c \
                    cudd/cuddZddMisc.c \
                    cudd/cuddZddPort.c \
                    cudd/cuddZddReord.c \
                    cudd/cuddZddSetop.c \
                    cudd/cuddZddSymm.c \
                    cudd/cuddZddUtil.c \
                    cudd/cudd.h \
                    cudd/cuddInt.h

libcudd_a_CFLAGS = @CUDD_XFLAGS@
libcudd_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libdddmp_a_SOURCES = dddmp/dddmpStoreBdd.c \
                     dddmp/dddmpStoreAdd.c \
                     dddmp/dddmpStoreCnf.c \
                     dddmp/dddmpLoad.c \
                     dddmp/dddmpLoadCnf.c \
                     dddmp/dddmpNodeBdd.c \
                     dddmp/dddmpNodeAdd.c \
                     dddmp/dddmpNodeCnf.c \
                     dddmp/dddmpStoreMisc.c \
                     dddmp/dddmpUtil.c \
                     dddmp/dddmpBinary.c \
                     dddmp/dddmpConvert.c \
                     dddmp/dddmpDbg.c \
                     dddmp/dddmp.h \
                     dddmp/dddmpInt.h

libdddmp_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libmtr_a_SOURCES = mtr/mtrBasic.c \
                   mtr/mtrGroup.c \
                   mtr/mtr.h

libmtr_a_CFLAGS = @CUDD_XFLAGS@
libmtr_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libst_a_SOURCES = st/st.c \
                  st/st.h

libst_a_CFLAGS = @CUDD_XFLAGS@
libst_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libutil_a_SOURCES = util/cpu_time.c \
                    util/cpu_stats.c \
                    util/safe_mem.c \
                    util/strsav.c \
                    util/texpand.c \
                    util/ptime.c \
                    util/prtime.c \
                    util/pipefork.c \
                    util/pathsearch.c \
                    util/stub.c \
                    util/datalimit.c \
                    util/util.h

libutil_a_CFLAGS = @CUDD_XFLAGS@
libutil_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libepd_a_SOURCES = epd/epd.c \
                  epd/epd.h

libepd_a_CFLAGS = @CUDD_XFLAGS@
libepd_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------

libobj_a_SOURCES = obj/cuddObj.cc \
                   obj/cuddObj.hh

#libobj_a_CXXFLAGS = @CUDD_XFLAGS@
libobj_a_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -DUNIX

#---------------------------------------------------------------------------


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

