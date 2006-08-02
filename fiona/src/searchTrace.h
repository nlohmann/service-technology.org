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
 * \file    searchTrace.h
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

#ifndef SEARCHTRACE_H
#define SEARCHTRACE_H

using namespace std;

class SearchTrace {
public:
	unsigned char bn_byte;	// byte to be matched against tree vector; constructed from MARKINGVECTOR
	int bn_t;				// index in tree vector
	unsigned char * bn_v;	// current tree vector
	int bn_s;				// nr of bits pending in byte from previous iteration
	int bn_dir;				// did we go "old" or "new" in last decision?
	int bn_b;				// bit nr at start of byte
	binDecision * frmdec, * tdec;
	binDecision * vectrdec;
};


extern SearchTrace * Trace;

#endif
