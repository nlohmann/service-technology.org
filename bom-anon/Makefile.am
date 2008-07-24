SUBDIRS = src doc

cvs-clean: maintainer-clean
	rm -fr .DS_Store INSTALL Makefile.in aclocal.m4 compile configure depcomp install-sh missing ylwrap @PACKAGE_TARNAME@-@VERSION@.zip config.guess config.sub
	rm -fr doc/.DS_Store doc/Makefile.in doc/mdate-sh
	rm -fr src/.DS_Store src/Makefile.in

win-bin: clean
	make all CXXFLAGS="-O3 -mno-cygwin -U _WIN32" LDFLAGS="-O3 -mno-cygwin -U _WIN32"

bin-dist: dist pdf
	make all
	mkdir @PACKAGE_TARNAME@-@VERSION@
	mv @PACKAGE_TARNAME@-@VERSION@.tar.gz @PACKAGE_TARNAME@-@VERSION@/sources.tar.gz
	cp doc/@PACKAGE_TARNAME@.pdf @PACKAGE_TARNAME@-@VERSION@/readme.pdf
	cp src/@PACKAGE_TARNAME@@EXEEXT@ @PACKAGE_TARNAME@-@VERSION@
	zip @PACKAGE_TARNAME@-@VERSION@.zip @PACKAGE_TARNAME@-@VERSION@/*
	rm -fr @PACKAGE_TARNAME@-@VERSION@