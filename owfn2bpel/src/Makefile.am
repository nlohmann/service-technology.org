bin_PROGRAMS = owfn2bpel owfn2bpel_lola

owfn2bpel_SOURCES = BPEL.cc BPEL.h \
	     	    Place.cc Place.h \
	       	    classes.h \
	  	    main.cc config.h \
		    class_owfn.cc class_owfn.h \
	      	    Transition.cc Transition.h \
		    cmdline.c cmdline.h getopt.h getopt.c getopt1.c

BUILT_SOURCES = cmdline.c

owfn2bpel_lola_SOURCES = $(LOLANAME).tar.gz create_lola

EXTRA_DIST = cmdline.ggo userconfig.H

cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --long-help --include-getopt --show-required

MAINTAINERCLEANFILES = cmdline.c cmdline.h

CLEANFILES = owfn2bpel_lola

# create a LoLA to check the safeness of the nets from the given source
# tarball announced in the configure script as "LOLANAME".
owfn2bpel_lola$(EXEEXT): $(LOLANAME).tar.gz
	gunzip -c $(srcdir)/$(LOLANAME).tar.gz > $(builddir)/$(LOLANAME).tar
	tar xf $(builddir)/$(LOLANAME).tar
	rm $(builddir)/$(LOLANAME).tar
	mv $(LOLANAME) lola
	cp $(srcdir)/userconfig.H lola/src
	-cp $(abs_top_srcdir)/config.status lola
	cd $(builddir) ; $(SHELL) $(srcdir)/create_lola
	cp lola/src/lola $(builddir)/owfn2bpel_lola
	rm -fr lola
