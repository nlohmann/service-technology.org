bin_PROGRAMS = lola

#######################
# the list of sources #
#######################

lola_SOURCES = readnet-syntax.yy \
	       readnet-lexic.ll \
	       net.cc net.H \
	       buchi.cc buchi.H \
	       formula.cc formula.H \
	       stubborn.cc stubborn.H \
	       graph.cc graph.H \
	       symboltab.cc symboltab.H \
	       symm.cc symm.H \
	       check.cc check.H \
	       unfold.cc unfold.H \
	       dimensions.cc dimensions.H \
	       path.cc path.H \
	       printnet.cc printnet.H \
	       tinv.cc tinv.H \
	       sweep.cc sweep.H \
	       structure.H \
	       userconfig.H 

lola_EXTRASOURCES = ltl.H ltl.cc

EXTRA_DIST = readnet-syntax.h

lola_CFLAGS = @configured_CFLAGS@
lola_CXXFLAGS = @configured_CXXFLAGS@
lola_LDFLAGS = @configured_LDFLAGS@
