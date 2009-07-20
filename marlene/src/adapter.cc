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
 * \file    adapter.cc
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

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include "adapter.h"
#include "macros.h"
#include "helper.h"
#include "pnapi/pnapi.h"


//! \brief a basic constructor of Adapter
Adapter::Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                ControllerType contType, unsigned int messageBound) :
    _engine(new pnapi::PetriNet), _nets(nets), _rs(rs), _contType(contType),
                    _messageBound(messageBound)
{
    FUNCIN
    //createEngineInterface();
    //createRuleTransitions();
    FUNCOUT
}


//! \brief a basic destructor of Adapter
Adapter::~Adapter()
{
    FUNCIN
    // deleting the engine if it exists
    if (_engine != NULL)
    {
        delete (_engine);
        _engine = NULL;
    }
    FUNCOUT
}

const pnapi::PetriNet * Adapter::buildEngine()
{
    FUNCIN
    createEngineInterface();
    createRuleTransitions();
    // adapter specific reduction should take place here
    
    _engine->reduce(pnapi::PetriNet::LEVEL_4);
    
    pnapi::Condition & finalCond = _engine->finalCondition();
    finalCond.addMarking(*(new pnapi::Marking(*_engine)));

    FUNCOUT
    return _engine;
}

const pnapi::PetriNet * Adapter::buildController()
{
    FUNCIN

    using namespace pnapi;

    if (_engine != NULL)
    {
        // we make a copy of the engine, #_engine is needed for the result
        // #_enginecopy for the synthesis of the controller
        PetriNet * _enginecopy = new PetriNet(*_engine);
        
        // create complementary places for the engine's internal places
        Adapter::createComplementaryPlaces(_enginecopy);
    
        // compose nets and engine
        std::map< std::string, pnapi::PetriNet * > nets;

        PetriNet * composed = new PetriNet(*_enginecopy);

        for ( unsigned int i = 0; i < _nets.size(); i++)
        {
            if ( i == 0 )
            {
                composed->compose(*_nets[i], std::string("engine."), std::string("net" + toString(i+1) + "." ));
            }
            else
            {
                composed->compose(*_nets[i], std::string(""), std::string("net" + toString(i+1) + "." ));
            }
                
        }
    
        Condition & finalCond = composed->finalCondition();
    
        
        // add complementary places to former interface.
        for ( unsigned int i = 0; i < _nets.size(); i++)
        {
            const std::set< Place * > & ifPlaces = _nets[i]->getInterfacePlaces();
            std::set< Place * >::const_iterator placeIter = ifPlaces.begin();
            while ( placeIter != ifPlaces.end() )
            {
                std::string placeName = (*placeIter)->getName();
                std::string placeName2 = "net" + toString(i+1) + "." + (*placeIter)->getName();
                
                Place * place = composed->findPlace(placeName);
                if (place == NULL)
                {
                    place = composed->findPlace(placeName2);
                    placeName = placeName2;
                }
                assert (place);
    
                Place * compPlace = &composed->createPlace("comp_" + placeName,
                    Node::INTERNAL, _messageBound+1, _messageBound+1);
    
                formula::FormulaEqual * prop = new formula::FormulaEqual(*compPlace,_messageBound);
                finalCond.addProposition(*prop);
                
                std::set< Node * > postSet = place->getPostset();
                std::set< Node * >::iterator nodeIter = postSet.begin();
                while ( nodeIter != postSet.end() )
                {
                    composed->createArc(**nodeIter, *compPlace);
                    nodeIter++;
                }

                std::set< Node * > preSet = place->getPreset();
                nodeIter = preSet.begin();
                while ( nodeIter != preSet.end() )
                {
                    composed->createArc(*compPlace, **nodeIter);
                    
                    // deadlock transition for message bound violation of former interface
                    Transition * dlTrans = &composed->createTransition("dl_" + placeName);
                    composed->createArc(*place, *dlTrans, _messageBound + 1);
                    
                    nodeIter++;
                }
                
                placeIter++;
            }
        }
        
        // finally reduce the strucure of the net as far as possible
        composed->reduce(pnapi::PetriNet::LEVEL_4);
    
        if (_contType == ASYNCHRONOUS)
        {
            composed->normalize();
        }

        /***********************************\
        * calculate most permissive partner *
        \***********************************/
        // create a unique temporary file name
        char tmp[] = "/tmp/marlene-XXXXXX";
#ifdef HAVE_MKSTEMP
        if (mkstemp(tmp) == -1) {
            abort(9, "could not create a temporary file '%s'", tmp);
        }
#endif

        std::string tmpname(tmp);
        std::string owfn_filename = tmpname + ".owfn";
        std::string sa_filename = tmpname + ".sa";

        {
            std::ofstream owfn_file(owfn_filename.c_str(), std::ios_base::out);
            if (! owfn_file)
            {
                abort(5, "could not create open net file");
            }
            owfn_file << pnapi::io::owfn << *composed;
            owfn_file.close();
        }

        std::string wendy_command = std::string(args_info.wendy_arg) + " " + owfn_filename
             + " --sa=" + sa_filename;
        wendy_command += " -m" + toString(_messageBound);
        
        time_t start_time, end_time;

        wendy_command += ((args_info.verbose_flag) ? " --verbose" : " 2> /dev/null");
        time(&start_time);
        status("executing '%s'", wendy_command.c_str());
        system(wendy_command.c_str());
        time(&end_time);
        status("Wendy done [%.0f sec]", difftime(end_time, start_time));

        /*******************************\
        * parse most-permissive partner *
        \*******************************/
        std::ifstream sa_file(sa_filename.c_str(), std::ios_base::in);
        if (! sa_file) {
            abort(5, "could not read controller");
        }
        pnapi::Automaton * mpp_sa = new pnapi::Automaton();
        sa_file >> pnapi::io::sa >> *mpp_sa;
        sa_file.close();

        /***********************************************\
        * transform most-permissive partner to open net *
        \***********************************************/
        time(&start_time);
        pnapi::PetriNet * controller = new pnapi::PetriNet(mpp_sa->stateMachine());
        time(&end_time);
        if (args_info.verbose_flag) {
            std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << *controller << std::endl;
        }
        status("converting most-permissive partner done [%.0f sec]", difftime(end_time, start_time));
        
        return controller;
        FUNCOUT
        
        
    }
    
    return NULL;
    FUNCOUT
}
    
/**
 * \brief   creates the interface towards the involved services
 */
void Adapter::createEngineInterface()
{
    FUNCIN
    assert(_engine != NULL);
    
    for (unsigned int netindex = 0; netindex < _nets.size(); netindex++)
    {
        using namespace pnapi;
        // renaming the net is done when merging the nets

        const std::set< Place * > & netIf = _nets[netindex]->getInterfacePlaces();
        for (std::set< Place *>::const_iterator place = netIf.begin(); place
                        != netIf.end(); place++)
        {
            // get interface name
            const std::string & ifname = (*place)->getName();
            // internal name
            std::string internalname = (*place)->getName() + "_int";
            
            // create interface place for a service's interface place
            Place & ifplace = _engine->createPlace(ifname, 
                            ((*place)->getType() == Node::INPUT) ? Node::OUTPUT
                            : Node::INPUT, 0, _messageBound);
            
            if (!_engine->findPlace(internalname))
            {
                //! the internal place which is a copy of the interface place
                Place & intplace = _engine->createPlace(internalname,
                                Node::INTERNAL, 0, _messageBound);
                
                //! name of sending/receiving transition
                std::string inttransname(( ((*place)->getType() == Node::INPUT ) ? "t_send_"
                                : "t_receive_") + ifname);
                
                //! transition which moves the token between the interface place and its copy
                Transition & inttrans =_engine->createTransition( inttransname );
                
                // create arcs between the internal place, the transition 
                // and the interface place
                if ((*place)->getType() == Node::INPUT)
                {
                    _engine->createArc(intplace, inttrans);
                    _engine->createArc(inttrans, ifplace);
                }
                else
                {
                    _engine->createArc(ifplace, inttrans);
                    _engine->createArc(inttrans, intplace);
                }
                
                // if we have a synchronous interface, create label for transition
                if (_contType == SYNCHRONOUS)
                {
                    std::set< std::string > syncLabel;
                    syncLabel.insert("sync_" + inttransname);
                    inttrans.setSynchronizeLabels(syncLabel);
                }
                // else create interface place to the controller
                else
                {
                    std::string contplacename =
                                    (((*place)->getType()
                                                    == Node::INPUT) ? "send_"
                                        : "receive_") + ifname;
                    Place
                                    & contplace =
                                                    _engine->createPlace(
                                                        contplacename,
                                                        ((*place)->getType()
                                                                        == Node::INPUT) ? Node::INPUT
                                                            : Node::OUTPUT, 0,
                                                        _messageBound);
                    if ((*place)->getType() == Node::INPUT)
                    {
                        _engine->createArc(contplace, inttrans);
                    } else
                    {
                        _engine->createArc(inttrans, contplace);
                    }
                }
            }
        }
    }
    FUNCOUT
}

void Adapter::createRuleTransitions()
{
    FUNCIN
    using namespace pnapi;
    
    // transitions are numbered
    unsigned int transNumber = 1;
    
    // get given rules
    std::list< RuleSet::AdapterRule * > rules = _rs.getRules();
    // iterate them
    std::list< RuleSet::AdapterRule * >::iterator ruleIter = rules.begin();
    while ( ruleIter != rules.end() )
    {
        const RuleSet::AdapterRule & rule = **ruleIter;
        
        std::string transName = "rule_" + toString(transNumber);
        
        // create rule transition
        
        // maybe transition alread exists (but why!)
        Transition * trans = _engine->findTransition(transName);
        // so let's have an assertion here
        assert (trans == NULL);
        
        if (trans == NULL)
        {
            trans = &_engine->createTransition(transName);
        }
        
        // now for the places, iterate and connect them to the transition
        std::list< unsigned int > messageList = rule.getRule().first;
        std::list< unsigned int >::iterator messageIter = messageList.begin();
        while ( messageIter != messageList.end() )
        {
            // create places for precondition
            std::string placeName = _rs.getMessageForId(*messageIter) + "_int";
            
            // let's look, if the place already exists
            Place * place = _engine->findPlace(placeName);
            
            // nope, create it
            if ( place == NULL )
            {
                place = &_engine->createPlace(placeName, Node::INTERNAL, 0, _messageBound);
            }
            
            // create arc between this place and rule transition
            _engine->createArc(*place, *trans);
            
            messageIter++;
        }
        
        messageList = rule.getRule().second;
        messageIter = messageList.begin();
        while ( messageIter != messageList.end() )
        {
            // create places for postcondition
            std::string placeName = _rs.getMessageForId(*messageIter) + "_int";
            
            // let's look, if the place already exists
            Place * place = _engine->findPlace(placeName);
            
            // nope, create it
            if ( place == NULL )
            {
                place = &_engine->createPlace(placeName, Node::INTERNAL, 0, _messageBound);
            }
            
            // create arc between this place and rule transition
            _engine->createArc(*trans, *place);
            
            messageIter++;
        }
        
        // if we have a synchronous interface, create label for transition
        if (_contType == SYNCHRONOUS)
        {
            std::set< std::string > syncLabel;
            syncLabel.insert("sync_" + transName);
            trans->setSynchronizeLabels(syncLabel);
        }
        // else create interface place to the controller
        else
        {
            std::string controlplacename = "control_" + transName;
            std::string observeplacename = "observe_" + transName;
            
            //if (rule.getMode() == )
            Place & controlplace = _engine->createPlace( controlplacename, Node::INPUT, 0, _messageBound);
            _engine->createArc(controlplace, *trans);
            
            Place & observeplace = _engine->createPlace( observeplacename, Node::OUTPUT, 0, _messageBound);
            _engine->createArc(*trans, observeplace);

        }
        // next Tansition
        transNumber++;
        ruleIter++;
    }
    FUNCOUT
}

void Adapter::createComplementaryPlaces(pnapi::PetriNet * net)
{
    FUNCIN
    
    using namespace pnapi;
    
    std::set< Place * > intPlaces = net->getInternalPlaces();
    std::set< Place * >::iterator placeIter = intPlaces.begin();
    
    while ( placeIter != intPlaces.end() )
    {
        Place * place = *placeIter;
        Place * compPlace = &net->createPlace("comp_" + place->getName(),
            Node::INTERNAL, place->getCapacity(), place->getCapacity());
        
        // update final condition
        Condition & finalCond = net->finalCondition();

        formula::FormulaEqual * prop = new formula::FormulaEqual(*compPlace, place->getCapacity());
        finalCond.addProposition(*prop);

        std::set< Node * > preSet = place->getPreset();
        std::set< Node * >::iterator nodeIter = preSet.begin();
        while ( nodeIter != preSet.end() )
        {
            net->createArc(*compPlace, **nodeIter);
            nodeIter++;
        }
        
        std::set< Node * > postSet = place->getPostset();
        nodeIter = postSet.begin();
        while ( nodeIter != postSet.end() )
        {
            net->createArc(**nodeIter, *compPlace);
            nodeIter++;
        }
        
        placeIter++;
    }
    
    FUNCOUT
}

RuleSet::RuleSet() :
    _maxId(0)
{
    FUNCIN
    /* empty */
    FUNCOUT
}

RuleSet::~RuleSet()
{
    FUNCIN
    std::list< AdapterRule * >::iterator rule = _adapterRules.begin();
    while (rule != _adapterRules.end())
    {
        delete *rule;
        
        rule++;
    }
    _adapterRules.clear();
    FUNCOUT
}

void RuleSet::addRules(FILE * inputStream)
{
    FUNCIN
    extern FILE * adapt_rules_yyin;
    extern int adapt_rules_yyparse();
    extern RuleSet * workingSet;
    extern int adapt_rules_yylineno;
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    extern int adapt_rules_yylex_destroy();
#endif

    // \TODO: Parser
    /*
    extern int adapt_rules_yydebug;
    extern int adapt_rules_yy_flex_debug;
    extern FILE* adapt_rules_yyin;
    extern int adapt_rules_yyerror();
    extern int adapt_rules_yyparse();

    */
    
    
    workingSet = this;
    adapt_rules_yyin = inputStream;
    adapt_rules_yylineno = 1;
    
    adapt_rules_yyparse();
    FUNCOUT
}

inline const std::list< RuleSet::AdapterRule * > RuleSet::getRules() const
{
    FUNCIN
    FUNCOUT
    return _adapterRules;
}

inline const std::string RuleSet::getMessageForId(const unsigned int id) const
{
    FUNCIN
    std::map< unsigned int, std::string >::const_iterator iter = _messageIndex.find(id);
    if ( iter != _messageIndex.end() )
    {
        FUNCOUT
        return iter->second;
    }
    else
    {
        FUNCOUT
        return "";
    }
}

unsigned int RuleSet::getIdForMessage(std::string message)
{
    FUNCIN
    // getting the id once is enough
    unsigned int id = _messageId[message]; 
    if ( id == 0 )
    {
        ++_maxId;
        _messageIndex[_maxId] = message;
        FUNCOUT
        return (_messageId[message] = _maxId);
    }
    else
    {
        FUNCOUT
        return id;
    }
}

RuleSet::AdapterRule::AdapterRule(std::pair< std::list<unsigned int>, std::list<unsigned int> > & rule, cfMode & modus) :
    _rule(rule), _modus(modus)
{   
    FUNCIN
    /* empty */
    FUNCOUT
}

RuleSet::AdapterRule::~AdapterRule()
{
    FUNCIN
    /* empty */
    FUNCOUT
}

inline const RuleSet::AdapterRule::rulepair & RuleSet::AdapterRule::getRule() const 
{
    FUNCIN
    FUNCOUT
    return _rule; 
}

inline const RuleSet::AdapterRule::cfMode & RuleSet::AdapterRule::getMode() const
{ 
    FUNCIN
    FUNCOUT
    return _modus; 
}

