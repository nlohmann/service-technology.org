###############################################################################
# UML2oWFN -- Translating IBM Business Modeler Processes into Petri Net Models
###############################################################################

# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src tests doc anon
else
SUBDIRS = src tests doc anon
endif


# maintainer - no maintainer directory yet

#############################################################################
# additional files
#############################################################################

EXTRA_DIST = scripts/Makefile

#############################################################################
# remove everything that can be re-built
#############################################################################
svn-clean: maintainer-clean
	rm -fr .DS_Store INSTALL Makefile.in aclocal.m4 compile configure depcomp \
	  install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub config.guess \
	  config.h.in~
	rm -fr libs/pnapi/Makefile.in
	rm -fr doc/.DS_Store doc/ChangeLog.txt doc/Makefile.in doc/mdate-sh doc/manual
	rm -fr src/.DS_Store src/Makefile.in src/config.h.in config.h.in~
	rm -fr tests/Makefile.in tests/.DS_Store doc/Makefile.in anon/Makefile.in doc/mdate-sh
	rm -fr maintainer/Makefile.in maintainer/.DS_Store
	rm -fr doc/doxygen doc/Doxyfile
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  aclocal"
	@echo "  autoconf"
	@echo "  autoheader"
	@echo "  automake -ac"
	@echo
	@echo "or"
	@echo "  autoreconf -i"
	@echo
	
#	rm -fr man/Makefile.in

##############################################################################
# compiler for windows
##############################################################################
win-bin: clean
	make all CXXFLAGS="-O3 -mno-cygwin -U _WIN32" LDFLAGS="-O3 -mno-cygwin -U _WIN32"