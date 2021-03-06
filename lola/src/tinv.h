/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#ifndef TINV_H
#define TINV_H

#include "net.h"

#if defined(PREDUCTION) || defined(CYCLE) || defined(STRUCT) || defined(SWEEP)
class summand {
    public:
        Node* var;
        long int value;
        summand* next;

        summand(Node*, long int);
};

class equation {
    public:
        summand* sum;
        equation* next;

        void apply();
        void applyunit(); // applies equations where unit matrix is appended
        equation(Node*);
};

class invariant {
    public:
        summand* support;
};

void tsolve();
void psolve();

extern int ProgressSpan;
extern int ZeroProgress;
extern int MonotoneProgress;

void progress_measure();
#endif

#endif
