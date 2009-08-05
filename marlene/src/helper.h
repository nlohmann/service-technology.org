/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <string>
#include "cmdline.h"
#include "adapter.h"

// some initial values, which are the default values
extern Adapter::ControllerType contType;
extern unsigned int messageBound;
extern bool useCompPlaces;
extern bool veryverbose;
extern unsigned int veryverboseindent;

extern gengetopt_args_info args_info;
extern void abort(unsigned int code, const char* format, ...);
extern void evaluate_command_line(int argc, char* argv[]);

extern void status(const char* format, ...);

extern std::string toString(int i);

#endif
