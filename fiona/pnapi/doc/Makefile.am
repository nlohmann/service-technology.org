# the main Texinfo document (the manual)
info_TEXINFOS = pnapi.texi

# any other files you need to add (images)
EXTRA_DIST = templates/parser-FORMAT.yy \
	     templates/parser-FORMAT-lexer.ll \
	     templates/parser-FORMAT-wrapper.h \
	     templates/parser-FORMAT-wrapper.cc

# the height of the README's ASCII art + URL + 3 spaces
BANNERHEIGHT = 9

# the help option that should be used in the manual
HELPOPTION = --full-help


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

#--------------------------------------#
# GENERIC: add FDL and helper TeX file #
#--------------------------------------#
TEXINFO_TEX = texinfo.tex
pnapi_TEXINFOS = fdl.texi

#-----------------------------------------------------------#
# GENERIC: run tool to include help output in documentation #
#-----------------------------------------------------------#
cmdline.out: $(top_srcdir)/utils/cmdline.ggo $(top_srcdir)/configure.ac
	$(AM_V_GEN)$(top_builddir)/utils/petri $(HELPOPTION) > cmdline.out
EXTRA_DIST += cmdline.out 
MAINTAINERCLEANFILES = cmdline.out
pnapi_TEXINFOS += cmdline.out

#-----------------------------------------------------------#
# GENERIC: create ASCII files out README and Texinfo manual #
#-----------------------------------------------------------#
all: $(top_srcdir)/ChangeLog $(top_srcdir)/REQUIREMENTS $(top_srcdir)/THEORY
pnapi_TEXINFOS += ChangeLog.texi
EXTRA_DIST += $(top_srcdir)/REQUIREMENTS $(top_srcdir)/THEORY

$(top_srcdir)/ChangeLog: ChangeLog.texi
	$(AM_V_GEN)
	$(AM_V_at)$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=79 $(srcdir)/$(PACKAGE).texi
	$(AM_V_at)$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(AM_V_at)$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	$(AM_V_at)rm ChangeLog2.txt
	$(AM_V_at)head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/ChangeLog
	$(AM_V_at)cat ChangeLog.txt >> $(top_srcdir)/ChangeLog
	$(AM_V_at)head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/NEWS
	$(AM_V_at)head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> $(top_srcdir)/NEWS
	$(AM_V_at)echo "\nSee file ChangeLog <http://service-technology.org/files/$(PACKAGE)/ChangeLog> for\ncomplete version history.\n" >> $(top_srcdir)/NEWS
	$(AM_V_at)rm ChangeLog.txt

$(top_srcdir)/REQUIREMENTS: $(PACKAGE).texi
	$(AM_V_GEN)
	$(AM_V_at)$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --no-number-sections --fill-column=79 $(srcdir)/$(PACKAGE).texi
	$(AM_V_at)$(SED) -n '/In order to run, compile, and/,/development./w ChangeLog2.txt' ChangeLog.txt
	$(AM_V_at)head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/REQUIREMENTS
	$(AM_V_at)cat ChangeLog2.txt >> $(top_srcdir)/REQUIREMENTS
	$(AM_V_at)echo "" >> $(top_srcdir)/REQUIREMENTS
	$(AM_V_at)rm ChangeLog.txt ChangeLog2.txt

$(top_srcdir)/THEORY: $(PACKAGE).texi
	$(AM_V_GEN)
	$(AM_V_at)$(MAKEINFO) -o THEORY.txt --no-split --no-headers --no-number-sections --fill-column=79 $(srcdir)/$(PACKAGE).texi
	$(AM_V_at)$(SED) -n '/$(PACKAGE_NAME) implements several algorithms/,/for more information./w THEORY2.txt' THEORY.txt
	$(AM_V_at)head -n$(BANNERHEIGHT) $(top_srcdir)/README > $(top_srcdir)/THEORY
	$(AM_V_at)cat THEORY2.txt >> $(top_srcdir)/THEORY
	$(AM_V_at)echo "" >> $(top_srcdir)/THEORY
	$(AM_V_at)rm THEORY.txt THEORY2.txt
