/*****************************************************************************\
 Locretia -- generating logs...

 Copyright (c) 2012 Simon Heiden

 Locretia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Locretia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Locretia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <pnapi/pnapi.h>
#include <vector>
#include <map>
#include <set>


/*!
 \brief open net

 Contains all open net related functions and attributes.
 Is used if the input is either an OWFN or a TPN.
*/
class openNet {

    public: /* static functions */

		static void initialize();

		/// change the viewpoint to the environment
        static void changeView(pnapi::PetriNet* net, const int b);

    public: /* static attributes */
        /// the open net
        static pnapi::PetriNet* net;

};


#define PORT_NAME "port"
