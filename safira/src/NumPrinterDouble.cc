
#include "NumPrinterDouble.h"

#include "helpers.h"

NumPrinterDouble::NumPrinterDouble(map<Intpair, Nodepair*, Cmp>* _nodepairs, bool _isForFirstFormula)
	: nodepairs(nodepairs), isForFirstFormula(_isForFirstFormula) {
}

string NumPrinterDouble::printNum(const int num) const {
	/// hier was ordentliches machen
	return intToString(num);
}


