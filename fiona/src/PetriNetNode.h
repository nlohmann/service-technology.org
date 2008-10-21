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
 * \file    PetriNetNode.h
 *
 * \brief   functions for Petri net elements
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef PETRINETNODE_H_
#define PETRINETNODE_H_

#include "mynew.h"
#include <iostream>
#include <vector>

using namespace std;

class Arc;

class Node {
    // public typedefs first, then private members, then public methods.
    public:
    
        /// vector including all arcs connected to this node
        typedef std::vector<Arc*> Arcs_t;
        
    private:
    
        /// arriving arcs for this nod
        Arcs_t ArrivingArcs;
        
        /// leaving arcs for this node
        Arcs_t LeavingArcs;
        
    public:
    
        /// name of the node
        std::string name;
        
        /// constructor
        Node(const std::string&);
        
        /// operator for node stream output
        ostream& operator <<(ostream &);
        
        /// destructor
        ~Node();

        /// returns the name of the node
        std::string getName() const;
        
        /// adds an arriving arc to the node
        void addArrivingArc(Arc* arc);
    
        /// removes an arriving arc from the node
        void removeArrivingArc(const Arc* arc);
    
        /// returns the number of arriving arcs
        Arcs_t::size_type getArrivingArcsCount() const;
        
        /// returns an arriving arc at the given iterator position
        Arc* getArrivingArc(Arcs_t::size_type i) const;
        
        /// adds a leaving arc to the node
        void addLeavingArc(Arc* arc);

        /// removes a leaving arc from the node
        void removeLeavingArc(const Arc* arc);

        /// returns the number of leaving arcs
        Arcs_t::size_type getLeavingArcsCount() const;

        /// returns an leaving arc at the given iterator position
        Arc* getLeavingArc(Arcs_t::size_type i) const;
};


class owfnTransition;
class owfnPlace;


class Arc {
    public:
        
        /// constructor
        Arc(owfnTransition *, owfnPlace *, bool, unsigned int);

        ///	static unsigned int cnt;
        
        /// source node of the arc
        Node * Source;
        
        /// destination node of the arc
        Node * Destination;
        
        /// pointer at the place of the arc
        owfnPlace * pl;
        
        /// pointer at the transition of the arc
        owfnTransition * tr;
        
        /// multiplicity of the arc
        unsigned int Multiplicity;
        
        /// multiplicity addition operator for arcs
        void operator +=(unsigned int);

        /// Provides user defined operator new. Needed to trace all new
        /// operations on this class.
#undef new
        NEW_OPERATOR(Arc)
#define new NEW_NEW
};

#endif /*PETRINETNODE_H_*/
