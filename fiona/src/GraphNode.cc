/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
 * \file    GraphNode.cc
 *
 * \brief   functions for handling of nodes of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "GraphNode.h"
#include "GraphFormula.h"
#include "state.h"
#include "debug.h"
#include "options.h"
#include <cassert>


using namespace std;


/************************
 * class GraphNodeColor *
 ************************/

GraphNodeColor::GraphNodeColor(GraphNodeColor_enum color) :
    color_(color) {

}

std::string GraphNodeColor::toString() const {
    switch (color_) {
        case BLUE:
            return "blue";
        case RED:
            return "red";

        default:
            assert(false);
            return "undefined color";
    }
}


GraphNodeColor::operator GraphNodeColor_enum() const {
    return color_;
}


/*********************************
 * class GraphNodeDiagnosisColor *
 *********************************/

GraphNodeDiagnosisColor::GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color) :
    diagnosis_color_(color) {

}


std::string GraphNodeDiagnosisColor::toString() const {
    switch (diagnosis_color_) {
        case DIAG_UNSET:
            return "darkgreen";
        case DIAG_RED:
            return "red";
        case DIAG_BLUE:
            return "blue";
        case DIAG_ORANGE:
            return "darkorange";
        case DIAG_VIOLET:
            return "violetred";

        default:
            assert(false);
            return "undefined color";
    }
}


GraphNodeDiagnosisColor::operator GraphNodeDiagnosisColor_enum() const {
    return diagnosis_color_;
}


/*******************
 * class GraphNode *
 *******************/


////! \brief constructor
//GraphNode::GraphNode() :
//    // initialize father
//    GraphNodeCommon<>() {
//
//    eventsUsed = new int [PN->getInputPlaceCount() + PN->getOutputPlaceCount()];
//
//    for (unsigned int i = 0;
//         i < PN->getInputPlaceCount() + PN->getOutputPlaceCount(); i++) {
//        eventsUsed[i] = 0;
//    }
//}
//
//    
////! \brief destructor
//GraphNode::~GraphNode() {
//    trace(TRACE_5, "GraphNode::~GraphNode() : start\n");
//
//    if (eventsUsed != NULL) {
//        delete[] eventsUsed;
//    }
//
//    numberDeletedVertices++;
//    trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
//}
//
//



