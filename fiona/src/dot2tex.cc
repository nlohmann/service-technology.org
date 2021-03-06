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
 * \file    dot2tex.cc 
 *
 * \brief   implementation of utility functions for dot to gastex conversion 
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 */

#include <string>
#include "dot2tex.h"


string texFormat(string str) {
    string::size_type pos;

    for(int i = 0; i < labelReplaceCount; i++) {
        pos = 0;
        while ((pos = str.find(labelReplace[i][0], 
            (pos == 0) ? 0 : pos + labelReplace[i][1].length())) != string::npos) {

            str.erase(pos, labelReplace[i][0].length());
            str.insert(pos, labelReplace[i][1]);
        }
    }
 
    return str;
}


