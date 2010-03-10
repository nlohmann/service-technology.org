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

#include <config.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#if defined(HAVE_SYS_WAIT_H) && ! defined(__MINGW32__)
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned int) (stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#include "adapternumerge.h"
#include "verbose.h"
//#include "helper.h"
#include "cmdline.h"
//#include "Output.h"
#include "pnapi.h"


std::string toString(const int i){
	
	std::stringstream ssout;
	ssout <<  i;
	std::string s(ssout.str());
	return s;
}



/* For documentation of the following functions, please see header file. */

Adapter::Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                ControllerType contType, unsigned int messageBound, 
                bool useCompPlaces) :
    _engine(new pnapi::PetriNet), _nets(nets), _rs(rs), _contType(contType),
                    _messageBound(messageBound), _useCompPlaces(useCompPlaces)
{
    
    /* empty */
    
}

Adapter::~Adapter()
{
	delete (_engine);
    _engine = NULL;
   /* 
    // deleting the engine if it exists
    
    delete (_controller);
    _controller = NULL;
    */
}

const pnapi::PetriNet * Adapter::buildEngine()
{
    
    // create engine and transitions for the transformation rules
    createEngineInterface();
    createRuleTransitions();
	// make  int places external if they have either no input or no output
	

/*	
    // adapter specific reduction should take place here
    removeUnnecessaryRules();
    //findConflictFreeTransitions();
    
    // reduce engine with standard PNAPI methods
    _engine->reduce(pnapi::PetriNet::LEVEL_4);
 */  
    // set final condition
    pnapi::Condition & finalCond = _engine->finalCondition();
    pnapi::Marking finalMarking (*_engine);
    finalCond.addMarking(finalMarking);
	//cout<<

    // create complementary places for the engine's internal places
/*    if (_useCompPlaces)
    {
        Adapter::createComplementaryPlaces(*_engine);
    }
    else
    {
        status("skipping creation of complementary places once ..");
    }

    */
    assert(_engine != NULL);

    // we make a copy of the engine, #_engine is needed for the result
    // #_enginecopy for the synthesis of the controller

    pnapi::PetriNet comp(*_engine);

    // compose engine with nets
    for (unsigned int i = 0; i < _nets.size(); i++)
    {
        if (i == 0)
        {
            comp.compose(*_nets[i], std::string("engine."),
                std::string("net" + toString(i+1) + "."));
        } else
        {
            comp.compose(*_nets[i], std::string(""), std::string("net"
                            + toString(i+1) + "."));
        }
    }
    return new pnapi::PetriNet(comp);
}

/*const pnapi::PetriNet * composeNetsEngine()
{
 
    // _engine should exist here 
    //assert(_engine != NULL);

    // we make a copy of the engine, #_engine is needed for the result
    // #_enginecopy for the synthesis of the controller

  /*  _composed=new pnapi::PetriNet(*_engine);

    // compose engine with nets
    for (unsigned int i = 0; i < _nets.size(); i++)
    {
        if (i == 0)
        {
            _composed->compose(*_nets[i], std::string("engine."),
                std::string("net" + toString(i+1) + "."));
        } else
        {
            _composed->compose(*_nets[i], std::string(""), std::string("net"
                            + toString(i+1) + "."));
        }
    }*/
	//	return new pnapi::PetriNet(*_engine);//_composed;
//}

//const pnapi::PetriNet * Adapter::buildController()
//{
 /*   

    using namespace pnapi;

    // _engine should exist here 
    assert(_engine != NULL);

    // we make a copy of the engine, #_engine is needed for the result
    // #_enginecopy for the synthesis of the controller

    PetriNet composed (*_engine);

    // compose engine with nets
    for (unsigned int i = 0; i < _nets.size(); i++)
    {
        if (i == 0)
        {
            composed.compose(*_nets[i], std::string("engine."),
                std::string("net" + toString(i+1) + "."));
        } else
        {
            composed.compose(*_nets[i], std::string(""), std::string("net"
                            + toString(i+1) + "."));
        }
    }

    // add complementary places for the now internal former interface places
    if (_useCompPlaces)
    {
        Condition & finalCond = composed.finalCondition();

        for (unsigned int i = 0; i < _nets.size(); i++)
        {
            const std::set< Place *> & ifPlaces = _nets[i]->getInterfacePlaces();
            std::set< Place *>::const_iterator placeIter = ifPlaces.begin();
            while (placeIter != ifPlaces.end() )
            {
                // \todo: warum werden beide Platznamen gebraucht? wo geht hier beim Komponieren was schief?
                std::string placeName = (*placeIter)->getName();
                std::string placeName2 = "net" + toString(i+1) + "." + (*placeIter)->getName();

                Place * place = composed.findPlace(placeName);
                //if (place == NULL)
                //{
                //    place = composed->findPlace(placeName2);
                //    placeName = placeName2;
                //}
                assert(place);

                Place * compPlace = &composed.createPlace("comp_" + placeName,
                    Node::INTERNAL, _messageBound +1, _messageBound+1);

                formula::FormulaEqual prop (formula::FormulaEqual(*compPlace,_messageBound + 1));
                finalCond.addProposition(prop);

                std::set< Node *> postSet = place->getPostset();
                std::set< Node *>::iterator nodeIter = postSet.begin();
                while (nodeIter != postSet.end() )
                {
                    composed.createArc(**nodeIter, *compPlace);
                    nodeIter++;
                }

                std::set< Node *> preSet = place->getPreset();
                nodeIter = preSet.begin();
                while (nodeIter != preSet.end() )
                {
                    composed.createArc(*compPlace, **nodeIter);
                    nodeIter++;
                }

                // deadlock transition for message bound violation of former interface
                Transition * dlTrans = &composed.createTransition("dl_"
                                + placeName);
                composed.createArc(*place, *dlTrans, _messageBound + 1);

                ++placeIter;
            }
        }
    } else
    {
        status("skipping creation of complementary places twice ..");
    }

    // finally reduce the strucure of the net as far as possible
    composed.reduce(pnapi::PetriNet::LEVEL_4);

    if (_contType == ASYNCHRONOUS)
    {
        composed.normalize();
    }
    // \todo: Experiment, ob sich das hier noch lohnt.
    composed.reduce(pnapi::PetriNet::LEVEL_4 || pnapi::PetriNet::KEEP_NORMAL);

    /***********************************\
        * calculate most permissive partner *
     \***********************************/
    // create a unique temporary file name
  /*  Output owfn_temp;
    //        Output sa_temp;
    //       Output og_temp;

    std::string owfn_filename(owfn_temp.name());
    std::string sa_filename = owfn_filename + ".sa";
    std::string og_filename = owfn_filename + ".og";
    std::string cost_filename = owfn_filename + ".cf";

    owfn_temp.stream() << pnapi::io::owfn << composed;

    std::string wendy_command;
    std::string candy_command;

    // should we diagnose?
    if (args_info.diagnosis_flag)
    {
        wendy_command = std::string(args_info.wendy_arg) + " " + owfn_filename
                        + " --diagnose --im --mi --og=" + og_filename;
    } else
    {
        if (args_info.costoptimized_flag)
        {
            Output cf_file(cost_filename, "cost file");
            cf_file.stream() << cost_file_content;

            // optimization
            wendy_command = std::string(args_info.wendy_arg) + " "
                            + owfn_filename + " --og=-";
            candy_command = " | " + std::string(args_info.candy_arg)
                            + " --automata --output=" + sa_filename
                            + " --costfile=" + cost_filename;
        } else
        {
            // default behavior
            wendy_command = std::string(args_info.wendy_arg) + " "
                            + owfn_filename + " --sa=" + sa_filename; // + " --og=" + og_filename;
        }
    }
    wendy_command += " -m" + toString(_messageBound);

    if (args_info.chatty_flag)
    {
        wendy_command += " --succeedingSendingEvent";
    } else if (args_info.arrogant_flag)
    {
        wendy_command += " --receivingBeforeSending";
    }

    time_t start_time, end_time;

#ifdef __MINGW32__
    wendy_command += ((args_info.verbose_flag) ? " --verbose" : " 2> NUL");
    candy_command += ((args_info.verbose_flag) ? " --verbose" : " 2> NUL");
#else
    wendy_command += ((args_info.verbose_flag) ? " --verbose"
                    : " 2> /dev/null");
    candy_command += ((args_info.verbose_flag) ? " --verbose 1>&2"
                    : " 1>&2 2> /dev/null");
#endif

    if (args_info.costoptimized_flag)
    {
        wendy_command += candy_command;
    }
    time(&start_time);
    status("executing '%s'", wendy_command.c_str());
    int result = system(wendy_command.c_str());
    result = WEXITSTATUS(result);
    time(&end_time);
    status("Wendy done [%.0f sec]", difftime(end_time, start_time));

    if (result != 0)
    {
        message("Wendy returned with status %d.", result);
        message("Controller could not be built! No adapter was created, exiting.");
        exit(EXIT_FAILURE);
    }

    /*******************************\
        * parse most-permissive partner *
     \*******************************/
   /* std::ifstream sa_file(sa_filename.c_str(), std::ios_base::in);
    if (!sa_file)
    {
        message("Controller was not built! No adapter was created, exiting.");
        exit(EXIT_FAILURE);
    }
    pnapi::Automaton * mpp_sa = new pnapi::Automaton();
    sa_file >> pnapi::io::sa >> *mpp_sa;
    sa_file.close();

    /***********************************************\
        * transform most-permissive partner to open net *
     \***********************************************/
  /*  time(&start_time);
    delete _controller;

    if (args_info.sa2on_arg == sa2on_arg_petrify and std::string(args_info.petrify_arg) != "not_found") // && _contType == ASYNCHRONOUS)

    {
        status("Using Petrify for conversion from SA to open net.");
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::PETRIFY);
        pnapi::PetriNet::setPetrify(args_info.petrify_arg);
        _controller = new pnapi::PetriNet(*mpp_sa);
    }
    else
    if (args_info.sa2on_arg == sa2on_arg_genet and std::string(args_info.genet_arg) != "not_found") // && _contType == ASYNCHRONOUS)

    {
        status("Using Genet for conversion from SA to open net.");
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::GENET);
        pnapi::PetriNet::setGenet(args_info.genet_arg);
        _controller = new pnapi::PetriNet(*mpp_sa);
    }
    else
    {
        status("Using a state machine for conversion from SA to open net.");
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::STATEMACHINE);
        _controller = new pnapi::PetriNet(*mpp_sa);
    }
    delete mpp_sa;
    time(&end_time);

    if (args_info.verbose_flag)
    {
        std::cerr << PACKAGE << ": most-permissive partner: "
                        << pnapi::io::stat << *_controller << std::endl;
    }
    status("converting most-permissive partner done [%.0f sec]", difftime(
        end_time, start_time));

    */
    //return _controller;
//}
    
/**
 * \brief   creates the interface towards the involved services
 */
void Adapter::createEngineInterface()
{
    
    assert(_engine != NULL);
    
    for (unsigned int netindex = 0; netindex < _nets.size(); netindex++)
    {
        using namespace pnapi;
        // renaming the net is done when merging the nets
        
        std::string portname = "net" + toString(netindex);

        const std::set< Place * > & netIf = _nets[netindex]->getInterfacePlaces();
        for (std::set< Place *>::const_iterator place = netIf.begin(); place
                        != netIf.end(); place++)
        {
            // get interface name
            const std::string & ifname = (*place)->getName();
            // internal name
            std::string internalname;
            //if (args_info.withprefix_flag)
            //{
             //   internalname  = ((*place)->getName().substr((*place)->getName().find_first_of(".") + 1) + "_int");
            //}
            //else
            //{
                internalname  = ((*place)->getName() + "_int");
            ///}
            
            // create interface place for a service's interface place
            Place & ifplace = _engine->createPlace(ifname, 
                            ((*place)->getType() == Node::INPUT) ? Node::OUTPUT
                            : Node::INPUT, 0, _messageBound, portname);
            
            Place * intplace = _engine->findPlace(internalname);
            if (intplace == NULL)
            {
                //! the internal place which is a copy of the interface place
                intplace = &_engine->createPlace(internalname,
                                Node::INTERNAL, 0, _messageBound);
                
            }
            //! name of sending/receiving transition
            std::string inttransname(( ((*place)->getType() == Node::INPUT ) ? "t_send_"
                            : "t_receive_") + ifname);
            
            //! transition which moves the token between the interface place and its copy
            Transition & inttrans =_engine->createTransition( inttransname );
            
            // create arcs between the internal place, the transition 
            // and the interface place
            if ((*place)->getType() == Node::INPUT)
            {
                _engine->createArc(*intplace, inttrans);
                _engine->createArc(inttrans, ifplace);
            }
            else
            {
                _engine->createArc(ifplace, inttrans);
                _engine->createArc(inttrans, *intplace);
            }
            
            // if we have a synchronous interface, create label for transition
            if (_contType == SYNCHRONOUS)
            {
                std::set< std::string> syncLabel;
                syncLabel.insert("sync_" + inttransname);
                inttrans.setSynchronizeLabels(syncLabel);

                // workaround for bug #14416 <https://gna.org/bugs/?14416>
                // \TODO: remove!
                std::set< std::string > labels (_engine->getSynchronousLabels());
                labels.insert("sync_" + inttransname);
                _engine->setSynchronousLabels(labels);

               // cost_file_content += "sync_" + inttransname + " 0;\n";
                
            }
            // else create interface place to the controller
            else
            {
                std::string contplacename = (((*place)->getType() == Node::INPUT) ? "send_"
                    : "receive_") + ifname;
                Place & contplace =
                                _engine->createPlace(contplacename, ((*place)->getType()
                                                == Node::INPUT) ? Node::INPUT
                                    : Node::OUTPUT, 0, _messageBound, "controller");
                if ((*place)->getType() == Node::INPUT)
                {
                    _engine->createArc(contplace, inttrans);
                } else
                {
                    _engine->createArc(inttrans, contplace);
                }
            }
        }
        
        std::set< std::string > ifLabels(_nets[netindex]->getSynchronousLabels());
        std::set< std::string > engineLabels(_engine->getSynchronousLabels());
        engineLabels.insert(ifLabels.begin(), ifLabels.end());
        _engine->setSynchronousLabels(engineLabels);
        
    }
    
}

void Adapter::createRuleTransitions()
{
    
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
        
        // add costs to the transition
        trans->setCost(rule.getCosts());
       // status("setting costs %d for %s", rule.getCosts(), transName.c_str());

        cost_file_content += "sync_" + transName + " " + toString(rule.getCosts()) + ";\n";
        
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
        
        // get synchronouns labels for this rule.
        std::set< std::string > syncLabel;
        messageList = rule.getSyncList();
        messageIter = messageList.begin();
        while ( messageIter != messageList.end() )
        {
            // get label name and insert it into label list for transition
            std::string labelName = _rs.getMessageForId(*messageIter);
            syncLabel.insert(labelName);
            
            messageIter++;
        }
        
        // if we have a synchronous interface, create label for transition
        if (_contType == SYNCHRONOUS)
        {
            syncLabel.insert("sync_" + transName);
            trans->setSynchronizeLabels(syncLabel);
            
            // workaround for bug #14416 <https://gna.org/bugs/?14416>
            // \TODO: remove!
            std::set< std::string > labels (_engine->getSynchronousLabels());
            labels.insert("sync_" + transName);
            _engine->setSynchronousLabels(labels);
            
            status("known labels: %d", _engine->getSynchronousLabels().size());
        }
        // else create interface place to the controller
        else
        {
            std::string controlplacename = "control_" + transName;
            std::string observeplacename = "observe_" + transName;
            
            if (rule.getMode() == RuleSet::AdapterRule::AR_NORMAL 
                            || rule.getMode() == RuleSet::AdapterRule::AR_CONTROLLABLE)
            {
                Place & controlplace = _engine->createPlace( controlplacename, Node::INPUT, 0, _messageBound, "controller");
                _engine->createArc(controlplace, *trans);
            }
            
            if (rule.getMode() == RuleSet::AdapterRule::AR_NORMAL 
                            || rule.getMode() == RuleSet::AdapterRule::AR_OBSERVABLE)
            {
                Place & observeplace = _engine->createPlace( observeplacename, Node::OUTPUT, 0, _messageBound, "controller");
                _engine->createArc(*trans, observeplace);
            }

        }
        // next Tansition
        transNumber++;
        ruleIter++;
    }
    
}

/// \todo: was ist mit Plätzen, die Selfloop haben?
void Adapter::createComplementaryPlaces(pnapi::PetriNet & net)
{
    
    
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

        formula::FormulaEqual prop (formula::FormulaEqual(compPlace, place.getCapacity()));
        finalCond.addProposition(prop);

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
    
    
}

/// \todo: kann doch die API!?
void Adapter::removeUnnecessaryRules()
{
    

    using namespace pnapi;

    std::set<Place *> possibleDeadPlaces = _engine->getInternalPlaces();

    while ( !possibleDeadPlaces.empty() )
    {
        // find all place with an empty preset (so places are structually dead)

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
    
}

/*
void Adapter::findConflictFreeTransitions()
{
    
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

            // workaround for bug #14416 <https://gna.org/bugs/?14416>
            // \TODO: remove!
            std::set< std::string > labels (_engine->getSynchronousLabels());
            labels.insert("sync_" + transName);
            _engine->setSynchronousLabels(labels);
            
            status("known labels: %d", _engine->getSynchronousLabels().size());

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
    
}
*/

//! returns the name for the rule with index i
inline std::string Adapter::getRuleName(unsigned int i)
{
    
    return "rule_" + toString(i);
    
}

RuleSet::RuleSet() :
    _maxId(0)
{
    
    /* empty */
    
}

RuleSet::~RuleSet()
{
    
    std::list< AdapterRule * >::iterator rule = _adapterRules.begin();
    while (rule != _adapterRules.end())
    {
        delete *rule;
        
        rule++;
    }
    _adapterRules.clear();
    
}

void RuleSet::addRules(FILE * inputStream)
{
	
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
    fclose(adapt_rules_yyin);
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    adapt_rules_yylex_destroy();
#endif
//std::cout<<std::endl<<"I am here"<<std::endl;
    
}

const std::list< RuleSet::AdapterRule * > RuleSet::getRules() const
{
    
    
    return _adapterRules;
}

inline const std::string RuleSet::getMessageForId(const unsigned int id) const
{
    
    std::map< unsigned int, std::string >::const_iterator iter = _messageIndex.find(id);
    assert ( iter != _messageIndex.end() );

    
    return iter->second;
}

unsigned int RuleSet::getIdForMessage(std::string message)
{
    
    // getting the id once is enough
    unsigned int id = _messageId[message]; 
    if ( id == 0 )
    {
        ++_maxId;
        _messageIndex[_maxId] = message;
        
        return (_messageId[message] = _maxId);
    }
    else
    {
        
        return id;
    }
}

RuleSet::AdapterRule::AdapterRule(rulepair & rule, syncList & slist, cfMode modus, int costs) :
    _rule(rule), _syncList(slist), _modus(modus), _costs(costs)
{   
    
    /* empty */
    
}

RuleSet::AdapterRule::~AdapterRule()
{
    
    /* empty */
    
}

///inline 
//const RuleSet::AdapterRule::rulepair & RuleSet::AdapterRule::getRule() const 
//{
    
    
  //  return _rule; 
//}

//inline const RuleSet::AdapterRule::syncList & RuleSet::AdapterRule::getSyncList() const 
//{
    
    
  //  return _syncList; 
//}

inline const RuleSet::AdapterRule::cfMode & RuleSet::AdapterRule::getMode() const
{ 
    
    
    return _modus; 
}

inline const int & RuleSet::AdapterRule::getCosts() const
{ 
    
    
    return _costs; 
}

