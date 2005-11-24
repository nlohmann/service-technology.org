/*!
 * \file debug.h
 *
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2005/11/24 10:40:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.8 $
 *          - 2005-11-15 (gierds) Trace level as enum.
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <iostream>
#include <fstream>
#include <exception>

#include "helpers.h"


typedef enum
{
  TRACE_ALWAYS,		/// trace level for errors
  TRACE_WARNINGS,       /// trace level for warnings 
  TRACE_INFORMATION,	/// trace level some more (useful) information
  TRACE_DEBUG,          /// trace level for detailed debug information
  TRACE_VERY_DEBUG	/// trace level for everything (? information collapse)
} trace_level;


void trace(trace_level pTraceLevel, std::string message);
void trace(std::string message);

/// Signalling syntax errors.
int yyerror(const char* msg);

#endif
