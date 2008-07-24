bin_PROGRAMS = generator

# the list of sources
generator_SOURCES = main.cc

generator_LDADD = lib_pnapi.a

lib_LIBRARIES = lib_pnapi.a

lib_pnapi_a_SOURCES = pnapi/pnapi.cc pnapi/pnapi.h\
		      pnapi/helpers.cc pnapi/helpers.h \
		      pnapi/options.h \
		      pnapi/petrinet-output.cc \
		      pnapi/petrinet-reduction.cc \
		      pnapi/petrinet.cc \
		      pnapi/petrinet.h
