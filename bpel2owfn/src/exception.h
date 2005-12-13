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
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/15
 *          - last changed: \$Date: 2005/12/13 22:33:49 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.10 $
 *          - 2005-11-15 (gierds) Initial version.
 *          - 2005-11-15 (nlohmann) Overworked class and moved error output
 *            into class.
 *
 */





#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <iostream>

using namespace std;



/// Enumeration holding the error-codes of the possible exceptions.
typedef enum
{
  FILE_NOT_FOUND = 2,	    ///< "File not found" exception
  NODE_NOT_FOUND,           ///< "Node not found" exception (petrinet.cc)
  MERGING_ERROR,            ///< "Merging-error" exception (petrinet.cc)
  ARC_ERROR,                ///< "Arc between" exception (petrinet.cc)
  CHECK_SYMBOLS_CAST_ERROR, ///< "Dynamic cast error" exception (check-symbols.cc)
  DOUBLE_PARTNERLINK,       ///< "Two PartnerLinks with same name" exception (check-symbols.cc)	  
  DOUBLE_VARIABLE,          ///< "Two Variables with same name" exception (check-symbols.cc)	  
  PARTNERLINK_UNDEFINED     ///< "PartnerLink does not exist" exception (check-symbols.cc)
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
    /// Constructor with mandatory parameters #myid and #myerror.
    Exception(exception_id myid, std::string myerror);

    /// Constructor with mandatory parameters #myid, #myerror and #myinformation.
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
