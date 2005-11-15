/*!
 * \file exception.cc
 *
 * \brief General exception class (implementation)
 *
 * This file implements the exception class defined in exception.h.
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

#include "exception.h"

Exception::Exception(int myid, std::string myerror)
{
  id = myid;
  error = myerror;
  information = "";
}

