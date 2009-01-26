SUBDIRS = . petrinet automaton

check_LIBRARIES = libtest.a

libtest_a_SOURCES = test.h test.cc


# needed includes
AM_CPPFLAGS = -I$(top_srcdir)/src

MAINTAINERCLEANFILES = petrinet/Makefile.in automaton/Makefile.in
