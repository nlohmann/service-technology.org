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
TEXINFO_TEX = texinfo.tex
lisa_TEXINFOS = fdl.texi

#-----------------------------------------------------------#
# GENERIC: run tool to include help output in documentation #
#-----------------------------------------------------------#
cmdline.out: $(top_srcdir)/src/cmdline.ggo $(top_srcdir)/configure.ac
	$(top_builddir)/src/$(PACKAGE) $(HELPOPTION) > cmdline.out
EXTRA_DIST += cmdline.out 
MAINTAINERCLEANFILES = cmdline.out
lisa_TEXINFOS += cmdline.out

#-----------------------------------------------------------#
# GENERIC: create ASCII files out README and Texinfo manual #
#-----------------------------------------------------------#
all: $(top_srcdir)/ChangeLog
lisa_TEXINFOS += ChangeLog.texi

$(top_srcdir)/ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=79 $(srcdir)/$(PACKAGE).texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/ChangeLog
	cat ChangeLog.txt >> $(top_srcdir)/ChangeLog
	head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/NEWS
	head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> $(top_srcdir)/
