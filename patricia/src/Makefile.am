# the main program
bin_PROGRAMS = patricia

# the plain sources you need to compile (no generated code)
patricia_SOURCES = cmdline.c cmdline.h main.cc \
                   verbose.cc verbose.h \
                   Output.cc Output.h \
                   action.cc action.h \
				   ptask.cc ptask.h \
                   tools.cc tools.h \
                   outcome.cc outcome.h \
                   problem.cc problem.h \
                   tools/lola.cc tools/lola.h tools/sara.cc tools/sara.h

# pass the sysconfdir to Patricia (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
patricia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"
patricia_LDADD = -lpthread

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
