echo "---simulation---"
echo "01. AP"
time cosme --simulation -a tests/testfiles/OGs/AP.bits.og -b tests/testfiles/OGs/AP.bits.og
echo ""
echo "02. Auction-Service"
time cosme --simulation -a tests/testfiles/OGs/auctionService.bits.og -b tests/testfiles/OGs/auctionService.bits.og
echo ""
echo "03. BH"
time cosme --simulation -a tests/testfiles/OGs/BH.bits.og -b tests/testfiles/OGs/BH.bits.og
echo ""
echo "04. CN"
time cosme --simulation -a tests/testfiles/OGs/CN.bits.og -b tests/testfiles/OGs/CN.bits.og
echo ""
echo "05. DG"
time cosme --simulation -a tests/testfiles/OGs/DG.bits.og -b tests/testfiles/OGs/DG.bits.og
echo ""
echo "06. Loan Approval Process"
time cosme --simulation -a tests/testfiles/OGs/loanApprovalProcess.bits.og -b tests/testfiles/OGs/loanApprovalProcess.bits.og
echo ""
echo "07. myCoffee"
time cosme --simulation -a tests/testfiles/OGs/myCoffee.bits.og -b tests/testfiles/OGs/myCoffee.bits.og
echo ""
echo "08. PH-Control 3"
time cosme --simulation -a tests/testfiles/OGs/phcontrol.bits.og -b tests/testfiles/OGs/phcontrol.bits.og
echo ""
echo "09. PO"
time cosme --simulation -a tests/testfiles/OGs/PO.bits.og -b tests/testfiles/OGs/PO.bits.og
echo ""
echo "10. QR"
time cosme --simulation -a tests/testfiles/OGs/QR.bits.og -b tests/testfiles/OGs/QR.bits.og
echo ""
echo "11. RE"
time cosme --simulation -a tests/testfiles/OGs/RE.bits.og -b tests/testfiles/OGs/RE.bits.og
echo ""
echo "12. RS"
time cosme --simulation -a tests/testfiles/OGs/RS.bits.og -b tests/testfiles/OGs/RS.bits.og
echo ""
echo "13. TPO"
time cosme --simulation -a tests/testfiles/OGs/TPO.bits.og -b tests/testfiles/OGs/TPO.bits.og
echo ""
echo "14. TR"
time cosme --simulation -a tests/testfiles/OGs/TR.bits.og -b tests/testfiles/OGs/TR.bits.og
echo ""

