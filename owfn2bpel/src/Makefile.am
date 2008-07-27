bin_PROGRAMS = owfn2bpel

all: owfn2bpel_lola

owfn2bpel_SOURCES = class_bpel.cc class_bpel.h \
	     	    class_place.cc class_place.h \
	       	    classes.h \
	  	    main.cc config.h \
		    class_owfn.cc class_owfn.h \
	      	    class_transition.cc class_transition.h \
		    options.h cmdline.c cmdline.h getopt.h getopt.c getopt1.c

BUILT_SOURCES = cmdline.c

EXTRA_DIST = cmdline.ggo $(LOLANAME).tar.gz create_lola userconfig.H

cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --long-help --include-getopt --show-required

MAINTAINERCLEANFILES = cmdline.c cmdline.h

CLEANFILES = owfn2bpel_lola

# create a LoLA to check the safeness of the nets from the given source
# tarball announced in the configure script as "LOLANAME".
owfn2bpel_lola: $(LOLANAME).tar.gz
	tar xfz $(srcdir)/$(LOLANAME).tar.gz
	mv $(LOLANAME) lola
	cp $(srcdir)/userconfig.H lola/src
	$(srcdir)/create_lola
	cp lola/src/lola $(builddir)/owfn2bpel_lola
	rm -fr lola
