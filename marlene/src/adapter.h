/*****************************************************************************
 * Copyright 2008 Christian Gierds                                           *
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
 * \file    adapter.h
 *
 * \brief   all functionality for adapter generation
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitaet zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef ADAPTER_H_
#define ADAPTER_H_

#include <vector>
#include <iostream>
#include "pnapi/pnapi.h"

// forward declarations
class Adapter;
class RuleSet;

/**
 * 
 * 
 */

class Adapter
{

    public:

        //! type of interface between engine and controller
        enum ControllerType {SYNCHRONOUS, ASYNCHRONOUS};
        
        //! constructor
        Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                        ControllerType contType = SYNCHRONOUS,
                        unsigned int messagebound = 1);

        //! basic destructor
        ~Adapter();
        
        //! actually build the engine
        const pnapi::PetriNet * buildEngine();
        
        //! actually build the controller
        const pnapi::PetriNet * buildController();

    private:
        
        //! #pnapi::PetriNet containing the engine part of the adapter
        pnapi::PetriNet * _engine;
        
        //! vector of all nets being adapted
        std::vector< pnapi::PetriNet * > & _nets;
        
        //! a #RuleSet which describes the engines behavior
        RuleSet & _rs;
        
        //! the modus operandi for the engine/controller interface
        ControllerType _contType;
        
        //! message bound on interface, neccesary for complement places
        unsigned int _messageBound;

        //! given the #_nets, creates the interface for #_engine
        void createEngineInterface();
        
        //! given the #_rs rule set, creates the transitions handling the rule for #_engine
        void createRuleTransitions();
        
        //! creates complementary places for all internal places
        static void createComplementaryPlaces(pnapi::PetriNet * net);
        

};

class RuleSet {
    public:
        
        class AdapterRule {
            
            public:
                typedef std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair;
                
                enum cfMode { AR_NORMAL, AR_HIDDEN, AR_OBSERVABLE, AR_CONTROLLABLE };

                AdapterRule(rulepair & rule, cfMode modus = AR_NORMAL); 

                ~AdapterRule();
                
                inline const rulepair & getRule() const;
                inline const cfMode & getMode() const;
            
            private:
                const std::pair< std::list< unsigned int >, std::list< unsigned int > > _rule;
                const cfMode _modus;
            
        };

        /// basic constructor
        RuleSet();
        
        /// basic destructor
        ~RuleSet();
        
        /// adds rules to #rules from a given input stream #is 
        void addRules(FILE *);
        
        /// returns a const reference to the rules
        inline const std::list< AdapterRule * > getRules() const;
        
        /// returns the message name for an id
        inline const std::string getMessageForId(const unsigned int) const; 

    private:
        std::map< unsigned int, std::string > _messageIndex;
        std::map< std::string, unsigned int > _messageId;
        std::list< AdapterRule * > _adapterRules;
        
        unsigned int _maxId;

        //! returns a unique id for a message name
        unsigned int getIdForMessage(std::string message);
                
        friend int adapt_rules_yyparse();
        
};


#endif /*ADAPTER_H_*/
