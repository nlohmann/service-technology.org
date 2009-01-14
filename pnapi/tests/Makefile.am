SUBDIRS = . petrinet

noinst_LIBRARIES = libtest.a

libtest_a_SOURCES = test.h test.cc


# needed includes
AM_CPPFLAGS = -I$(top_srcdir)/src


# cannot be deleted because of SUBDIRS traversal
MAINTAINERCLEANFILES = petrinet/Makefile.in