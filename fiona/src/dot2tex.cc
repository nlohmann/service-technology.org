/*****************************************************************************
 * Copyright 2007, 2008                                                      *
 *   Peter Massuthe, Robert Danitz                                           *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    dot2tex.cc 
 *
 * \brief   implementation of utility functions for dot to gastex conversion 
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
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


