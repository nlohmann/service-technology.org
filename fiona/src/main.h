/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    main.h
 *
 * \brief   main
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "owfn.h"

extern oWFN* PN;
extern unsigned int numberOfDecodes;
extern unsigned int NonEmptyHash;
extern unsigned int numberDeletedVertices;
extern string texBuffer[];

extern unsigned short int globalExitCode;

// Sets the exit code. If the new exit code is >= exitAtThisCode,
// exit(newExitCode) is called.
void setExitCode(unsigned short int newExitCode);

// Retrieve the exit code
unsigned short int getExitCode();

///Equivalent for basename() on Posix systems. See 'man 3 basename'.
extern std::string platform_basename(const std::string& path);

#endif /*MAIN_H*/
