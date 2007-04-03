/*****************************************************************************
 * Copyright 2007 Jan Bretschneider                                          *
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
 * \file	enums.cc
 *
 * \brief	Implementation for some classes acting as enums.
 * 
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 */

#include "enums.h"
#include <cassert>

using namespace std;

vertexColor::vertexColor() : color_(RED)
{
}

vertexColor::vertexColor(vertexColor_enum color) : color_(color)
{
}

std::string vertexColor::toString() const
{
    switch (color_)
    {
        case BLUE: return "blue";
        case RED:  return "red";
    }

    // control should never reach this line.
    assert(true);
    return "undefined color";
}

vertexColor::operator vertexColor_enum() const
{
    return color_;
}
