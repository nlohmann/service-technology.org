# the main program
bin_PROGRAMS = patricia

# the plain sources you need to compile (no generated code)
patricia_SOURCES = main.c \
                   actions.c actions.h \
                   tools.c tools.h \
                   types.c types.h \
                   utils.c utils.h \
                   tools/lola.c tools/lola.h tools/sara.c tools/sara.h

# pass the sysconfdir to Patricia (ugly, but see http://www.gnu.org/software/autoconf/manual/autoconf.html#Defining-Directories)
patricia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#include $(top_srcdir)/m4/Makefile.src
