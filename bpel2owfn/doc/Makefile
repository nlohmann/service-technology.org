# last changed $Date: 2006/01/05 09:14:22 $ by $Author: nlohmann $



.PHONY: clean

# Tools
TEXI2DVI  = texi2dvi
DVIPS     = dvips

# Flags
TEXI2DVIFLAGS = --quiet
DVIPSFLAGS = -q


###############################################################################
# Main target

dvi: bpel2owfn.texi gpl.texi version.texi install.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) bpel2owfn.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) bpel2owfn.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) install.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) install.texi

ps: dvi
	$(DVIPS) $(DVIPSFLAGS) bpel2owfn.dvi -o bpel2owfn.ps
	$(DVIPS) $(DVIPSFLAGS) install.dvi -o install.ps

pdf: dvi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf bpel2owfn.texi
	$(TEXI2DVI) $(TEXI2DVIFLAGS) --pdf install.texi

info: bpel2owfn.info install.info

bpel2owfn.info: bpel2owfn.texi
	$(MAKEINFO) $< --no-split --output=$@

install.info: install.texi
	$(MAKEINFO) $< --no-split --output=$@

html: bpel2owfn.html install.html

bpel2owfn.html: bpel2owfn.texi
	$(MAKEINFO) $< --html --no-split --output=$@

install.html: install.texi
	$(MAKEINFO) $< --html --no-split --output=$@

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
