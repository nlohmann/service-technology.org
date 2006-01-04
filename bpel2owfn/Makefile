# last changed $Date: 2006/01/04 22:51:54 $ by $Author: nlohmann $

.PHONY: all clean distclean

# Names
DISTNAME = bpel2owfn-1.0

# Files
DISTFILES = AUTHORS CHANGES COPYING INSTALL README Makefile
DISTDIRS  = doc src

all:
	$(MAKE) all --directory=src

clean:
	$(MAKE) clean --directory=src
	$(MAKE) clean --directory=doc
	$(MAKE) clean --directory=test

distclean: clean
	$(MAKE) distclean --directory=src
	$(MAKE) distclean --directory=doc
	-rm -f -r $(DISTNAME)
	-rm -f $(DISTNAME).tar.gz

check:
	$(MAKE) check --directory=test

dist: distclean
	mkdir $(DISTNAME)
	chmod 777 $(DISTNAME)
	cp -R $(DISTDIRS) $(DISTNAME)
	cp $(DISTFILES) $(DISTNAME)
	tar cvf $(DISTNAME).tar $(DISTNAME)
	gzip $(DISTNAME).tar
	-rm -f -r $(DISTNAME)

dvi:
	$(MAKE) dvi --directory=doc

html:
	$(MAKE) html --directory=doc

pdf:
	$(MAKE) pdf --directory=doc

ps:
	$(MAKE) ps --directory=doc

info:
	$(MAKE) info --directory=doc
