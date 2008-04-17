/*****************************************************************************
 * Copyright 2005, 2006, 2007, 2008                                          *
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
 * \file    GastexGraph.h
 *
 * \brief   Holds the graph structure of OGs etc.. with purpose of using with 
 *          latex (gastex package)
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GASTEXGRAPH_H_
#define GASTEXGRAPH_H_

#include <string>
#include <set>

using namespace std;

class GasTexNode;
class GasTexEdge;
class GasTexGraph;

class GasTexNode {

public:
    string id;
    string label;
    bool isInitial;
    bool isFinal;
    int posX, posY;
    float width, height;
    string color;
    string fillcolor;
    string fontcolor;
    string fontname;
    int fontsize;
    string shape;
    string style;

    set<GasTexEdge*> edges;

    GasTexNode();
    ~GasTexNode();

    void addEdge(GasTexNode* dest);
    void addEdge(GasTexEdge* edge);
    void setToDefault();
};


class GasTexEdge {

public:
    string label;
    string labelfontcolor;
    string labelfontname;
    int labelfontsize;
    string color;
    string fontcolor;
    string fontname;
    int fontsize;
    string headlabel;
    string headport;
    string taillabel;
    string tailport;
    string style;

    GasTexNode* srcNode;
    GasTexNode* destNode;

    GasTexEdge(GasTexNode* src = 0, GasTexNode* dest = 0);
    ~GasTexEdge();

    void setToDefault();
};


class GasTexGraph {

public:
    string label;
    int posx, posy, width, height;
    string fontname;
    int fontsize;
    string fontcolor; 
    int label_posx, label_posy;

    GasTexNode* root;
    set<GasTexNode*> nodes; 

    GasTexGraph();
    ~GasTexGraph();

    void addNode(GasTexNode* node);
    void addEdge(GasTexEdge* edge);
    GasTexNode* getNode(string id);
    void makeGasTex(string texFileName);
};

#endif
