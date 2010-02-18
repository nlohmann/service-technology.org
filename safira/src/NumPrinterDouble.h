#ifndef NUMPRINTERDOUBLE_H_
#define NUMPRINTERDOUBLE_H_

#include <map>
#include <string>
#include "types.h"
#include "Node.h"

/// The num printer implementation
/// which merges the num with all nums from another graph.
class NumPrinterDouble : public NumPrinterBase {
private:
	bool isForFirstFormula;
	map<Intpair, Nodepair*, Cmp>* nodepairs;

public:
	NumPrinterDouble(map<Intpair, Nodepair*, Cmp>* _nodepairs, bool _isForFirstFormula);
	string printNum(const int num) const;
};

#endif /* NUMPRINTERDOUBLE_H_ */
