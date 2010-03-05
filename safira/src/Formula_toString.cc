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

#include <config.h>
#include <iostream>
#include "Formula.h"
#include "map"

extern map<int, string> id2label;
extern map<string, int> label2id;


/****************************************************************************
 * string output
 ***************************************************************************/

string Formula::toString() const {
	NumPrinterBase* printer = new NumPrinterSingle();
	string s = this->toString(printer);
	delete printer;
	return s;
}



/****************************************************************************
 * extended string output
 ***************************************************************************/

string FormulaAND::toString(NumPrinterBase* printer) const {
    return ("(" + left->toString(printer) + " * " + right->toString() + ")");
}

string FormulaOR::toString(NumPrinterBase* printer) const {
    return ("(" + left->toString(printer) + " + " + right->toString() + ")");
}

string FormulaNOT::toString(NumPrinterBase* printer) const {
    return ("~(" + f->toString(printer) + ")");
}

string FormulaLit::toString(NumPrinterBase* printer) const {
	assert(id2label.find(number) != id2label.end());
    return id2label[number];
}

string FormulaNUM::toString(NumPrinterBase* printer) const {
    return printer->printNum(number);
}

string FormulaTrue::toString(NumPrinterBase* printer) const {
    return ("true");
}

string FormulaFalse::toString(NumPrinterBase* printer) const {
    return ("false");
}

string FormulaFinal::toString(NumPrinterBase* printer) const {
    return ("final");
}

string FormulaDummy::toString(NumPrinterBase* printer) const {
    return f->toString(printer);
}
