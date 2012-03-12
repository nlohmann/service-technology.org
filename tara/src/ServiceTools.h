/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef SERVICE_TOOLS_H
#define SERVICE_TOOLS_H

#include <pnapi/pnapi.h>
#include <string>
#include "Tara.h"

bool isControllable(pnapi::PetriNet &net, bool useWendyOptimization=false);
void getLolaStatespace(pnapi::PetriNet &net, const std::string &tempFile);
void computeOG(pnapi::PetriNet &net, std::string outputFile);
void computeMP(pnapi::PetriNet &net, std::string outputFile);

#endif
