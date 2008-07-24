bin_PROGRAMS = lola


#lola_YFLAGS = --verbose


#######################
# issues to be solved #
#######################

# 1. The original Makefile set the following flags:
#
# lola_CXXFLAGS = -I bdd/bddlib -I bdd/mem -I dist -I comm -I storage $(CONFIG)
#
# As those directories are not present, they currently make no sense.
#
#
# 2. The following source files were in the original contribution, but not
#    listed in the Makefile:
#    - structure.cc
#    - test.cc


#######################
# the list of sources #
#######################

lola_SOURCES = readnet-syntax.yy \
	       readnet-lexic.ll \
	       net.cc net.H \
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

EXTRA_DIST = readnet-syntax.h
