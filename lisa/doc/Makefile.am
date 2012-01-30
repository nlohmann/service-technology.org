# the main Texinfo document (the manual)
info_TEXINFOS = lisa.texi

# any other files you need to add (images)
EXTRA_DIST = 

# the height of the README's ASCII art + URL + 2 spaces
BANNERHEIGHT = 10

# the help option that should be used in the manual
HELPOPTION = --full-help


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#--------------------------------------#
# GENERIC: add FDL and helper TeX file #
#--------------------------------------#
lisa_TEXINFOS = fdl.texi

#-----------------------------------------------------------#
# GENERIC: run tool to include help output in documentation #
#-----------------------------------------------------------#
cmdline.out: $(top_srcdir)/src/cmdline.ggo $(top_srcdir)/configure.ac
	$(top_builddir)/src/$(PACKAGE)$(EXEEXT) $(HELPOPTION) > cmdline.out
EXTRA_DIST += cmdline.out 
MAINTAINERCLEANFILES = cmdline.out
lisa_TEXINFOS += cmdline.out
