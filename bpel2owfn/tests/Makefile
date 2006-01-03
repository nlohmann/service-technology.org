# last changed $Date: 2006/01/03 14:39:12 $ by $Author: nlohmann $



.PHONY: clean test1 test1 test2 test3

# Tools
BPEL2OWFN = ../src/bpel2owfn


###############################################################################

check: test1 test2 test3

test1:
	$(BPEL2OWFN) -f purchase.bpel

test2:
	$(BPEL2OWFN) -f loan.bpel

test3:
	$(BPEL2OWFN) -f shipping.bpel


###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f *.aux *.cp *.fn *.ky *.log *.pg *.tmp *.toc *.tp *.vr
	@-rm -f *~
