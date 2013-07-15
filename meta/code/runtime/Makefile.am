AUTOMAKE_OPTIONS = subdir-objects

noinst_PROGRAMS = runtime_test
TESTS = ./runtime_test

#CXXFLAGS = -O3 -flto
#CXXFLAGS = -Wall -pedantic -Wextra -W -ansi -Wdocumentation

dependencies:
	svn co http://svn.gna.org/svn/service-tech/trunk/meta/code/json /tmp/json
	cp /tmp/json/src/JSON.cc /tmp/json/src/JSON.h src
	rm -fr /tmp/json

runtime_test_SOURCES = src/Runtime.cc src/Runtime.h test/runtime_test.cc src/JSON.cc src/JSON.h
runtime_test_CPPFLAGS = -I$(top_srcdir)/src

svn-clean: maintainer-clean
	rm -fr src/JSON.cc src/JSON.h
	rm -fr configure INSTALL aclocal.m4 build-aux depcomp install-sh missing test-driver
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done
