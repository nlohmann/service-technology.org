/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
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
 * \date    \$Date: 2007/07/13 12:50:47 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.7 $
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
using namespace PNapi;

extern PetriNet PN;
int netnr = 0;  //net number for unique output filename

/*
Creates nets from input net.
All if activities in the output nets contain just one case branch.
*/
void split_net(PetriNet & P)
{
   extern int netnr;
   bool valid = false;
   std::map<unsigned int, unsigned int>::iterator it;
   for (it = globals::if_branches.begin();
       it != globals::if_branches.end();
       it++)
   {
      int count = 0;
      for (unsigned int i = 1; i < (*it).second; i++)
      {
         if (P.findTransition( toString((*it).first) + ".internal.case" + toString(i) ))
         {
            count++;
         }
      }
      if (P.findTransition( toString((*it).first) + ".internal.caseo" ))
      {
         count++;
      }
      if (count <= 1)
      {
         valid = true;
      }
      else
      {
         valid = false;
	 break;
      }
  }
  if (valid) 
  //all if activities consist of just one case branch, so print net
  {
     P.reduce(1);
     cerr << P.information() << endl;
     netnr++;
     // create oWFN output ?
     if (formats[F_OWFN])
     {
       if (globals::output_filename != "")
       {
         output = openOutput(globals::output_filename + toString(netnr) + "." + suffixes[F_OWFN]);
       }
       trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
       P.set_format(FORMAT_OWFN);
       (*output) << P;
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
         output = openOutput(globals::output_filename + toString(netnr) + "." + suffixes[F_DOT]);
       }
       trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
       // if parameter "nointerface" is set, set dot format with parameter "false"
       // not to draw an interface
       P.set_format(FORMAT_DOT, !globals::parameters[P_NOINTERFACE]);
       (*output) << P;
       if (globals::output_filename != "")
       {
         closeOutput(output);
         output = NULL;
      
#ifdef HAVE_DOT
         string systemcall = "dot -q -Tpng -o" + globals::output_filename + toString(netnr) + ".png " + globals::output_filename + toString(netnr) + "." + suffixes[F_DOT];
         trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
         trace(TRACE_INFORMATION, systemcall + "\n\n");
         system(systemcall.c_str());
#endif
       }
     }  
  }
  else
  //in P (*it).first contains several branches
  //for each branch create PNnew which contains just this branch
  {
     for (unsigned int i = 1; i <= (*it).second; i++)
     {
        PetriNet PNnew;
	PNnew = P;
        for (unsigned int j = 1; j <= (*it).second; j++)
	{
           if (i != j && j == (*it).second)
	       PNnew.removeTransition( PNnew.findTransition( toString((*it).first) + ".internal.caseo" ));
	   else 
              if (i != j)
	         PNnew.removeTransition( PNnew.findTransition( toString((*it).first) + ".internal.case" + toString(j) ));	
        }
        PNnew.reduce(1);
	split_net(PNnew);
     }
  }  
}



void data_extension_main()
{
  std::cerr << "------------------------------------------------------------------------------" << std::endl;

  std::cerr << "This process has " << globals::if_branches.size() << " <if> activities" << std::endl;
  
  
  
  // now the net will not change any more, thus the nodes are re-enumerated
  // and the maximal occurrences of the nodes are calculated.
  PN.reenumerate();
  
  //  PN.calculate_max_occurrences();
  cerr << PN.information() << endl;
  
  
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
      string systemcall = "dot -q -Tpng -o\"" + globals::output_filename + ".png\" \"" + globals::output_filename + "." + suffixes[F_DOT] + "\"";
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
  
  PetriNet PN1;
  PN1 = PN;  //using assignment operator since copy constructor is erroneous
  split_net(PN1);

}

