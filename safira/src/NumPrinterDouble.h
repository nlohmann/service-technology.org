/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

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
