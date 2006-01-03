# last changed $Date: 2006/01/03 14:55:34 $ by $Author: nlohmann $

.PHONY: all clean distclean

all:
	$(MAKE) all --directory=src
	$(MAKE) all --directory=doc

clean:
	$(MAKE) clean --directory=src
	$(MAKE) clean --directory=doc
	$(MAKE) clean --directory=test

distclean: clean
	$(MAKE) distclean --directory=src
	$(MAKE) distclean --directory=doc

check:
	$(MAKE) check --directory=test
