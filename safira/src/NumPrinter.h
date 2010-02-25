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
