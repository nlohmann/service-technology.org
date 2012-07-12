SUBDIRS = doc

svn-clean: maintainer-clean
	rm -fr configure INSTALL aclocal.m4 src/config.h.in doc/mdate-sh src/config-log.h build-aux
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done
