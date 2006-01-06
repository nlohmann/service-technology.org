# last changed $Date: 2006/01/06 20:48:19 $ by $Author: nlohmann $

.PHONY: clean

# Tools
TEXI2DVI = texi2dvi
DVIPS    = dvips

# Flags
TEXI2DVIFLAGS = --quiet
DVIPSFLAGS = -q


##############################################################################

dvi: bpel2owfn.dvi install.dvi

html: bpel2owfn.html install.html

ps: bpel2owfn.ps install.ps

pdf: bpel2owfn.pdf install.pdf

info: bpel2owfn.info install.info


##############################################################################

bpel2owfn.dvi: bpel2owfn.texi version.texi gpl.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) bpel2owfn.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) bpel2owfn.texi

install.dvi: install.texi version.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) install.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) install.texi	

bpel2owfn.html: bpel2owfn.texi
	$(MAKEINFO) $< --html --no-split --output=$@

install.html: install.texi
	$(MAKEINFO) $< --html --no-split --output=$@

bpel2owfn.ps: bpel2owfn.dvi
	$(DVIPS) $(DVIPSFLAGS) $< -o $@

install.ps: install.dvi
	$(DVIPS) $(DVIPSFLAGS) $< -o $@

bpel2owfn.pdf: bpel2owfn.texi version.texi gpl.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf bpel2owfn.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf bpel2owfn.texi

install.pdf: install.texi version.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf install.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf install.texi

bpel2owfn.info: bpel2owfn.texi
	$(MAKEINFO) $< --no-split --output=$@

install.info: install.texi
	$(MAKEINFO) $< --no-split --output=$@


###############################################################################
# This target creates a plain text version of the file `install.texi' and
# splits its chapters to the files README, CHANGES, AUTHORS and INSTALL in the
# root directory of the distribution.

distinstructions: install.texi
	$(MAKEINFO) $< -oinstall.txt --no-split --no-headers --no-number-section
	sed -n '/BEGIN OF README/,/END OF README/w ../README' install.txt
	sed -n '/BEGIN OF CHANGES/,/END OF CHANGES/w ../CHANGES' install.txt
	sed -n '/BEGIN OF AUTHORS/,/END OF AUTHORS/w ../AUTHORS' install.txt
	sed -n '/BEGIN OF INSTALL/,/END OF INSTALL/w ../INSTALL' install.txt
	rm install.txt

###############################################################################

clean:
	@echo removing temporary files...
	@-rm -f *.aux *.cp *.fn *.ky *.log *.pg *.tmp *.toc *.tp *.vr
	@-rm -f *~

distclean: clean
	@echo removing generated files...
	@-rm -f *.dvi *.pdf *.ps *.html *.info
