#ifndef NUMPRINTER_H_
#define NUMPRINTER_H_

#include <string>
using namespace std;

/// The abstract baseclass for num printers
/// which can transform a num into a string.
class NumPrinterBase {
public:
	/// Transforms the specified num into an string.
	virtual string printNum(const int num) const = 0;
};

/// The num printer implementation
/// which simply returns the num.
class NumPrinterSingle : public NumPrinterBase {
public:
	string printNum(const int num) const;
};


#endif /* NUMPRINTER_H_ */
