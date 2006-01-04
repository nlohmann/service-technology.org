# last changed $Date: 2006/01/04 23:18:29 $ by $Author: nlohmann $



.PHONY: clean test1 test1 test2 test3

# Tools
BPEL2OWFN = ../src/bpel2owfn


###############################################################################

check: test1 test2 test3

test1:
	$(BPEL2OWFN) --version
	$(BPEL2OWFN) --file purchase.bpel
	$(BPEL2OWFN) --file loan.bpel
	$(BPEL2OWFN) --file shipping.bpel

test2:
	$(BPEL2OWFN) --file purchase.bpel -O2F
	$(BPEL2OWFN) --file loan.bpel -L2F
	$(BPEL2OWFN) --file shipping.bpel -pn -s -D2F

test3:
	-$(BPEL2OWFN) --file error.bpel
	-$(BPEL2OWFN) --file foo.bpel

###############################################################################
clean:
	@echo removing temporary files...
	@-rm -f *.info *.lola *.owfn *.dot
