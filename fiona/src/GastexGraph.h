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
 * \file    GastexGraph.h
 *
 * \brief   Holds the graph structure of OGs etc.. with purpose of using with 
 *          latex (gastex package)
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 */

#ifndef GASTEXGRAPH_H_
#define GASTEXGRAPH_H_

#include <string>
#include <set>
#include <cstdlib>

#include "options.h"

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
    unsigned int peripheries;

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

struct BoundingBox {
    int x;
    int y;
    int width;
    int height;
};

class GasTexGraph {

public:
    typedef enum {
        STYLE_OWFN,
        STYLE_OG
    } STYLE; 

    STYLE style;
    string label;
    int posx, posy, width, height;
    string fontname;
    int fontsize;
    string fontcolor; 
    int label_posx, label_posy;
    set<BoundingBox*> boundingBoxes;

    GasTexNode* root;
    set<GasTexNode*> nodes; 

    GasTexGraph();
    ~GasTexGraph();

    void setStyle(STYLE style);
    void addNode(GasTexNode* node);
    void addEdge(GasTexEdge* edge);
    GasTexNode* getNode(string id);
    void makeGasTex(string texFileName);
    void makeGasTexOfOWFN(fstream& texFile);
    void makeGasTexOfOG(fstream& texFile);
};

#endif
