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
#include <algorithm> // for min/max
#include <ctime>
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

#include "adapter.h"
#include "macros.h"
#include "helper.h"
#include "cmdline.h"
#include "Output.h"
#include "pnapi/pnapi.h"

/* For documentation of the following functions, please see header file. */

Adapter::Adapter(std::vector< pnapi::PetriNet *> & nets, RuleSet & rs,
                ControllerType contType, unsigned int messageBound, 
                bool useCompPlaces) :
    _engine(new pnapi::PetriNet), _controller(NULL), _nets(nets), _rs(rs), _contType(contType),
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
    delete (_engine);
    _engine = NULL;
    delete (_controller);
    _controller = NULL;
    FUNCOUT
}

const pnapi::PetriNet * Adapter::buildEngine()
{
    FUNCIN

    // create port to the controller
    _engine->createPort("controller");
    // create engine and transitions for the transformation rules
    createEngineInterface();
    createRuleTransitions();

    // adapter specific reduction should take place here
    removeUnnecessaryRules();
    //findConflictFreeTransitions();
    
    // reduce engine with standard PNAPI methods
    _engine->reduce(pnapi::PetriNet::LEVEL_3); // due to bug https://gna.org/bugs/?15820, normally 4
    
    // set final condition
    pnapi::Condition & finalCond = _engine->getFinalCondition();
    pnapi::Marking finalMarking (*_engine);
    finalCond.addMarking(finalMarking);

    // create complementary places for the engine's internal places
    if (_useCompPlaces)
    {
        Adapter::createComplementaryPlaces(*_engine);
    }
    else
    {
        status("skipping creation of complementary places once ..");
    }

    FUNCOUT
    return _engine;
}

const pnapi::PetriNet * Adapter::buildController()
{
    FUNCIN

    using namespace pnapi;

    // _engine should exist here 
    assert(_engine != NULL);

    // we make a copy of the engine, #_engine is needed for the result
    // #_enginecopy for the synthesis of the controller

    PetriNet composed (*_engine);

    // compose engine with nets
    for (unsigned int i = 0; i < _nets.size(); ++i)
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
        Condition & finalCond = composed.getFinalCondition();

        for (unsigned int i = 0; i < _nets.size(); ++i)
        {
            const std::set< Label *> & ifPlaces = _nets[i]->getInterface().getAsynchronousLabels();
            std::set< Label *>::const_iterator placeIter = ifPlaces.begin();
            while (placeIter != ifPlaces.end() )
            {
                // \todo: warum werden beide Platznamen gebraucht? wo geht hier beim Komponieren was schief?
                std::string placeName = (*placeIter)->getName();
                //std::string placeName2 = "net" + toString(i+1) + "." + (*placeIter)->getName();

                Place * place = composed.findPlace(placeName);
                //if (place == NULL)
                //{
                //    place = composed.findPlace(placeName2);
                //    placeName = placeName2;
                //}
                assert(place);

                Place * compPlace = &composed.createPlace("comp_" + placeName,
                     _messageBound +1, _messageBound+1);

                formula::FormulaEqual prop (formula::FormulaEqual(*compPlace,_messageBound + 1));
                finalCond.addProposition(prop);

                std::set< Node *> postSet = place->getPostset();
                std::set< Node *>::iterator nodeIter = postSet.begin();
                while (nodeIter != postSet.end() )
                {
                    composed.createArc(**nodeIter, *compPlace);
                    ++nodeIter;
                }

                std::set< Node *> preSet = place->getPreset();
                nodeIter = preSet.begin();
                while (nodeIter != preSet.end() )
                {
                    composed.createArc(*compPlace, **nodeIter);
                    ++nodeIter;
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
    composed.reduce(pnapi::PetriNet::LEVEL_3); // due to bug https://gna.org/bugs/?15820, normally 4

    if (_contType == ASYNCHRONOUS)
    {
        composed.normalize();
    }
    // \todo: Experiment, ob sich das hier noch lohnt.
    // composed.reduce(pnapi::PetriNet::LEVEL_4 || pnapi::PetriNet::KEEP_NORMAL);

    /***********************************\
        * calculate most permissive partner *
     \***********************************/
    // create a unique temporary file name
    Output owfn_temp;
    //        Output sa_temp;
    //       Output og_temp;

    std::string owfn_filename(owfn_temp.name());
    std::string sa_filename = owfn_filename + ".sa";
    std::string og_filename = owfn_filename + ".og";
    std::string cost_filename = owfn_filename + ".cf";


    owfn_temp.stream() << pnapi::io::owfn << composed << std::flush;

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
    std::ifstream sa_file(sa_filename.c_str(), std::ios_base::in);
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
    time(&start_time);
    delete _controller;

    status("Path to petrify %s", args_info.petrify_arg);
    status("Path to genet %s", args_info.genet_arg);

    if (args_info.sa2on_arg == sa2on_arg_petrify and std::string(args_info.petrify_arg) != "\"not found\"") // && _contType == ASYNCHRONOUS)

    {
        status("Using Petrify for conversion from SA to open net.");
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::PETRIFY);
        pnapi::PetriNet::setPetrify(args_info.petrify_arg);
        _controller = new pnapi::PetriNet(*mpp_sa);
    }
    else
    if (args_info.sa2on_arg == sa2on_arg_genet and std::string(args_info.genet_arg) != "\"not found\"") // && _contType == ASYNCHRONOUS)

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

    {
        // CG workaround
        status("Port work-around: adding all labels to port `controller'.");

        pnapi::Port & controllerport = _controller->createPort("controller");
        pnapi::Port & oldport = *(_controller->getInterface().getPort());
        const std::set<Label *> netIf = oldport.getAllLabels();
        for (std::set<Label *>::const_iterator label = netIf.begin(); label
                        != netIf.end(); ++label)
        {
            pnapi::Label::Type labeltype = (*label)->getType();
            const std::string labelname((*label)->getName());
            {
                // CG Workaround:
                pnapi::Label & labelcopy = controllerport.addLabel(labelname,
                    labeltype);
                const std::set<Transition *> & transen =
                                (*label)->getTransitions();
                for (std::set<Transition *>::const_iterator trans =
                                transen.begin(); trans != transen.end(); ++trans)
                {
                    (*trans)->addLabel(labelcopy);
                }
                oldport.removeLabel(labelname);
            }
        }
    }

    time(&end_time);

    if (args_info.verbose_flag)
    {
        std::cerr << PACKAGE << ": most-permissive partner: "
                        << pnapi::io::stat << *_controller << std::endl;
    }
    status("converting most-permissive partner done [%.0f sec]", difftime(
        end_time, start_time));

    FUNCOUT
    return _controller;
}
    
/**
 * \brief   creates the interface towards the involved services
 */
void Adapter::createEngineInterface()
{
    FUNCIN
    assert(_engine != NULL);
    
    pnapi::Port & contport = *(_engine->getInterface().getPort("controller"));

    for (unsigned int netindex = 0; netindex < _nets.size(); ++netindex)
    {
        using namespace pnapi;
        // renaming the net is done when merging the nets
        
        std::string portname = "net" + toString(netindex);

        pnapi::Port & netport = _engine->createPort(portname);

        // CG Workaround
        pnapi::Port & partnerport = _nets[netindex]->createPort(portname);
        pnapi::Port & oldport = *(_nets[netindex]->getInterface().getPort());

        const std::set< Label * > netIf = _nets[netindex]->getInterface().getAsynchronousLabels();
        for (std::set< Label *>::const_iterator place = netIf.begin(); place
                        != netIf.end(); ++place)
        {
            pnapi::Label::Type labeltype = (*place)->getType();
            const std::string labelname ((*place)->getName());
            {
                // CG Workaround:
                pnapi::Label & labelcopy = partnerport.addLabel(labelname, labeltype);
                const std::set< Transition  * > & transen = (*place)->getTransitions();
                for (std::set< Transition * >::const_iterator trans = transen.begin(); trans != transen.end(); ++trans)
                {
                    (*trans)->addLabel(labelcopy);
                }
                oldport.removeLabel(labelname);
            }

            // get interface name
            //const std::string & ifname = (*place)->getName();
            // internal name
            std::string internalname;
            if (args_info.withprefix_flag)
            {
                internalname  = (labelname.substr(labelname.find_first_of(".") + 1) + "_int");
            }
            else
            {
                internalname  = (labelname + "_int");
            }
            
            Label * iflabel = NULL;
            if (labeltype == Label::INPUT) {
                iflabel = &netport.addOutputLabel(labelname);
            }
            else {
                iflabel = &netport.addInputLabel(labelname);
            }
            
            Place * intplace = _engine->findPlace(internalname);
            if (intplace == NULL)
            {
                //! the internal place which is a copy of the interface place
                intplace = &_engine->createPlace(internalname,
                                0, _messageBound);
                
            }
            //! name of sending/receiving transition
            std::string inttransname(( (labeltype == Label::INPUT ) ? "t_send_"
                            : "t_receive_") + labelname);
            
            //! transition which moves the token between the interface place and its copy
            Transition & inttrans =_engine->createTransition( inttransname );
            
            // create arcs between the internal place, the transition 
            // and the interface place
            if (labeltype == Label::INPUT)
            {
                _engine->createArc(*intplace, inttrans);
                // _engine->createArc(inttrans, ifplace);
            }
            else
            {
                // _engine->createArc(ifplace, inttrans);
                _engine->createArc(inttrans, *intplace);
            }
            inttrans.addLabel(*iflabel);

            // if we have a synchronous interface, create label for transition
            if (_contType == SYNCHRONOUS)
            {
                Label & contLabel = contport.addSynchronousLabel("sync_" + inttransname);
                inttrans.addLabel(contLabel);

                cost_file_content += "sync_" + inttransname + " 0;\n";
                
            }
            // else create interface place to the controller
            else
            {
                if (labeltype == Label::INPUT) {
                    Label & inputLabel = contport.addInputLabel("send_" + labelname);
                    inttrans.addLabel(inputLabel);
                }
                else {
                    Label & outputLabel = contport.addOutputLabel("receive_" + labelname);
                    inttrans.addLabel(outputLabel);
                }
            }
        }
        std::set< Label * > ifLabels(_nets[netindex]->getInterface().getSynchronousLabels());
        for (std::set< Label * >::const_iterator iter = ifLabels.begin(); iter != ifLabels.end(); ++iter)
        {
            const std::string labelname ((*iter)->getName());
            netport.addSynchronousLabel(labelname);

            // CG Workaround:
            pnapi::Label & labelcopy = partnerport.addLabel(labelname,
                pnapi::Label::SYNCHRONOUS);
            const std::set<Transition *> & transen = (*iter)->getTransitions();
            for (std::set<Transition *>::const_iterator trans = transen.begin(); trans
                            != transen.end(); ++trans)
            {
                (*trans)->addLabel(labelcopy);
            }
            oldport.removeLabel(labelname);
        }
        
    }
    FUNCOUT
}

void Adapter::createRuleTransitions()
{
    FUNCIN
    using namespace pnapi;
    
    // we need the port to the controller for adding control labels
    pnapi::Port & contport = *(_engine->getInterface().getPort("controller"));

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
                place = &_engine->createPlace(placeName, 0, _messageBound);
            }
            
            // create arc between this place and rule transition
            _engine->createArc(*place, *trans);
            
            ++messageIter;
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
                place = &_engine->createPlace(placeName, 0, _messageBound);
            }
            
            // create arc between this place and rule transition
            _engine->createArc(*trans, *place);
            
            ++messageIter;
        }
        
        // get synchronouns labels for this rule.
        std::set< std::string > syncLabel;
        messageList = rule.getSyncList();
        messageIter = messageList.begin();
        while ( messageIter != messageList.end() )
        {
            // get label name and insert it into label list for transition
            std::string labelName = _rs.getMessageForId(*messageIter);
            pnapi::Label * syncLabel = _engine->getInterface().findLabel(labelName);
            if (syncLabel != NULL)
            {
                trans->addLabel(*syncLabel);
            }
            else // This transition will never fire, so a dead place is added!
            {
                Place * deadplace = _engine->findPlace(labelName);
                if (deadplace == NULL)
                {
                    deadplace = &_engine->createPlace(labelName);
                }
                _engine->createArc(*deadplace, *trans);
            }
            
            ++messageIter;
        }
        
        // if we have a synchronous interface, create label for transition
        if (_contType == SYNCHRONOUS)
        {
            Label & synclabel = contport.addSynchronousLabel("sync_" + transName);
            trans->addLabel(synclabel);
        }
        // else create interface place to the controller
        else
        {
            if (rule.getMode() == RuleSet::AdapterRule::AR_NORMAL 
                            || rule.getMode() == RuleSet::AdapterRule::AR_CONTROLLABLE)
            {
                Label & inputlabel = contport.addInputLabel("control_" + transName);
                trans->addLabel(inputlabel);
            }
            
            if (rule.getMode() == RuleSet::AdapterRule::AR_NORMAL 
                            || rule.getMode() == RuleSet::AdapterRule::AR_OBSERVABLE)
            {
                Label & outputlabel = contport.addOutputLabel("observe_" + transName);
                trans->addLabel(outputlabel);
            }

        }
        // next Tansition
        ++transNumber;
        ++ruleIter;
    }
    FUNCOUT
}

/// \todo: was ist mit Plätzen, die Selfloop haben?
void Adapter::createComplementaryPlaces(pnapi::PetriNet & net)
{
    FUNCIN
    
    using namespace pnapi;
    
    std::set< Place * > intPlaces = net.getPlaces();
    std::set< Place * >::iterator placeIter = intPlaces.begin();
    
    while ( placeIter != intPlaces.end() )
    {
        Place & place = **placeIter;
        Place & compPlace = net.createPlace("comp_" + place.getName(),
            place.getCapacity(), place.getCapacity());
        
        // update final condition
        Condition & finalCond = net.getFinalCondition();

        formula::FormulaEqual prop (formula::FormulaEqual(compPlace, place.getCapacity()));
        finalCond.addProposition(prop);

        std::set< Node * > preSet = place.getPreset();
        std::set< Node * >::iterator nodeIter = preSet.begin();
        while ( nodeIter != preSet.end() )
        {
            Arc * a = net.findArc(**nodeIter, place);
            int x = (a != NULL)?a->getWeight():0;
            Arc * b = net.findArc(place, **nodeIter);
            int y = (b != NULL)?b->getWeight():0;
            unsigned int weight = std::max(x - y,0);
            if (weight > 0) net.createArc(compPlace, **nodeIter, weight);
            ++nodeIter;
        }
        
        std::set< Node * > postSet = place.getPostset();
        nodeIter = postSet.begin();
        while ( nodeIter != postSet.end() )
        {
            Arc * a = net.findArc(**nodeIter, place);
            int x = (a != NULL)?a->getWeight():0;
            Arc * b = net.findArc(place, **nodeIter);
            int y = (b != NULL)?b->getWeight():0;
            unsigned int weight = std::max(y - x,0);
            if (weight > 0) net.createArc(**nodeIter, compPlace, weight);
            ++nodeIter;
        }
        
        ++placeIter;
    }
    
    FUNCOUT
}

/// \todo: kann doch die API!?
void Adapter::removeUnnecessaryRules()
{
    FUNCIN

    using namespace pnapi;

    std::set<Place *> possibleDeadPlaces = _engine->getPlaces();

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
            ++placeIter;
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
//                    if ( (*cand)->getType() == Label::INTERNAL )
                    {
                        possibleDeadPlaces.insert( dynamic_cast<Place*>(*cand) );
                    }
                    ++cand;
                }
                

                _engine->deleteTransition(*(dynamic_cast<Transition*>(*nodeIter)));
                // deleting adjacent interface places, if they exist

                ++nodeIter;
            }

            //status("Deleting place %s.", p->getName().c_str());
            _engine->deletePlace(*p);

            ++place2Delete;
        }
    }
    FUNCOUT
}

/*
void Adapter::findConflictFreeTransitions()
{
    FUNCIN
    using namespace pnapi;

    for (unsigned int i = 1; i <= _rs.getRules().size(); ++i)
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
                    ++placeIter;
                }
            }
        }
    }
    FUNCOUT
}
*/

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
        
        ++rule;
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
    fclose(adapt_rules_yyin);
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    adapt_rules_yylex_destroy();
#endif

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
    assert ( iter != _messageIndex.end() );

    FUNCOUT
    return iter->second;
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

RuleSet::AdapterRule::AdapterRule(rulepair & rule, syncList & slist, cfMode modus, int costs) :
    _rule(rule), _syncList(slist), _modus(modus), _costs(costs)
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

inline const RuleSet::AdapterRule::syncList & RuleSet::AdapterRule::getSyncList() const 
{
    FUNCIN
    FUNCOUT
    return _syncList; 
}

inline const RuleSet::AdapterRule::cfMode & RuleSet::AdapterRule::getMode() const
{ 
    FUNCIN
    FUNCOUT
    return _modus; 
}

inline const int & RuleSet::AdapterRule::getCosts() const
{ 
    FUNCIN
    FUNCOUT
    return _costs; 
}

