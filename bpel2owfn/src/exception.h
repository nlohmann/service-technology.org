/*!
 * \file exception.h
 *
 * \brief General exception class (declaration)
 *
 * This file declares the exception class.
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/11/15
 *          - last changed: \$Date: 2005/11/24 10:40:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.7 $
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
