# last changed $Date: 2006/01/04 20:47:21 $ by $Author: nlohmann $



.PHONY: clean

# Tools
TEXI2DVI  = texi2dvi
DVIPS     = dvips

# Flags
TEXI2DVIFLAGS = --quiet
DVIPSFLAGS = -q


###############################################################################
# Main target

all: dvi ps pdf

dvi: bpel2owfn.texi gpl.texi version.texi
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

###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f *.aux *.cp *.fn *.ky *.log *.pg *.tmp *.toc *.tp *.vr
	@-rm -f *~

distclean: clean
	@echo removing generated files...
	@-rm -f *.dvi *.pdf *.ps
