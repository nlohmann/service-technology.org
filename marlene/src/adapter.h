/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

/*!
 * \file    adapter.h
 *
 * \brief   all functionality for adapter generation
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
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
 * \brief Given nets and rules creates engine, controller and finally an 
 *        adapter.
 * 
 * Typical usage of this class is to read the input nets and a rule set, which 
 * are given to the constructor of this class, and then run 
 * Adapter::buildEngine() and Adapter::buildController().
 */

class Adapter
{

    public:

        //! type of interface between engine and controller
        enum ControllerType {SYNCHRONOUS, ASYNCHRONOUS};
        
        /*! 
         * \brief Given a vector of nets, a rule set, the type of the controller 
         *        and a message bound initializes the adapter.
         * 
         * \param nets          a vector of nets that shall be adapted
         * \param rs            a set of message transformation rules
         * \param contType      sets the interface between engine and controller
         *                      to be SYNCHRONOUS or ASYNCHRONOUS
         * \param useCompPlaces flag, whether to use complementary places in the
         *                      engine
         * \param messagebound  the messagebound of the nets
         */ 
        Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                        ControllerType contType = SYNCHRONOUS,
                        unsigned int messagebound = 1, 
                        bool useCompPlace = true);

        /*!
         * \brief Destroys the adapter, also deletes Adapter::_engine.
         */
        ~Adapter();
        
        /*!
         * \brief Actually builds the engine.
         * 
         * \return pointer to the engine containing appropriate interface and 
         *         rule transitions
         */
        const pnapi::PetriNet * buildEngine();
        
        /*!
         * \brief Actually builds the controller.
         * 
         * \note Should only be called after Adapter::buildEngine()
         * 
         * \return pointer to the controller of Adapter::_engine composed with 
         *         Adapter::_nets, NULL if no controller could be built 
         */
        const pnapi::PetriNet * buildController();

    private:
        
        //! pnapi::PetriNet containing the engine part of the adapter
        pnapi::PetriNet * _engine;
        
        //! vector of all nets being adapted
        std::vector< pnapi::PetriNet * > & _nets;
        
        //! a #RuleSet which describes the engines behavior
        RuleSet & _rs;
        
        //! the modus operandi for the engine/controller interface
        ControllerType _contType;
        
        //! message bound on interface, neccesary for complement places
        unsigned int _messageBound;

        //! flag indicating, whether to use complementary places or not
        bool _useCompPlaces;
        
        /*!
         * \brief Given the #_nets, creates the interface for #_engine.
         */
        void createEngineInterface();
        
        /*!
         * \brief Given the rule set #_rs, creates the transitions handling the 
         *        rules for #_engine.
         */
        void createRuleTransitions();
        
        /*!
         * \brief Creates complementary places for all internal places.
         * 
         * \param net pointer to the net, for which complementary places shall be created
         */
        static void createComplementaryPlaces(pnapi::PetriNet & net);
        
        /*!
         * \brief Removes rules that hinder reaching the controller goal, or are just superfluous.
         */
        void removeUnnecessaryRules();
        
        /*!
         * \brief Finds transitions that have atleast one place in their preset
         *        without any confict, and removes these transitions' interface
         *        to the controller.
         */
        void findConflictFreeTransitions();
        
        /*! 
         * \brief Returns the name for the rule with index i.
         * 
         * \param i index of the rule (so its number)
         * 
         * \return string representing the rule's for using as transition name
         */ 
        static inline std::string getRuleName(unsigned int i);
        

};

/*!
 * \brief Contains a set of message transformation rules.
 * 
 * Typical usage of this class is to create an object of it and to add rules by
 * calling addRules().
 */ 
class RuleSet {
    public:
        
        /*!
         * \brief Represents exactly one rule.
         */
        class AdapterRule {
            
            public:
                //! type of a rule pair
                typedef std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair;
                
                //! type of a list of sync channels
                typedef std::list< unsigned int > syncList;
                
                /*! 
                 * \brief Modus of a transformation rule
                 *
                 * Normally a transformation rule is observable and 
                 * controllable. So, the rule must be triggered, in order to 
                 * apply it (controllable) and it can be observed, if the rule
                 * was actually applied.
                 * For sake of expressiveness this behavior can be restricted.
                 */
                enum cfMode { 
                    AR_NORMAL,      //!< rule is observable and controllable
                    AR_HIDDEN,      //!< rule is neither observable nor controllable
                    AR_OBSERVABLE,  //!< rule is observable, but not contrallable
                    AR_CONTROLLABLE //!< rule is controllable, but not obserable
                    };

                /*!
                 * \brief Given a #rulepair and a modus for the transformation
                 *        rule create an adapter rule.
                 * 
                 * \param rule  a reference to a #rulepair
                 * \param modus modus of the transformation rule
                 */
                AdapterRule(rulepair & rule, syncList & slist, cfMode modus = AR_NORMAL); 

                /*!
                 * \brief basic destructor, nothing happens here
                 */
                ~AdapterRule();
                
                /*!
                 * \brief Returns the #rulepair associated with the object.
                 * 
                 * \return the associated #rulepair
                 */
                inline const rulepair & getRule() const;
                
                /*!
                 * \brief Returns the #syncList associated with the object.
                 * 
                 * \return the associated #syncList
                 */
                inline const syncList & getSyncList() const;
                
                /*!
                 * \brief Returns the modus of the transformation rule.
                 * 
                 * \return the transformation rule's modus
                 */
                inline const cfMode & getMode() const;
            
            private:
                
                /*! 
                 * \brief a std::pair of unsigned int lists containing the 
                 *        consumed (left) and produced (right) messages of
                 *        a transformation rule
                 * 
                 * \note messages are referenced by their internal id
                 *       (see RuleSet::getMessageForId)
                 */
                const std::pair< std::list< unsigned int >, std::list< unsigned int > > _rule;
                
                /*!
                 * \brief a list of unsigned int containing the channels to
                 *        synchronize with when applying this rule
                 */
                const std::list< unsigned int > _syncList;
                
                //! modus of the commnunication flow for this rule
                const cfMode _modus;
            
        };

        //! basic constructor
        RuleSet();
        
        //! basic destructor
        ~RuleSet();
        
        /*! 
         * \brief Adds rules to #_adapterRules from a given input file.
         * 
         * \param inputStream a file pointer to the file containing the transformation rules
         */ 
        void addRules(FILE * inputStream);
        
        /*!
         * \brief Returns a const reference to the transformation rules.
         * 
         * \return list of transformation rules
         */
        inline const std::list< AdapterRule * > getRules() const;
        
        /*!
         * \brief Returns the message name for an ID.
         * 
         * \param id the id of message we want the name for
         * 
         * \return the message name connected to an ID
         */
        inline const std::string getMessageForId(const unsigned int id) const; 

    private:
        //! a mapping from IDs to message names (contrary of #_messageId)
        std::map< unsigned int, std::string > _messageIndex;
        
        //! a mapping from message names to IDs (contrary of #_messageIndex)
        std::map< std::string, unsigned int > _messageId;
        
        //! a list of message transformation rules
        std::list< AdapterRule * > _adapterRules;
        
        //! the highest used ID for a message name, is increased by getIdForMessage()
        unsigned int _maxId;

        /*! 
         * \brief Returns a unique ID for a message name.
         * 
         * \note Increases #_maxId, if a new ID is assigned
         * 
         * \param message the message you want an ID for
         * 
         * \return the unique ID
         */
        unsigned int getIdForMessage(std::string message);
                
        /*! 
         * \brief Reference to the rules file parser, which shall have access to this classes members.
         * 
         * \return error code
         */
        friend int adapt_rules_yyparse();
        
};


#endif /*ADAPTER_H_*/
