/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file    bpel-unparse-tools.h
 *
 * \brief   unparse helper tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2006/10/25 06:53:37 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.16 $
 *
 * \ingroup debug
 * \ingroup creation
 */





#ifndef BPEL_UNPARSE_TOOLS_H
#define BPEL_UNPARSE_TOOLS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>

#include "petrinet.h"
#include "debug.h"

using namespace std;





/******************************************************************************
 * Functions for the Petri net unparser defined in bpel-unparse-petri.k
 *****************************************************************************/

Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix,
    int negativeControlFlow, bool preventFurtherFaults = true);
Transition *stop(Place *p, string p_name, string prefix);
void dpeLinks(Transition* t, int id);
string inString();
void header(kc::integer id, bool indent = false);
void header(int id, bool indent = false);
void footer(kc::integer id, bool indent = false);
void footer(int id, bool indent = false);





/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

void in();
void inup();
void indown();





#endif


/*!
 * \defgroup creation Petri Net Creation Module
 */
