/*!
 * \file exception.cc
 *
 * \brief General exception class (implementation)
 *
 * This file implements the exception class defined in exception.h.
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/15
 *          - last changed: \$Date: 2005/12/04 14:16:07 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.5 $
 *          - 2005-11-15 (gierds) Initial version.
 *          - 2005-11-15 (nlohmann) Implemented #Exception::info().
 *
 */

#include "exception.h"

Exception::Exception(exception_id myid, std::string myerror)
{
  id = myid;
  error = myerror;
  information = "";
}





Exception::Exception(exception_id myid, std::string myerror, std::string myinformation)
{
  id = myid;
  error = myerror;
  information = myinformation;
}





void Exception::info()
{
  cerr << "Exception #" << id << " occured!" << endl;
  cerr << "  " << error << flush;

  if (information != "")
    cerr << "  Additional information: " << information << endl << flush;
}
