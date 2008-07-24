bin_PROGRAMS = owfn2bpel_lola

owfn2bpel_lola_CXXFLAGS = -I bdd/bddlib -I bdd/mem -I dist -I comm -I storage $(CONFIG)
owfn2bpel_lola_CFLAGS = -I dist -I comm -I storage $(CONFIG)

owfn2bpel_lola_SOURCES = readnet-syntax.yy \
	       readnet-lexic.ll \
	       net.cc formula.cc stubborn.cc graph.cc symboltab.cc symm.cc \
	       check.H net.H symboltab.H unfold.H dimensions.H structure.H \
	       check.cc path.cc unfold.cc printnet.cc tinv.cc dimensions.cc \
	       sweep.cc \
	       check.H net.H symboltab.H unfold.H dimensions.H structure.H \
	       path.H symm.H userconfig.H formula.H stubborn.H printnet.H \
	       graph.H sweep.H tinv.H

EXTRA_DIST = readnet-syntax.h

MAINTAINERCLEANFILES = readnet-syntax.cc readnet-syntax.h readnet-lexic.cc
