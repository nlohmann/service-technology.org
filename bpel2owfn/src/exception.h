#ifndef EXCEPTION_H
#define EXCEPTION_H

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
 *          - last changed: \$Date: 2005/11/15 15:23:03 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.1 $
 *          - 2005-11-11 (gierds) Initial version.
 *
 */

#include <string>

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
    /// error number
    int id;
    /// error message
    std::string error;
    /// additional information
    std::string information;

    /// Constructur with mandatory parameters #id and #error
    Exception(int myid, std::string myerror);
    
};

#endif
