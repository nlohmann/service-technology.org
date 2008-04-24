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
    color = "";
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
    labelfontsize = 10;
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

    set<BoundingBox*>::iterator iBB;
    for(iBB = boundingBoxes.begin(); iBB != boundingBoxes.end(); iBB++) {
        delete (*iBB);
    }
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
    trace(TRACE_5, "GasTexGraph::makeGasTex(string)::begin()\n");

    fstream texFile(texFileName.c_str(), ios_base::out | ios_base::trunc);
    if (!texFile) {
        cerr << "cannot write to file " << texFileName << endl;
        exit(4);
    }

    for(int i = 0; i < texHeaderCount; i++) {
        texFile << texHeader[i];
    }

    if (gastexGraph) {
        if (parameters[P_OG] || parameters[P_IG]) {
            makeGasTexOfOG(texFile);
        } else {
            makeGasTexOfPN(texFile);
        } 
    }

    for (int i = 0; i < texFooterCount; i++) {
        texFile << texFooter[i];
    }

    trace(TRACE_5, "GasTexGraph::makeGasTex()::end\n");
}

void GasTexGraph::makeGasTexOfOG(fstream& texFile) {

    // drawing nodes

    if (ogNodeStdStmt != "") {
        texFile << ogNodeStdStmt << endl;
    }

    for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
        iNode != gastexGraph->nodes.end(); ++iNode) {

        texFile << ogNodeStmt[0];
        texFile << ogNodeStmt[1] << 5;

        if((*iNode)->isInitial) {
            texFile << ogNodeStmt[2] << "i";
            if((*iNode)->isFinal) {
                texFile << "r";
            }
        } else if ((*iNode)->isFinal) {
            texFile << ogNodeStmt[2] << "r";
        }

        if ((*iNode)->isInitial || (*iNode)->isFinal) {
            texFile << ogNodeStmt[3] << ((*iNode)->width * charWidthRatio + .5);
            texFile << ogNodeStmt[4] << ((*iNode)->height * charHeightRatio + .5);
        } else {
            texFile << ogNodeStmt[3] << ((*iNode)->width * charWidthRatio);
            texFile << ogNodeStmt[4] << ((*iNode)->height * charHeightRatio);
        }

        if ((*iNode)->color != "") {
            texFile << ogNodeStmt[5] << (char)toupper((*iNode)->color.at(0));
            texFile << (*iNode)->color.substr(1, (*iNode)->color.length());
        }

        if ((*iNode)->fillcolor != "") {
            texFile << ogNodeStmt[6] << (char)toupper((*iNode)->fillcolor.at(0));
            texFile << (*iNode)->fillcolor.substr(1, (*iNode)->fillcolor.length());
        }

        texFile << ogNodeStmt[7] << (*iNode)->id;
        texFile << ogNodeStmt[8] << (int) ((*iNode)->posX * scaleFactor);
        texFile << ogNodeStmt[9] << (int) ((*iNode)->posY * scaleFactor);
        texFile << ogNodeStmt[10];
        if ((*iNode)->label != "") {
            texFile << ogNodeStmt[11] << texFormat((*iNode)->label);
        }
        texFile << ogNodeStmt[12] << endl;
    }

    texFile << endl;

    // drawing edges

    if (ogEdgeStdStmt != "") {
        texFile << ogEdgeStdStmt << endl;
    }

    for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
        iNode != gastexGraph->nodes.end(); ++iNode) {
        for(set<GasTexEdge*>::iterator iEdge = (*iNode)->edges.begin();
            iEdge != (*iNode)->edges.end(); iEdge++) {

            texFile << ogEdgeStmt[0];

            if ((*iNode)->color != "") {
                texFile << ogEdgeStmt[1] << (char)toupper((*iEdge)->color.at(0));
                texFile << (*iEdge)->color.substr(1, (*iEdge)->color.length());
                texFile << ogEdgeStmt[2];
            }

            texFile << ogEdgeStmt[3] << (*iEdge)->srcNode->id;
            texFile << ogEdgeStmt[4] << (*iEdge)->destNode->id;
            texFile << ogEdgeStmt[5];

            if ((*iEdge)->label != "") {
                texFile << texFormat((*iEdge)->label);
            }
            texFile << ogEdgeStmt[6] << endl;
        }
    }
}


void GasTexGraph::makeGasTexOfPN(fstream& texFile) {

    /*
    // drawing bounding boxes
    for(set<BoundingBox*>::iterator iBB = gastexGraph->boundingBoxes.begin()++; 
        iBB != gastexGraph->boundingBoxes.end(); ++iBB) {
        texFile << box_stmt_str[0] << (int)((*iBB)->x * scale_factor); 
        texFile << box_stmt_str[1] << (int)((*iBB)->y * scale_factor); 
        texFile << box_stmt_str[2] << (int)((*iBB)->width * scale_factor); 
        texFile << box_stmt_str[3] << (int)((*iBB)->height * scale_factor); 
        texFile << box_stmt_str[4] << endl; 
    }
    */

    // drawing nodes
    if (pnNodeStdStmt != "") {
        texFile << pnNodeStdStmt << endl;
    }
    for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
        iNode != gastexGraph->nodes.end(); ++iNode) {

        if((*iNode)->isInitial) {
            texFile << pnInitialNodeStmt[0];
            texFile << (int) ((*iNode)->posX * scaleFactor);
            texFile << pnInitialNodeStmt[1];
            texFile << (int) ((*iNode)->posY * scaleFactor);
            texFile << pnInitialNodeStmt[2];
            texFile << endl;
        }

        texFile << pnNodeStmt[0];
        if ((*iNode)->shape == "box") {
            texFile << pnNodeStmt[1] << "0";
        } else if ((*iNode)->shape == "circle") {
            texFile << pnNodeStmt[1] << "15";
        }

        if ((*iNode)->isFinal) {
            texFile << pnNodeStmt[2];
        }

        if ((*iNode)->style == "invis") {
            texFile << pnNodeStmt[3]; 
        }

        texFile << pnNodeStmt[4] << (*iNode)->id;
        texFile << pnNodeStmt[5] << (int) ((*iNode)->posX * scaleFactor);
        texFile << pnNodeStmt[6] << (int) ((*iNode)->posY * scaleFactor);
        texFile << pnNodeStmt[7];

        if ((*iNode)->label != "") {
            texFile << texFormat((*iNode)->label);
        }

/* else {
            string label = "";
            for (set<GasTexNode*>::iterator jNode = gastexGraph->nodes.begin();
                 jNode != gastexGraph->nodes.end(); ++jNode) {
                for (set<GasTexEdge*>::iterator jEdge = (*jNode)->edges.begin();
                     jEdge != (*jNode)->edges.end(); ++jEdge) {
                    if (((*jEdge)->srcNode == (*iNode)) && ((*jEdge)->taillabel != "")) {
                        label = (*jEdge)->taillabel;
                    }
                    if (((*jEdge)->destNode == (*iNode)) && ((*jEdge)->headlabel != "")) {
                        label = (*jEdge)->headlabel;
                    }
                }
            }
            if (label != "") {
                texFile << texFormat(label);
            }
        } 
*/
        texFile << pnNodeStmt[8] << endl;
    }


    texFile << endl;

    // drawing edges 
    if (pnEdgeStdStmt != "") {
        texFile << pnEdgeStdStmt;
    }
    for(set<GasTexNode*>::iterator iNode = gastexGraph->nodes.begin();
        iNode != gastexGraph->nodes.end(); ++iNode) {
        for(set<GasTexEdge*>::iterator iEdge = (*iNode)->edges.begin();
            iEdge != (*iNode)->edges.end(); iEdge++) {

            texFile << pnEdgeStmt[0]; 
            if ((*iEdge)->srcNode->style == "invis" || (*iEdge)->destNode->style == "invis") {
                texFile << pnEdgeStmt[1];
                if ((*iEdge)->headlabel != "") {
                    texFile << pnEdgeStmt[2];
                } else {
                    texFile << pnEdgeStmt[3];
                } 
            }
            texFile << pnEdgeStmt[4] << (*iEdge)->srcNode->id; 
            texFile << pnEdgeStmt[5] << (*iEdge)->destNode->id;
            texFile << pnEdgeStmt[6]; 

            if ((*iEdge)->label != "") {
                texFile << texFormat((*iEdge)->label); 
            } else if ((*iEdge)->headlabel != "") {
                texFile << texFormat((*iEdge)->headlabel); 
            } else if ((*iEdge)->taillabel != "") {
                texFile << texFormat((*iEdge)->taillabel); 
            }

            texFile << pnEdgeStmt[7] << endl; 
        }
    }
}

