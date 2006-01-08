# last changed $Date: 2006/01/08 15:32:42 $ by $Author: nlohmann $

.PHONY: clean

.SUFFIXES:
.SUFFIXES: .texi .html .ps .dvi .info

# Tools
MAKEINFO = makeinfo  # GNU standard
TEXI2DVI = texi2dvi  # GNU standard
DVIPS    = dvips

# Flags
TEXI2DVIFLAGS = --quiet
DVIPSFLAGS = -q
MAKEINFOFLAGS = --no-split

# Documentation files to be created
DOCUFILES = bpel2owfn install


##############################################################################

dvi: $(DOCUFILES:=.dvi)

html: $(DOCUFILES:=.html)

ps: $(DOCUFILES:=.ps)

pdf: $(DOCUFILES:=.pdf)

info: $(DOCUFILES:=.info)


##############################################################################

bpel2owfn.dvi: bpel2owfn.texi version.texi gpl.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) $<
	$(TEXI2DVI) $(TEXI2DVIFLAGS) $<

install.dvi: install.texi version.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) $<
	$(TEXI2DVI) $(TEXI2DVIFLAGS) $<

.texi.html:
	$(MAKEINFO) $< $(MAKEINFOFLAGS) --html --output=$@

.dvi.ps:
	$(DVIPS) $(DVIPSFLAGS) $< -o $@

bpel2owfn.pdf: bpel2owfn.texi version.texi gpl.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf $<
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf $<

install.pdf: install.texi version.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf $<
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf $<

.texi.info:
	$(MAKEINFO) $< $(MAKEINFOFLAGS) --output=$@

###############################################################################
# This target creates a plain text version of the file `install.texi' and
# splits its chapters to the files README, CHANGES, AUTHORS and INSTALL in the
# root directory of the distribution.

distinstructions: install.texi
	$(MAKEINFO) $< -oinstall.txt --no-split --no-headers --no-number-section
	sed -n '/BEGIN OF README/,/END OF README/w ../README' install.txt
	sed -n '/BEGIN OF ChangeLog/,/END OF ChangeLog/w ../ChangeLog' install.txt
	sed -n '/BEGIN OF AUTHORS/,/END OF AUTHORS/w ../AUTHORS' install.txt
	sed -n '/BEGIN OF INSTALL/,/END OF INSTALL/w ../INSTALL' install.txt
	rm install.txt

###############################################################################

clean:
	@echo removing temporary files...
	@-rm -f $(DOCUFILES:=.aux) $(DOCUFILES:=.cp) $(DOCUFILES:=.fn)\
	$(DOCUFILES:=.ky) $(DOCUFILES:=.log) $(DOCUFILES:=.pg)\
	$(DOCUFILES:=.tmp) $(DOCUFILES:=.toc) $(DOCUFILES:=.tp)\
	$(DOCUFILES:=.vr)
	@-rm -f *~

distclean: clean
	@echo removing generated files...
	@-rm -f $(DOCUFILES:=.dvi) $(DOCUFILES:=.pdf) $(DOCUFILES:=.ps)\
	$(DOCUFILES:=.html) $(DOCUFILES:=.info)
