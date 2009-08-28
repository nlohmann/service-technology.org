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

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include "adapter.h"
#include "macros.h"
#include "helper.h"
#include "pnapi/pnapi.h"


/* For documentation of the following functions, please see header file. */

Adapter::Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                ControllerType contType, unsigned int messageBound, 
                bool useCompPlaces) :
    _engine(new pnapi::PetriNet), _nets(nets), _rs(rs), _contType(contType),
                    _messageBound(messageBound), _useCompPlaces(useCompPlaces)
{
    FUNCIN
    /* empty */
    FUNCOUT
}

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
    // create engine and transitions for the transformation rules
    createEngineInterface();
    createRuleTransitions();

    // adapter specific reduction should take place here
    removeUnnecessaryRules();
    findConflictFreeTransitions();
    
    // reduce engine with standard PNAPI methods
    _engine->reduce(pnapi::PetriNet::LEVEL_4);
    
    // set final condition
    pnapi::Condition & finalCond = _engine->finalCondition();
    finalCond.addMarking(*(new pnapi::Marking(*_engine)));

    FUNCOUT
    return _engine;
}

const pnapi::PetriNet * Adapter::buildController()
{
    FUNCIN

    using namespace pnapi;

    // _engine should exist here 
    if (_engine != NULL)
    {
        // we make a copy of the engine, #_engine is needed for the result
        // #_enginecopy for the synthesis of the controller
        PetriNet * _enginecopy = new PetriNet(*_engine);
        
        // create complementary places for the engine's internal places
        if (_useCompPlaces)
        {
            Adapter::createComplementaryPlaces(*_enginecopy);
        }
        else
        {
            status("skipping creation of complementary places once ..");
        }
    
        // compose nets and engine
        std::map< std::string, pnapi::PetriNet * > nets;

        PetriNet * composed = new PetriNet(*_enginecopy);

        // compose engine with nets
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

        // add complementary places for the now internal former interface places
        if (_useCompPlaces)
        {
            Condition & finalCond = composed->finalCondition();

            for (unsigned int i = 0; i < _nets.size(); i++)
            {
                const std::set< Place *> & ifPlaces =
                                _nets[i]->getInterfacePlaces();
                std::set< Place *>::const_iterator placeIter = ifPlaces.begin();
                while (placeIter != ifPlaces.end() )
                {
                    std::string placeName = (*placeIter)->getName();
                    std::string placeName2 = "net" + toString(i+1) + "." + (*placeIter)->getName();
                    
                    Place * place = composed->findPlace(placeName);
                    if (place == NULL)
                    {
                        place = composed->findPlace(placeName2);
                        placeName = placeName2;
                    }
                    assert(place);

                    Place * compPlace = &composed->createPlace("comp_"
                                    + placeName, Node::INTERNAL, _messageBound
                                    +1, _messageBound+1);

                    formula::FormulaEqual * prop = new formula::FormulaEqual(*compPlace,_messageBound + 1);
                    finalCond.addProposition(*prop);

                    std::set< Node *> postSet = place->getPostset();
                    std::set< Node *>::iterator nodeIter = postSet.begin();
                    while (nodeIter != postSet.end() )
                    {
                        composed->createArc(**nodeIter, *compPlace);
                        nodeIter++;
                    }

                    std::set< Node *> preSet = place->getPreset();
                    nodeIter = preSet.begin();
                    while (nodeIter != preSet.end() )
                    {
                        composed->createArc(*compPlace, **nodeIter);
                        nodeIter++;
                    }

                    // deadlock transition for message bound violation of former interface
                    Transition * dlTrans =
                                    &composed->createTransition("dl_"
                                                    + placeName);
                    composed->createArc(*place, *dlTrans, _messageBound + 1);

                    placeIter++;
                }
            }
        }
        else
        {
            status("skipping creation of complementary places twice ..");
        }
        
        // finally reduce the strucure of the net as far as possible
        composed->reduce(pnapi::PetriNet::LEVEL_4);
    
        if (_contType == ASYNCHRONOUS)
        {
            composed->normalize();
        }
        composed->reduce(pnapi::PetriNet::LEVEL_4);

        /***********************************\
        * calculate most permissive partner *
        \***********************************/
        // create a unique temporary file name
#if defined(HAVE_MKSTEMP) && not defined(__MINGW32__)
        char tmp[] = "/tmp/marlene-XXXXXX";
        if (mkstemp(tmp) == -1) {
            abort(9, "could not create a temporary file '%s'", tmp);
        }
#else
        status("setting tempfile name for MinGW to 'marlene.tmp'");
        char tmp[] = "marlene.tmp";
        
        //std::cerr << std::string(tmp) << std::endl;
#endif

        std::string tmpname(tmp);
        std::string owfn_filename = tmpname + ".owfn";
        std::string sa_filename = tmpname + ".sa";
        std::string og_filename = tmpname + ".og";

        {
            //std::cerr << owfn_filename << std::endl;
            std::ofstream owfn_file(owfn_filename.c_str(), std::ios_base::out);
            if (! owfn_file)
            {
                abort(5, "could not create open net file");
            }
            owfn_file << pnapi::io::owfn << *composed;
            owfn_file.close();
        }

        std::string wendy_command = std::string(args_info.wendy_arg) + " " + owfn_filename
             + " --sa=" + sa_filename; // + " --og=" + og_filename;
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
        pnapi::PetriNet * controller;
/*
        if (std::string(CONFIG_PETRIFY) != "not found")
        {
            controller = new pnapi::PetriNet(*mpp_sa);
        }
        else
*/
        {
            controller = new pnapi::PetriNet(mpp_sa->stateMachine());
        }
        time(&end_time);
        
        if (args_info.verbose_flag) {
            std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << *controller << std::endl;
        }
        status("converting most-permissive partner done [%.0f sec]", difftime(end_time, start_time));

        FUNCOUT
        return controller;
    }
    
    FUNCOUT
    return NULL;
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
        
        std::string transName (Adapter::getRuleName(transNumber));
        
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

void Adapter::createComplementaryPlaces(pnapi::PetriNet & net)
{
    FUNCIN
    
    using namespace pnapi;
    
    std::set< Place * > intPlaces = net.getInternalPlaces();
    std::set< Place * >::iterator placeIter = intPlaces.begin();
    
    while ( placeIter != intPlaces.end() )
    {
        Place & place = **placeIter;
        Place & compPlace = net.createPlace("comp_" + place.getName(),
            Node::INTERNAL, place.getCapacity(), place.getCapacity());
        
        // update final condition
        Condition & finalCond = net.finalCondition();

        formula::FormulaEqual * prop = new formula::FormulaEqual(compPlace, place.getCapacity());
        finalCond.addProposition(*prop);

        std::set< Node * > preSet = place.getPreset();
        std::set< Node * >::iterator nodeIter = preSet.begin();
        while ( nodeIter != preSet.end() )
        {
            net.createArc(compPlace, **nodeIter);
            nodeIter++;
        }
        
        std::set< Node * > postSet = place.getPostset();
        nodeIter = postSet.begin();
        while ( nodeIter != postSet.end() )
        {
            net.createArc(**nodeIter, compPlace);
            nodeIter++;
        }
        
        placeIter++;
    }
    
    FUNCOUT
}

void Adapter::removeUnnecessaryRules()
{
    FUNCIN

    using namespace pnapi;

    std::set<Place *> possibleDeadPlaces = _engine->getInternalPlaces();

    while ( !possibleDeadPlaces.empty() )
    {
        // find all place with an empty preset (so places are structually dead

        std::set<Place *>::iterator placeIter = possibleDeadPlaces.begin();

        std::list<Place *> placeDeletionList;

        while (placeIter != possibleDeadPlaces.end() )
        {
            if ((*placeIter)->getPreset().empty() )
            {
                //status("Place %s is dead.", (*placeIter)->getName().c_str());
                placeDeletionList.push_back(*placeIter);
            }
            placeIter++;
        }

        possibleDeadPlaces.clear();
        
        // delete dead places and all depending nodes
        std::list<Place *>::iterator place2Delete = placeDeletionList.begin();

        while (place2Delete != placeDeletionList.end() )
        {
            Place * p = *place2Delete;
            //status("Deleting post set of place %s.", p->getName().c_str());

            std::set<Node *> postset = p->getPostset();

            std::set<Node *>::iterator nodeIter = postset.begin();

            while (nodeIter != postset.end() )
            {
                if (_contType == ASYNCHRONOUS)
                {
                    status("Deleting transition %s.", (*nodeIter)->getName().c_str());

                    std::string controlplacename = "control_" + (*nodeIter)->getName();
                    std::string observeplacename = "observe_" + (*nodeIter)->getName();

                    Place * p;

                    p = _engine->findPlace(controlplacename);

                    if (p != NULL)
                    {
                        _engine->deletePlace(*p);
                    }

                    p = _engine->findPlace(observeplacename);

                    if (p != NULL)
                    {
                        _engine->deletePlace(*p);
                    }

                }

                std::set< Node * > deadCandidates = (*nodeIter)->getPostset();
                std::set< Node * >::const_iterator cand = deadCandidates.begin();
                
                while ( cand != deadCandidates.end() )
                {
                    // only internal places are appropriate for removal
                    if ( (*cand)->getType() == Node::INTERNAL )
                    {
                        possibleDeadPlaces.insert( dynamic_cast<Place*>(*cand) );
                    }
                    cand++;
                }
                

                _engine->deleteTransition(*(dynamic_cast<Transition*>(*nodeIter)));
                // deleting adjacent interface places, if they exist

                nodeIter++;
            }

            //status("Deleting place %s.", p->getName().c_str());
            _engine->deletePlace(*p);

            place2Delete++;
        }
    }
    FUNCOUT
}

void Adapter::findConflictFreeTransitions()
{
    FUNCIN
    using namespace pnapi;

    for (unsigned int i = 1; i <= _rs.getRules().size(); i++)
    {
        // get the name of the transition
        std::string transname(Adapter::getRuleName(i));

        Transition * trans = _engine->findTransition(transname);
        // does it still exist
        if (trans != NULL)
        {
            // check every pre place, if it is conflict free
            std::set< Node *> preset = trans->getPreset();
            std::set< Node *>::iterator placeIter = preset.begin();

            while (placeIter != preset.end() )
            {
                if ( (*placeIter)->getType() == pnapi::Place::INTERNAL && (*placeIter)->getPostset().size() == 1)
                {
                    // if controller type is synchronous remove label
                    if (_contType == SYNCHRONOUS)
                    {
                        std::set< std::string> labels =
                                        trans->getSynchronizeLabels();
                        labels.erase("sync_" + transname);
                        trans->setSynchronizeLabels(labels);
                    } 
                    else // remove interface places
                    {
                        std::string controlplacename = "control_" + transname;
                        std::string observeplacename = "observe_" + transname;

                        pnapi::Place * p;

                        p = _engine->findPlace(controlplacename);

                        if (p != NULL)
                        {
                            _engine->deletePlace(*p);
                        }

                        p = _engine->findPlace(observeplacename);

                        if (p != NULL)
                        {
                            _engine->deletePlace(*p);
                        }

                    }
                    placeIter = preset.end();
                } else
                {
                    placeIter++;
                }
            }
        }
    }
    FUNCOUT
}

//! returns the name for the rule with index i
inline std::string Adapter::getRuleName(unsigned int i)
{
    FUNCIN
    return "rule_" + toString(i);
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

RuleSet::AdapterRule::AdapterRule(std::pair< std::list<unsigned int>, std::list<unsigned int> > & rule, cfMode modus) :
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

