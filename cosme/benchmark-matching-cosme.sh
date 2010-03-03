echo "---Matching---"
echo "01. AP"
time cosme --matching -a tests/testfiles/OGs/AP.bits.og -c tests/testfiles/Services/AP-partner.owfn
echo ""
echo "02. Auction-Service"
time cosme --matching -a tests/testfiles/OGs/auctionService.bits.og -c tests/testfiles/Services/auctionService-partner.owfn
echo ""
echo "03. BH"
time cosme --matching -a tests/testfiles/OGs/BH.bits.og -c tests/testfiles/Services/BH-partner.owfn
echo ""
echo "04. CN"
time cosme --matching -a tests/testfiles/OGs/CN.bits.og -c tests/testfiles/Services/CN-partner.owfn
echo ""
echo "05. DG"
time cosme --matching -a tests/testfiles/OGs/DG.bits.og -c tests/testfiles/Services/DG-partner.owfn
echo ""
echo "06. Loan Approval Process"
time cosme --matching -a tests/testfiles/OGs/loanApprovalProcess.bits.og -c tests/testfiles/Services/loanApprovalProcess-partner.owfn
echo ""
echo "07. myCoffee"
time cosme --matching -a tests/testfiles/OGs/myCoffee.bits.og -c tests/testfiles/Services/myCoffee-partner.owfn
echo ""
echo "08. PH-Control 3"
time cosme --matching -a tests/testfiles/OGs/phcontrol.bits.og -c tests/testfiles/Services/phcontrol3.unf-partner.owfn
echo ""
echo "09. PO"
time cosme --matching -a tests/testfiles/OGs/PO.bits.og -c tests/testfiles/Services/PO-partner.owfn
echo ""
echo "10. QR"
time cosme --matching -a tests/testfiles/OGs/QR.bits.og -c tests/testfiles/Services/QR-partner.owfn
echo ""
echo "11. RE"
time cosme --matching -a tests/testfiles/OGs/RE.bits.og -c tests/testfiles/Services/RE-partner.owfn
echo ""
echo "12. RS"
time cosme --matching -a tests/testfiles/OGs/RS.bits.og -c tests/testfiles/Services/RS-partner.owfn
echo ""
echo "13. TR"
time cosme --matching -a tests/testfiles/OGs/TR.bits.og -c tests/testfiles/Services/TR-partner.owfn
echo ""

