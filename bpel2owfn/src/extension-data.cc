/*****************************************************************************\
 * Copyright 2007  Thomas Heidinger, Niels Lohmann                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    extension-data.cc
 *
 * \author  Thomas Heidinger <heidinge@informatik.hu-berlin.de>,
 *          Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 * 
 * \since   2007-06-07
 *
 * \date    \$Date: 2007/06/20 09:24:22 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.3 $
 */





/******************************************************************************
* Headers
*****************************************************************************/

#include <iostream>
#include <map>

#include "ast-details.h"
#include "extension-data.h"
#include "globals.h"
#include "petrinet.h"
#include "options.h"
#include "bpel2owfn.h"

using std::cerr;
using std::endl;

extern PetriNet PN;




void data_extension_main()
{
  std::cerr << "------------------------------------------------------------------------------" << std::endl;

  std::cerr << "This process has " << globals::if_branches.size() << " <if> activities" << std::endl;
  
  
  
  // now the net will not change any more, thus the nodes are re-enumerated
  // and the maximal occurrences of the nodes are calculated.
  PN.reenumerate();
  
  //  PN.calculate_max_occurrences();
  cerr << PN.information() << endl;
  
  
  PN.removeTransition( PN.findTransition( "5.internal.case2" ) );
  PN.removeTransition( PN.findTransition( "5.internal.caseo" ) );
  
  
  PN.reduce(1);
  

  //  PN.calculate_max_occurrences();
  cerr << PN.information() << endl;

  
  // create oWFN output ?
  if (formats[F_OWFN])
  {
    if (globals::output_filename != "")
    {
      output = openOutput(globals::output_filename + "." + suffixes[F_OWFN]);
    }
    trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
    PN.set_format(FORMAT_OWFN);
    (*output) << PN;
    if (globals::output_filename != "")
    {
      closeOutput(output);
      output = NULL;
    }
  }
  
  // create dot output ?
  if ( formats[F_DOT] )
  {
    if (globals::output_filename != "")
    {
      output = openOutput(globals::output_filename + "." + suffixes[F_DOT]);
    }
    trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
    // if parameter "nointerface" is set, set dot format with parameter "false"
    // not to draw an interface
    PN.set_format(FORMAT_DOT, !globals::parameters[P_NOINTERFACE]);
    (*output) << PN;
    if (globals::output_filename != "")
    {
      closeOutput(output);
      output = NULL;
      
#ifdef HAVE_DOT
      string systemcall = "dot -q -Tpng -o" + globals::output_filename + ".png " + globals::output_filename + "." + suffixes[F_DOT];
      trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
      trace(TRACE_INFORMATION, systemcall + "\n\n");
      system(systemcall.c_str());
#endif
    }
  }  
  
  for (std::map<unsigned int, unsigned int>::iterator it = globals::if_branches.begin();
       it != globals::if_branches.end();
       it++)
  {
    std::cerr << "there is an <if> with id " << (*it).first << " and with " << (*it).second << " branches" << std::endl;
  }
  
  std::cerr << globals::if_branches[5] << std::endl;
}
