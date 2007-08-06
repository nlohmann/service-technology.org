/*****************************************************************************
 * Copyright 2005, 2006 Kathrin Kaschner                                     *
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
 * \file    BddRepresentation.h
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef BDDREPRESENTATION_H_
#define BDDREPRESENTATION_H_
 
#include "mynew.h" 
#include <vector>
#include <map>
#include <list>

#include "GraphNode.h"

#include "util.h"
#include "cudd.h"
#include "cuddInt.h"
#include "dddmp.h"

using namespace std;
typedef vector<bool> BitVector;

class BddLabelTab;

class BddRepresentation{
	public:
		BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT, unsigned int cntNodes = 1, bool calcStates = false);
		~BddRepresentation();
		
		void convertRootNode(GraphNode* root);
		void generateRepresentation(GraphNode* v, std::map<GraphNode*, bool>&);
		void addOrDeleteLeavingEdges(GraphNode* v);
		void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
		void printDotFile(char** names = NULL, char* option = "OG");
		void print();
		void save(char* option = "OG");
		void printMemoryInUse();
		
		void testSymbRepresentation(GraphNode* v, std::map<GraphNode*, bool>&);
		unsigned int getBound();
		void setMaxPlaceBits(GraphNode* v, std::map<GraphNode*, bool>&);
		
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(BddRepresentation)
#define new NEW_NEW

	private:	  
		DdManager* mgrMp;
		DdManager* mgrAnn;
		DdNode* bddMp;
		DdNode* bddRed;
		DdNode* bddAnn;
		int maxNodeBits;
		int maxLabelBits;
		unsigned int maxNodeNumber;
		int nbrLabels;
		map<unsigned int, unsigned int> nodeMap;
		BddLabelTab * labelTable;
		
		/**
		 * Returns the BDD of a label (given as integer).
		 */
		DdNode* labelToBddMp(const std::string& label);

		DdNode* nodesToBddMp(unsigned int node1, unsigned int node2);
		DdNode* annotationToBddAnn(GraphNode * v);
		DdNode* clauseToBddAnn(const GraphFormulaMultiaryOr* cl);
		unsigned int getBddNumber(unsigned int node);
		void addBddVars(unsigned int max);	
		BitVector numberToBin(unsigned int number, int cntBits);
		int nbrBits(unsigned int i);
		string myitoa(unsigned int value, int base);
		void checkManager(DdManager* mgr, char* table);
		
		DdNode* statesToBddMp(GraphNode* v);
		DdNode* markingToBddMp(unsigned int * marking);
		unsigned int bound;
		void calculateBound(GraphNode* v, std::map<GraphNode*, bool>&);
		int maxPlaceBits;
};

#endif /*BDDREPRESENTATION_H_*/
