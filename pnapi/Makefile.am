SUBDIRS = src tests utils utils/unfolder doc man

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# delete all files that can be created by other files
svn-clean: maintainer-clean
	rm -fr configure INSTALL aclocal.m4 src/config.h.in doc/mdate-sh \
	       src/config-log.h build-aux m4/libtool.m4 m4/lt~obsolete.m4 \
	       m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done
