/*!
 * \file debug.h
 *
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2005/12/04 12:51:06 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.10 $
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

/// Signalling syntax errors.
int yyerror(const char* msg);

#endif
