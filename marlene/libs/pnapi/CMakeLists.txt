
# include file for calling Flex
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Flex.cmake)
# include file for calling Bison
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Bison.cmake)


########### next target ###############

SET(pnapi_STAT_SRCS
   pnapi.h
   link.h
   link.cc
   component.h
   component.cc
   petrinet.h
   petrinet.cc
   parser.h
   parser.cc
   util.h
   util.cc
   formula.h
   formula.cc
   condition.h
   condition.cc
   petrinet-reduction.cc
   petrinet-pn.cc
   automaton.h
   automaton.cc
   state.h
   state.cc
   marking.h
   marking.cc
   myio.h
   myio.cc
   io-format.cc
   errorhandling.cc
   Output.h
   Output.cc
)

ADD_BISON_FILES(pnapi_STAT_SRCS
    parser-owfn.yy
    parser-pn.yy
    parser-onwd.yy
    parser-lola.yy
    parser-sa.yy
)

ADD_FLEX_FILES(pnapi_STAT_SRCS    
    parser-owfn-lexer.ll
    parser-pn-lexer.ll
    parser-onwd-lexer.ll
    parser-lola-lexer.ll
    parser-sa-lexer.ll
)

add_library(pnapi STATIC ${pnapi_STAT_SRCS})

include_directories(${CMAKE_SOURCE_DIR}/libs/pnapi ${CMAKE_SOURCE_DIR}/vs_header ${CMAKE_SOURCE_DIR}/src)

add_definitions(-DHAVE_CONFIG_H -DYY_NO_UNISTD_H)

########### install files ###############




#original Makefile.am contents follow:

#
#noinst_LIBRARIES = libpnapi.a
#
#libpnapi_a_SOURCES = \
#	parser-owfn.yy    parser-owfn-lexer.ll \
#	parser-pn.yy parser-pn-lexer.ll \
#	parser-onwd.yy    parser-onwd-lexer.ll \
#	parser-lola.yy    parser-lola-lexer.ll \
#	parser-sa.yy	  parser-sa-lexer.ll \
#	pnapi.h \
#	link.h link.cc \
#	component.h component.cc \
#	petrinet.h petrinet.cc \
#	parser.h parser.cc \
#	util.h util.cc \
#	formula.h formula.cc \
#	condition.h condition.cc \
#	petrinet-reduction.cc \
#	petrinet-pn.cc \
#	automaton.h automaton.cc\
#	state.h state.cc \
#	marking.h marking.cc \
#	myio.h myio.cc io-format.cc \
#	errorhandling.cc \
#	Output.h Output.cc
#
#BUILT_SOURCES = parser-owfn.cc parser-owfn.h parser-owfn-lexer.cc \
#                parser-pn.cc parser-pn.h parser-pn-lexer.cc \
#		parser-onwd.cc parser-onwd.h parser-onwd-lexer.cc \
#		parser-lola.cc parser-lola.h parser-lola-lexer.cc \
#		parser-sa.cc parser-sa.h parser-sa-lexer.cc
#
#EXTRA_DIST = parser-owfn.h parser-pn.h parser-onwd.h parser-lola.h \
#	     parser-sa.h
#
## clean additional (generated) files
#DISTCLEANFILES = parser-owfn.h parser-owfn.cc parser-owfn-lexer.cc \
#		 parser-onwd.h parser-onwd.cc parser-onwd-lexer.cc \
#		 parser-pn.h parser-pn.cc parser-pn-lexer.cc \
#		 parser-lola.h parser-lola.cc parser-lola-lexer.cc \
#		 parser-sa.h parser-sa.cc parser-sa-lexer.cc
#
#
