/*****************************************************************************
 * Copyright 2005, 2006, 2007, 2008 Peter Massuthe, Robert Danitz            *
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
 * \file    GastexGraph.cc
 *
 * \brief   Implementation of class GasTexGraph. See GastexGraph.h for further
 *          information.
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "GastexGraph.h"
#include "dot2tex.h"
#include "debug.h"

using namespace std;

extern GasTexGraph* gastexGraph;


/*** GasTexNode ***/

GasTexNode::GasTexNode() {
    setToDefault();
}


GasTexNode::~GasTexNode() {
    set<GasTexEdge*>::iterator iEdge;
    for(iEdge = edges.begin(); iEdge !=  edges.end(); iEdge++) {
        delete (*iEdge);
    }
}


void GasTexNode::addEdge(GasTexNode* dest) {
    if (!dest) return;

    GasTexEdge* edge = new GasTexEdge(this, dest);
    edges.insert(edge);
}


void GasTexNode::addEdge(GasTexEdge* edge) {
    if (!edge) return;

    edges.insert(edge);
}


void GasTexNode::setToDefault() {
    id = "";
    label = "";
    isInitial = false;
    isFinal = false;
    posX = posY = 0;
    width = height = 1.0;
    color = "black";
    fillcolor = "";
    fontcolor = "";
    fontsize = 10;
    shape = "";
    style = "";
}


/*** GasTexEdge ***/

GasTexEdge::GasTexEdge(GasTexNode* src, GasTexNode* dest) {
    srcNode = src;
    destNode = dest;

    setToDefault();
}


GasTexEdge::~GasTexEdge() {
}


void GasTexEdge::setToDefault() {
    label = "";
    labelfontcolor = "";
    labelfontname = "";
    labelfontsize = 12;
    color = "";
    fontcolor = "";
    fontname = "";
    fontsize = 10;
    headlabel = "";
    headport = "";
    taillabel = "";
    tailport = "";
    style = "";
}


/*** GasTexGraph ***/

GasTexGraph::GasTexGraph() {
    root = NULL;
}


GasTexGraph::~GasTexGraph() {
    set<GasTexNode*>::iterator iNode;
    for(iNode = nodes.begin(); iNode != nodes.end(); iNode++) {
        delete (*iNode);
    }
    root = NULL;
}


void GasTexGraph::addNode(GasTexNode* node) {
    if (!node) return;

    nodes.insert(node);
}


void GasTexGraph::addEdge(GasTexEdge* edge) {
    if (!edge) return;

    edge->srcNode->addEdge(edge);
}


GasTexNode* GasTexGraph::getNode(string id) {
    GasTexNode* where = 0;
    for(set<GasTexNode*>::iterator iNode = nodes.begin(); iNode != nodes.end(); iNode++) {
        if (id == (*iNode)->id) {
            where = (*iNode);
        }
    }
    return where;
}


void GasTexGraph::makeGasTex(string texFileName) {
    trace(TRACE_5, "gastexGraph::makeGasTex(string)::begin()\n");

    fstream texFile(texFileName.c_str(), ios_base::out | ios_base::trunc);
    if (!texFile) {
        cerr << "cannot write to file " << texFileName << endl;
        exit(4);
    }

    for(int i = 0; i < texHeaderCount; i++) {
        texFile << texHeader[i];
    }

    if (gastexGraph) {
        // nodes
        for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
            iNode != gastexGraph->nodes.end(); iNode++) {

            if ((*iNode)->style != "invis") {
                texFile << node_stmt_str[0];
                if((*iNode)->isInitial) {
                    texFile << node_stmt_str[1] << "i";
                    if((*iNode)->isFinal) {
                        texFile << "r";
                    }
                    texFile << ", ";
                } else if ((*iNode)->isFinal) {
                    texFile << node_stmt_str[1] << "r";
                    texFile << ", ";
                }

                if ((*iNode)->isInitial || (*iNode)->isFinal) {
                    texFile << node_stmt_str[2] << ((*iNode)->width * char_width_ratio + .5);
                    if ((*iNode)->shape != "circle" && (*iNode)->shape != "box") {
                        texFile << node_stmt_str[3] << ((*iNode)->height * char_height_ratio + .5);
                    } else {
                        texFile << node_stmt_str[3] << ((*iNode)->width * char_width_ratio + .5);
                    }
                } else {
                    texFile << node_stmt_str[2] << ((*iNode)->width * char_width_ratio);
                    if ((*iNode)->shape != "circle" && (*iNode)->shape != "box") {
                        texFile << node_stmt_str[3] << ((*iNode)->height * char_height_ratio);
                    } else {
                        texFile << node_stmt_str[3] << ((*iNode)->width * char_width_ratio);
                    }
                }

                if ((*iNode)->shape == "box") {
                    texFile << node_stmt_str[4] << "0";
                } else if ((*iNode)->shape == "ellipse") {
                    texFile << node_stmt_str[4] << "3";
                } else if ((*iNode)->shape == "circle") {
                    texFile << node_stmt_str[4] << "15";
                }

                if ((*iNode)->color != "") {
                    texFile << node_stmt_str[5] << (char)toupper((*iNode)->color.at(0));
                    texFile << (*iNode)->color.substr(1, (*iNode)->color.length());
                }

                if ((*iNode)->fillcolor != "") {
                    texFile << node_stmt_str[6] << (char)toupper((*iNode)->fillcolor.at(0));
                    texFile << (*iNode)->fillcolor.substr(1, (*iNode)->fillcolor.length());
                }

                texFile << node_stmt_str[7] << (*iNode)->id;
                texFile << node_stmt_str[8] << (int) ((*iNode)->posX * scale_factor);
                texFile << node_stmt_str[9] << (int) ((*iNode)->posY * scale_factor);
                if ((*iNode)->label != "") {
                    texFile << node_stmt_str[10] << texFormat((*iNode)->label);
                } else {
                    texFile << node_stmt_str[10] << texFormat((*iNode)->id);
                }
                texFile << node_stmt_str[11] << endl;
            }
        }

        texFile << endl;

        // edges 
        for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
            iNode != gastexGraph->nodes.end(); iNode++) {
            for(set<GasTexEdge*>::iterator iEdge = (*iNode)->edges.begin();
                iEdge != (*iNode)->edges.end(); iEdge++) {

                if ((*iEdge)->srcNode->style != "invis" && (*iEdge)->destNode->style != "invis") {
                    texFile << edge_stmt_str[0] << (*iEdge)->srcNode->id; 
                    texFile << edge_stmt_str[1] << (*iEdge)->destNode->id; 
                    texFile << edge_stmt_str[2] << texFormat((*iEdge)->label); 
                    texFile << edge_stmt_str[3] << endl; 
                }
            }
        }
    }

    for (int i = 0; i < texFooterCount; i++) {
        texFile << texFooter[i];
    }

    trace(TRACE_5, "gastexGraph::report()::end\n");
}

