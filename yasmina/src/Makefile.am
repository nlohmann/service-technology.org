bin_PROGRAMS = yasmina

yasmina_SOURCES = syntax_mp.yy lexer_mp.ll \
                  parser_adapt_rulesmarlene.yy lexer_adapt_rulesmarlene.ll \
                  adapternumerge.h adapternumerge.cc \
                  main.cc \
                  cmdline.c cmdline.h \
                  verbose.cc verbose.h \
                  Output.cc Output.h \
                  eventTerm.h eventTerm.cc 

yasmina_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.a

yasmina_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve

if COMPILE_PNAPI
yasmina_CPPFLAGS += -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
yasmina_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
endif


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
