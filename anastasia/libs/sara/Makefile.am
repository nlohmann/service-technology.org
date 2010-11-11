noinst_LIBRARIES = libsara.a

libsara_a_SOURCES = main.cc problem.cc problem.h reachalyzer.cc reachalyzer.h imatrix.cc imatrix.h constraint.cc constraint.h lpwrapper.cc lpwrapper.h pathfinder.cc pathfinder.h partialsolution.cc partialsolution.h jobqueue.cc jobqueue.h verbose.cc verbose.h tarjan.cc tarjan.h sara_lib.h

libsara_a_CPPFLAGS = -DSARALIB -I. -I$(top_srcdir)/src -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi -I$(top_srcdir)/libs/lp_solve
#sara_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
#if COMPILE_PNAPI
#sara_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
#endif

# information for libtool
includedir = @includedir@/sara

include_HEADERS = problem.h reachalyzer.h imatrix.h constraint.h lpwrapper.h pathfinder.h partialsolution.h jobqueue.h tarjan.h

