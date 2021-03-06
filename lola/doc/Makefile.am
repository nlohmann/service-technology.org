info_TEXINFOS = lola.texi

lola_TEXINFOS = ChangeLog.texi graph2dot.out figs fdl.texi

EXTRA_DIST = gendocs.sh gendocs_template

MAINTAINERCLEANFILES = graph2dot.out


#############################################################################
# when a HTML is installed, the GIF figures are installed as well
#############################################################################
install-html-local:
	test -z "$(pdfdir)" || $(MKDIR_P) "$(DESTDIR)$(htmldir)/figs"
	$(INSTALL_DATA) -t $(DESTDIR)$(htmldir)/figs figs/*.gif

#############################################################################
# create ASCII ChangeLog/NEWS out of Texinfo (8 is height of header)
#############################################################################
all: ../ChangeLog
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=81 lola.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' | $(SED) 's/^   //' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n8 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	head -n8 ../README > ../NEWS
	head -n `($(GREP) -n -m2 "^Version " ChangeLog.txt | $(SED) -n '2p' | $(SED) -e 's/:/ /' | $(AWK) '{print $$1-1}')` ChangeLog.txt >> ../NEWS
	echo "\nSee file ChangeLog <http://service-technology.org/files/$(PACKAGE)/ChangeLog> for\ncomplete version history.\n" >> ../NEWS
	rm ChangeLog.txt


#############################################################################
# collect the "--help" outputs of the utilities for the documentation
#############################################################################
graph2dot.out: $(top_srcdir)/utils/cmdline-graph2dot.ggo $(top_srcdir)/configure.ac
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(top_srcdir)/utils
	$(AM_V_GEN)$(top_srcdir)/utils/graph2dot$(EXEEXT) --help > graph2dot.out


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: $(info_TEXINFOS)
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"
	cp -R figs manual
	cp -R figs manual/html_node
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"
