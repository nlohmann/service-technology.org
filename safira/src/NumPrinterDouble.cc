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

#include "NumPrinterDouble.h"

#include "helpers.h"

NumPrinterDouble::NumPrinterDouble(map<Intpair, Nodepair*, Cmp>* _nodepairs, bool _isForFirstFormula)
	: nodepairs(_nodepairs), isForFirstFormula(_isForFirstFormula) {
}

string NumPrinterDouble::printNum(const int num) const {
	/// hier was ordentliches machen
	return intToString(num);
}


