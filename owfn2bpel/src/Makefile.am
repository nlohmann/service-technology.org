SUBDIRS = owfn2bpel_lola

bin_PROGRAMS = owfn2bpel

owfn2bpel_SOURCES = class_bpel.cc class_bpel.h \
	     	    class_place.cc class_place.h \
	       	    classes.h \
	  	    main.cc config.h \
		    class_owfn.cc class_owfn.h \
	      	    class_transition.cc class_transition.h \
		    options.h cmdline.c cmdline.h getopt.h getopt.c getopt1.c

BUILT_SOURCES = cmdline.c

EXTRA_DIST = cmdline.ggo

cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --long-help --include-getopt --show-required

MAINTAINERCLEANFILES = cmdline.c cmdline.h
