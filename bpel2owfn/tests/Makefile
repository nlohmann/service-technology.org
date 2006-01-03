# last changed $Date: 2006/01/03 14:54:07 $ by $Author: nlohmann $



.PHONY: clean test1 test1 test2 test3

# Tools
BPEL2OWFN = ../src/bpel2owfn


###############################################################################

check: test1 test2 test3

test1:
	$(BPEL2OWFN) -f purchase.bpel
	$(BPEL2OWFN) -f loan.bpel
	$(BPEL2OWFN) -f shipping.bpel

test2:
	$(BPEL2OWFN) -f purchase.bpel -O2F
	$(BPEL2OWFN) -f loan.bpel -L2F
	$(BPEL2OWFN) -f shipping.bpel -pn -s -D2F

test3:
	-$(BPEL2OWFN) -f error.bpel
	-$(BPEL2OWFN) -f foo.bpel

###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f *.info *.lola *.owfn *.dot
