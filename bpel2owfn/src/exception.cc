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
 *          - last changed: \$Date: 2005/12/13 22:33:49 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.6 $
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
