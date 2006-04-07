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
 * \file debug.h
 *
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - last changes of: \$Author: weinberg $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2006-04-07 06:53:46 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.1 $
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <iostream>
#include <fstream>
#include <exception>

typedef enum
{
  TRACE_0,			//!< no messages
  TRACE_1,			//!< keep track of nodes
  TRACE_2,       	//!< edges
  TRACE_3,			//!< number of states: global, local
  TRACE_4,          //!< to be defined
  TRACE_5			//!< all
} trace_level;

extern trace_level debug_level;

/**
 * Provides output to stderr using different #trace_level 
 * (in order to regulate amount of output)
 *
 * \param pTraceLevel	the #trace_level
 * \param message	the output
 *
 */
void trace(trace_level pTraceLevel, std::string message);

/**
 * Works like #trace(trace_level,std::string) with trace_level = TRACE_ALWAYS
 *
 * \param message the output
 *
 */
void trace(std::string message);

std::string intToString(int);

/// Signalling syntax errors.
int yyerror(const char* msg);


#endif


