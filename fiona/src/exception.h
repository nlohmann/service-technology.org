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
 * \file exception.h
 *
 * \brief General exception class (declaration)
 *
 * This file declares the exception class.
 * 
 * \author  
 *          - last changes of: \$Author: massuthe $
 *          
 * \date
 *          - last changed: \$Date: 2006-10-07 18:11:04 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.4 $
 */





#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "mynew.h"
#include <string>
#include <iostream>

using namespace std;



/// Enumeration holding the error-codes of the possible exceptions.
/// \todo Overwork comments here and when throwing exceptions
typedef enum
{
  FILE_NOT_FOUND = 2,	         ///< "File not found" exception
  FILE_NOT_OPEN,		 ///< "File could not be opened" exception

  OPTION_MISMATCH = 10,
  
  CHECK_SYMBOLS_CAST_ERROR = 30, ///< "Dynamic cast error" exception (check-symbols.cc)

  NODE_NOT_FOUND = 40,           ///< "Node not found" exception (petrinet.cc)
  DOUBLE_NODE,                   ///< "Node already seen exception (petrinet.cc)
  MERGING_ERROR,                 ///< "Merging-error" exception (petrinet.cc)
  ARC_ERROR                      ///< "Arc between" exception (petrinet.cc)  
} exception_id;





/**
 * \class	Exception
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	The exception class
 * 
 * Exception is a class to be thrown as exception in order to provide
 * a detailed feedback about the error that has occured.
 *
 */
class Exception
{
  public:
    /// Constructor with mandatory parameters myid and myerror.
    Exception(exception_id myid, std::string myerror);

    /// Constructor with mandatory parameters myid, myerror and myinformation.
    Exception(exception_id myid, std::string myerror, std::string myinformation);

    /// Prints exception message.
    void info();
    
    /// error number
    exception_id id;


  private:
    /// error message
    std::string error;

    /// additional information
    std::string information;    
};

#endif
