/*****************************************************************************\
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg,					 *
 * 						Christian Gierds, Dennis Reinert  					 *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/ 

/*!
 * \file    binDecision.h
 *
 * \brief   
 *
 * \author  
 *          - responsible: Dennis Reinert <reinert@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 *          - created:
 *          - last changed:
 * 
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version 
 *
 */

#ifndef BINDECISION_H
#define BINDECISION_H

using namespace std;

//class forward declartion 
class oWFN;
class State;

// binDecision records form a binary decision tree

class binDecision {
public:
	int bitnr;
	binDecision *  nextold;
	binDecision *  nextnew;
	unsigned char * vector; 	// actual bit vector
    binDecision * prev; 		// backlink to previous decision
    State * state;   			// link to state record for this state
	binDecision(int, long int);
	~binDecision();

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(binDecision)
#define new NEW_NEW
};

void binDelete(binDecision ** Bucket, long int BitVectorSize);
void binDeleteAll(binDecision * d);
void binDeleteAll(oWFN * PN, int h);
State * binInsert(oWFN * PN);
State * binInsert(binDecision ** Bucket, oWFN * PN);
State * binSearch(oWFN * PN);
State * binSearch(binDecision * Bucket, oWFN * PN);
void inttobits(unsigned char * bytepos, int bitpos, int nrbits, unsigned int value);
int logzwo(int m);

#endif //BINDECISION_H
