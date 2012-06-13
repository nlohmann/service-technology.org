noinst_LTLIBRARIES = liblpsolve55.la

INCLUDES = -I. -I$(srcdir)/shared -I$(srcdir)/bfp -I$(srcdir)/bfp/bfp_LUSOL -I$(srcdir)/bfp/bfp_LUSOL/LUSOL -I$(srcdir)/colamd

DEFINES = -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine

liblpsolve55_la_CFLAGS = -fPIC $(DEFINES)

liblpsolve55_la_SOURCES = lp_MDO.c lp_MDO.h \
	ini.c ini.h \
	fortify.c fortify.h \
	lp_crash.c lp_crash.h \
	lp_Hash.c lp_Hash.h \
	lp_lib.c lp_lib.h \
	lp_wlp.c lp_wlp.h \
	lp_matrix.c lp_matrix.h \
	lp_mipbb.c lp_mipbb.h \
	lp_MPS.c lp_MPS.h \
	lp_params.c \
	lp_presolve.c lp_presolve.h \
	lp_price.c lp_price.h \
	lp_pricePSE.c lp_pricePSE.h \
	lp_report.c lp_report.h \
	lp_scale.c lp_scale.h \
	lp_simplex.c lp_simplex.h \
	lp_SOS.c lp_SOS.h \
	lp_rlp.c lp_rlp.h \
	lp_utils.c lp_utils.h \
	yacc_read.c yacc_read.h \
	bfp/lp_BFP.h \
	bfp/bfp_LUSOL/lp_LUSOL.c bfp/bfp_LUSOL/lp_LUSOL.h \
	bfp/bfp_LUSOL/LUSOL/lusol.c bfp/bfp_LUSOL/LUSOL/lusol.h \
	colamd/colamd.c colamd/colamd.h \
	shared/commonlib.c shared/commonlib.h \
	shared/mmio.c shared/mmio.h \
	shared/myblas.c shared/myblas.h \
	declare.h lp_explicit.h lp_fortify.h lp_bit.h \
	lp_solveDLL.h lp_types.h lpkit.h lpsolve.h ufortify.h


EXTRA_DIST = bfp/lp_BFP1.c bfp/lp_BFP2.c \
	bfp/bfp_LUSOL/LUSOL/lusol2.c \
	bfp/bfp_LUSOL/LUSOL/lusol6a.c \
	bfp/bfp_LUSOL/LUSOL/lusol1.c \
	bfp/bfp_LUSOL/LUSOL/lusol7a.c \
	bfp/bfp_LUSOL/LUSOL/lusol8a.c \
	bfp/bfp_LUSOL/LUSOL/lusol6l0.c \
	bfp/bfp_LUSOL/LUSOL/lusol6u.c \
	lp_rlp.l lp_rlp.y \
	README.txt

# information for libtool
includedir = @includedir@/lp_solve_5.5

include_HEADERS = ini.h fortify.h lp_crash.h lp_Hash.h lp_lib.h \
	lp_wlp.h lp_matrix.h lp_mipbb.h lp_MPS.h lp_presolve.h lp_price.h \
	lp_pricePSE.h lp_report.h lp_scale.h lp_simplex.h lp_SOS.h lp_rlp.h \
	lp_utils.h yacc_read.h bfp/lp_BFP.h bfp/bfp_LUSOL/lp_LUSOL.h \
	bfp/bfp_LUSOL/LUSOL/lusol.h colamd/colamd.h shared/commonlib.h \
	shared/mmio.h shared/myblas.h declare.h lp_explicit.h lp_fortify.h \
	lp_solveDLL.h lp_types.h lpkit.h lpsolve.h ufortify.h lp_bit.h \
	lp_MDO.h
