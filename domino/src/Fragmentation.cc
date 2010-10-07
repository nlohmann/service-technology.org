/*****************************************************************************\
 			 _____                  _             
			|  __ \                (_)            
			| |  | | ___  _ __ ___  _ _ __   ___  
			| |  | |/ _ \| '_ ` _ \| | '_ \ / _ \ 
			| |__| | (_) | | | | | | | | | | (_) |
			|_____/ \___/|_| |_| |_|_|_| |_|\___/ 		                          
			DecOMposition of busINess wOrkflows (into services)
			http://service-technology.org/domino

 Copyright (c) 2010 Andreas Lehmann

 Domino is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Domino is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Domino.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Fragmentation.h"

Fragmentation::~Fragmentation() {}

Fragmentation::Fragmentation(pnapi::PetriNet &Petrinet) {
	this->ROLE_UNASSIGNED = -1;	
	this->PLACE_UNASSIGNED = "";
	this->GLOBALSTART_REACHED = "AL_1983_07_12";
	this->SERVICE_PLACE_PREFIX = "sp_";
	this->SERVICE_TRANSITION_PREFIX = "st_";
	
	this->mCurrentDianeForces = 0;
	this->mOverallDianeForces = 0;
	this->mCurrentDianeAlternatives = 0;
	this->mOverallDianeAlternatives = 0;

	this->mLolaCalls = 0;
	this->mDianeCalls = 0;
	this->mIsFreeChoice = Petrinet.isFreeChoice();
	this->mHasCycles = false;
	

	this->mFragmentNet = NULL;
	this->mNet = &Petrinet;
	this->mMaxFragID = this->mNet->getTransitions().size();
	this->mGlobalStartPlace = PLACE_UNASSIGNED;
	
	this->init();
	
	this->mRoleFragmentsBuild = false;
	this->mUnassignedFragmentsProcessed = false;
	this->mServicesCreated = false;

}

role_id_t Fragmentation::getRoleID(const string & Role) {
	roleName2RoleID_t::iterator curRole;

	curRole = this->mRoleName2RoleID.find(Role);
	if (curRole == this->mRoleName2RoleID.end()) {
		abort(9, "Fragmentation::getRoleID(%s) failed", Role.c_str());
	}

	return curRole->second;
}

void Fragmentation::init() {
	status("init() called...");

	set<std::string> roles;
	places_t places;
	transitions_t transitions;
	pnapi::Place * place;
	pnapi::Transition * transition;
	size_t i;

	this->initColors();
	this->mMaxFragID = this->mNet->getTransitions().size();
	
	status("..caching roles");
	roles = this->mNet->getRoles();
	i = 0;
	FOREACH(r, roles) {
		status("....%s -> %i", (*r).c_str(), i);
		this->mRoleID2RoleName[i] = (*r);
		this->mRoleName2RoleID[(*r)] = i++;
	}
	status("..roles cached");

	status("..caching places");
	places = this->getPlaces(*this->mNet);
	PNAPI_FOREACH(p, places) {
		status("....%s", (*p).c_str());
		if (*p == GLOBALSTART_REACHED) {
			abort(10, "Fragmentation::init(): place %s has predefined constant name", (*p).c_str());
		}
		if (this->getPlacePreset(*this->mNet, *p).size() == 0) {
			status("......is global start place");
			if (this->mGlobalStartPlace != PLACE_UNASSIGNED) {
				abort(11, "Fragmentation::init(): another global start place found");
			}
			this->mGlobalStartPlace = *p;
		}
		this->mPlaces.insert(*p);
		place = this->mNet->findPlace(*p);
		this->mPlaceName2PlacePointer[*p] = place;
		this->mPlacePointer2PlaceName[place] = *p;
	}
	if (this->mGlobalStartPlace == PLACE_UNASSIGNED) {
		abort(11, "Fragmentation::init(): no global start place was found");
	}
	status("..places cached");

	status("..caching transitions");
	transitions = this->getTransitions(*this->mNet);
	PNAPI_FOREACH(t, transitions) {
		status("....%s", (*t).c_str());
		this->mTransitions.insert(*t);
		transition = this->mNet->findTransition(*t);
		this->mTransitionName2TransitionPointer[*t] = transition;
		this->mTransitionPointer2TransitionName[transition] = *t;
	}
	status("..transitions cached");

	status("init() finished");
}

void Fragmentation::buildServices() {
	if (!this->mUnassignedFragmentsProcessed) {
		abort(8, "buildServices(): processUnassignedFragments() is necessary");
	}

	if (mServicesCreated) {
		abort(6, "buildServices(): already called");
	}

	status("buildServices() called...");

	place_t newPlace;
	place_t minPlace;
	place_t maxPlace;
	places_t transitionPlaces;
	transition_t newTransition;
	transition_t curPredecessor;
	transitions_t netTransitions;
	transitions_t startTransitions;
	transitions_t restrictions;
	transitions_t transitionsDone;
	stack<transition_t> transitionsToDo;
	set< pair<transition_t, transition_t> > predecessors;
	set< pair<transition_t, transition_t> > reactivatings;
	set< pair<frag_id_t, frag_id_t> > processedFragments;
	pair<frag_id_t, frag_id_t> curFragmentPair;
	role_id_t transitionRoleID;
	frag_id_t transitionFragID;
	frag_id_t minimum;
	frag_id_t maximum;
	map<place_t, unsigned int>::const_iterator curPlaceMaxToken;
	size_t reactivatingCount;
	unsigned int curPlaceTokens;
	bool changed;
	Tarjan tarjan(*this->mNet);

	tarjan.calculateSCC();
	this->mHasCycles = tarjan.hasNonTrivialSCC();

	status(_cimportant_("..has non trivial SCC: %d"), this->mHasCycles);
	status(_cimportant_("..is free choice: %d"), this->mIsFreeChoice);

	netTransitions = this->getTransitions(*this->mNet);
	FOREACH(t, netTransitions) {
		if (this->isStartTransition(*t)) {
			status("..handle %s", (*t).c_str());
			startTransitions.insert(*t);
			transitionRoleID = this->getTransitionRoleID(*t);
			transitionFragID = this->getTransitionFragID(*t);
			reactivatingCount = 0;

			transitionsDone.clear();
			this->addTransitionPredecessors(transitionsToDo, transitionsDone, *t);
			while (!transitionsToDo.empty()) {
				curPredecessor = transitionsToDo.top();
				transitionsToDo.pop();

				status("....handle %s", curPredecessor.c_str());

				if (curPredecessor == this->GLOBALSTART_REACHED) {
					status("......add to restrictions");
					restrictions.insert(*t);
				}
				//else if ( (this->getTransitionRoleID(curPredecessor) == transitionRoleID) && (this->getTransitionFragID(curPredecessor) != transitionFragID) && (!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(curPredecessor))) ) {
				else if ( (this->getTransitionRoleID(curPredecessor) == transitionRoleID) && (!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(curPredecessor))) ) {
					status("......add to predecessors");
					predecessors.insert( std::make_pair(*t, curPredecessor) );
				}
				else if ( (this->getTransitionRoleID(curPredecessor) == transitionRoleID) && (tarjan.getNodeSCC(curPredecessor) == tarjan.getNodeSCC(*t))  ) {
					status("......add to reactiviating");
					reactivatings.insert( std::make_pair(*t, curPredecessor) );
					reactivatingCount++;
				}
				else if (curPredecessor == *t) {
					abort(9, "Fragmentation::buildServices(): curPredecessor == *t");
				}
				else {
					this->addTransitionPredecessors(transitionsToDo, transitionsDone, curPredecessor);
				}

			}

			if ( (tarjan.isNonTrivialSCC(tarjan.getNodeSCC(*t))) && (reactivatingCount == 0) ) {
				abort(9, "Fragmentation::buildServices(): no reactiviating transitions");
			}

		}
	}

	if ((restrictions.size() != 0) && !this->mIsFreeChoice) {
		extern int graph_yyparse();
		extern int graph_yylex_destroy();
		extern FILE* graph_yyin;

		// define variables
		time_t start_time, end_time;
		string loutputParam;
		string lfileName;

		loutputParam += " --marking=";
		
		    // create a temporary file
#if defined(__MINGW32__)
		lfileName = mktemp(basename(args_info.tmpfile_arg));
#else
		lfileName = mktemp(args_info.tmpfile_arg);
#endif
		loutputParam += lfileName;

		// select LoLA binary and build LoLA command
#if defined(__MINGW32__)
		// MinGW does not understand pathnames with "/", so we use the basename
		string command_line(basename(args_info.lola_arg));
#else
		string command_line(args_info.lola_arg);
#endif

		command_line += loutputParam;
		// call LoLA
		this->mLolaCalls++;
		status("..creating a pipe to %s by calling '%s'", _ctool_("LoLA"), _cparameter_(command_line));

		{
		    // set start time
		    time(&start_time);
		    // create stringstream to store the open net
		    std::stringstream ss;
		    ss << pnapi::io::lola << *this->mNet << std::flush;
		    // call LoLA and open a pipe
		    FILE* fp = popen(command_line.c_str(), "w");
		    // send the net to LoLA
		    fprintf(fp, "%s", ss.str().c_str());
		    // close the pipe
		    pclose(fp);
		    // set end time
		    time(&end_time);
		}

		// status message
		status("..%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));

        // open LoLA's output file and link output file pointer
        graph_yyin = fopen(lfileName.c_str(), "r");
        if (!graph_yyin) {
			abort(3, "Fragmentation::buildServices(): failed to open file %s", _cfilename_(lfileName));
        }

        /// actual parsing
        graph_yyparse();

        // close input (output is closed by destructor)
        fclose(graph_yyin);

        /// clean lexer memory
        graph_yylex_destroy();

		if (!args_info.noClean_flag) {
			if (remove(lfileName.c_str()) != 0) {
				abort(12, "Fragmentation::buildServices(): file %s cannot be deleted", _cfilename_(lfileName));
			}
		}

	}

	if (startTransitions.size() != 0) {
		status("..create places:");
		 FOREACH(t, startTransitions) {
			newPlace = this->SERVICE_PLACE_PREFIX + *t;
			status("....%s", newPlace.c_str());
			this->createPlace(newPlace, this->getTransitionFragID(*t), this->getTransitionRoleID(*t));
			this->createArc(newPlace, *t);
		}
	}

	if (restrictions.size() != 0) {
		status("..restrictions:");
		if (!this->mIsFreeChoice) {
			FOREACH(t, restrictions) {
				newPlace = this->SERVICE_PLACE_PREFIX + *t;
				status("....%s", newPlace.c_str());

				curPlaceTokens = 0;
				transitionPlaces = this->getTransitionPreset(*this->mNet, *t);
				FOREACH(p, transitionPlaces) {
					curPlaceMaxToken = Place2MaxTokens.find(*p);
					if (curPlaceMaxToken == Place2MaxTokens.end()) {
						//abort(2, "Fragmenation::buildServices(): %s is unknown in Place2MaxTokens", (*p).c_str());
					}
					if (curPlaceMaxToken->second > curPlaceTokens) {
						curPlaceTokens = curPlaceMaxToken->second;
					}
				}

				if (curPlaceTokens == 0) {
					abort(9, "Fragmenation::buildServices(): no boundness found for %s", (*t).c_str());
				}

				this->mPlaceName2PlacePointer[newPlace]->setTokenCount(curPlaceTokens);
			}
		}
		else {
			FOREACH(t, restrictions) {
				newPlace = this->SERVICE_PLACE_PREFIX + *t;
				status("....%s", newPlace.c_str());
				this->mPlaceName2PlacePointer[newPlace]->setTokenCount(1);
			}
		}
	}

	if (predecessors.size() != 0) {
		status("..predecessors:");
		FOREACH(p, predecessors) {
			newPlace = this->SERVICE_PLACE_PREFIX + (*p).first;
			status("....%s --> %s", (*p).second.c_str(), (*p).first.c_str());
			this->createArc((*p).second, newPlace);
			//this->createArc(newPlace, (*p).first);		already known...
			this->addPlaceFragID(newPlace, this->getTransitionFragID((*p).second));
		}
	}

	if (tarjan.hasNonTrivialSCC()) {
		status("..reactivatings:");
		FOREACH(p, reactivatings) {
			newPlace = this->SERVICE_PLACE_PREFIX + (*p).first;
			status("....%s --> %s", (*p).second.c_str(), (*p).first.c_str());
			this->createArc((*p).second, newPlace);
			//this->createArc(newPlace, (*p).first);		already known...
			this->addPlaceFragID(newPlace, this->getTransitionFragID((*p).second));
		}
	}

	this->connectFragments();

	changed = false;
	status("..connecting unconnected fragments");
	for (frag_id_t f_a = 0; f_a < this->mMaxFragID; f_a++) {
		for (frag_id_t f_b = 0; f_b < this->mMaxFragID; f_b++) {
			if ( (f_a != f_b) && (!this->isFragmentEmpty(f_a) && !this->isFragmentEmpty(f_b)) && (this->getFragmentRoleID(f_a) == this->getFragmentRoleID(f_b)) ) {
				minimum = std::min(f_a, f_b);
				maximum = std::max(f_a, f_b);
				curFragmentPair = std::make_pair(minimum, maximum);
				if (processedFragments.find(curFragmentPair) == processedFragments.end()) {
					status(_cwarning_("....connection (%d,%d) necessary"), minimum, maximum);

					changed = true;

					std::stringstream curMinimum;
					std::stringstream curMaximum;
					curMinimum << minimum;
					curMaximum << maximum;
					newTransition = this->SERVICE_TRANSITION_PREFIX + "_dead_" + curMinimum.str() + "_" + curMaximum.str();
					newPlace = this->SERVICE_PLACE_PREFIX + this->SERVICE_TRANSITION_PREFIX + curMinimum.str() + "_" + curMaximum.str();

					this->createTransition(newTransition, minimum, this->getFragmentRoleID(minimum));
					this->createPlace(newPlace, minimum, this->getFragmentRoleID(minimum));
					this->createArc(newPlace, newTransition);

					FOREACH(t, startTransitions) {
						if ((this->getTransitionRoleID(*t) == this->getFragmentRoleID(minimum)) && (this->getTransitionFragID(*t) == minimum)) {
							minPlace = this->SERVICE_PLACE_PREFIX + *t;
						}
						if ((this->getTransitionRoleID(*t) == this->getFragmentRoleID(maximum)) && (this->getTransitionFragID(*t) == maximum)) {
							maxPlace = this->SERVICE_PLACE_PREFIX + *t;
						}
					}

					this->createArc(newTransition, minPlace);
					this->createArc(newTransition, maxPlace);

					this->addPlaceFragID(maxPlace, minimum);

					processedFragments.insert(curFragmentPair);
				}
			}
		}
	}

	if (changed) {
		this->connectFragments();
	}

	this->mServicesCreated = true;

	status("buildServices() finished");
}

void Fragmentation::addTransitionPredecessors(stack<transition_t> & ToDo, transitions_t & Done, const transition_t & Transition) {
	places_t places;
	transitions_t transitions;

	places = this->getTransitionPreset(*this->mNet, Transition);
	FOREACH(p, places) {
		if (*p == this->mGlobalStartPlace) {
			if (Done.find(this->GLOBALSTART_REACHED) == Done.end()) {
				ToDo.push(this->GLOBALSTART_REACHED);
				Done.insert(this->GLOBALSTART_REACHED);
			}
		}
		else {
			transitions = this->getPlacePreset(*this->mNet, *p);
			FOREACH(t, transitions) {
				if (Done.find(*t) == Done.end()) {
					ToDo.push(*t);
					Done.insert(*t);
				}
			}
		}
	}
}

void Fragmentation::processUnassignedFragments() {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::processUnassignedFragments(): buildRoleFragments() is necessary");
	}

	if (mUnassignedFragmentsProcessed) {
		abort(6, "Fragmentation::processUnassignedFragments(): already called");
	}

	status("processUnassignedFragments() called...");

	for (frag_id_t FragID = 0; FragID <= this->mMaxFragID; FragID++) {
		if (!this->isFragmentEmpty(FragID)) {
			if (this->getFragmentRoleID(FragID) == this->ROLE_UNASSIGNED) {
				processUnassignedFragment(FragID);
			}
		}
	}

	this->mUnassignedFragmentsProcessed = true;
	
	if (this->mOverallDianeForces != 0) {
		status(_cimportant_("..%d forced decisions"), this->mOverallDianeForces);
		status(_cimportant_("....with %d alternatives"), this->mOverallDianeAlternatives);
	}

	status("processUnassignedFragments() finished");
}

void Fragmentation::buildRoleFragments() {
	if (this->mRoleFragmentsBuild) {
		abort(6, "Fragmentation::buildRoleFragments(): already called");
	}
	
	status("buildRoleFragments() called...");

	frag_id_t curFragID;
	frag_id_t curMinFragID;
	frag_id_t minFragID;
	frag_id_t transitionFragID;
	role_id_t curRoleID;
	set<std::string> transitionRoles;
	places_t transitionPlaces;
	places_t placesToDo;
	transitions_t placeTransitions;
	pnapi::Transition *transition;
	pair<minFragID2Places_t::iterator, minFragID2Places_t::iterator> curPlaces;
	

	status("..initiating places");
	PNAPI_FOREACH(p, this->mPlaces) {
		this->setPlaceMinFragID(*p, this->mMaxFragID);
	}
	status("..places initiated");

	status("..assigning roles to nodes");
	curFragID = 0;
	PNAPI_FOREACH(t, this->mTransitions) {
		status("....processing %s", (*t).c_str());		
		transition = this->mTransitionName2TransitionPointer[*t];

		//assign role to t
		if (transition->getRoles().size() == 0) {
			status("......t has no role(s) assigned");
			curRoleID = this->ROLE_UNASSIGNED;
		}
		else if (transition->getRoles().size() == 1) {
			transitionRoles = transition->getRoles();
			curRoleID = this->mRoleName2RoleID.find(*(transitionRoles.begin()))->second;
			status("......t has role %s [%d]", (*(transitionRoles.begin())).c_str(), curRoleID);
		}
		else {
			abort(13, "Fragmentation::buildRoleFragments(): transition %s has more than one role", (*t).c_str());
		}
		this->setTransitionRoleID(*t, curRoleID, false);
		status("......assigned RoleID %d to t", curRoleID);
		this->setTransitionFragID(*t, curFragID, false);
		status("......set FragID %d to t", curFragID);

		//processing places connected with t
		status("......processing places connected with t");
		curMinFragID = -1;
		transitionPlaces = this->getTransitionPlaces(*this->mNet, *t);
		PNAPI_FOREACH(p, transitionPlaces) {
			this->addPlaceRoleID(*p, curRoleID);
			if (this->isSharedPlace(*p)) {
				status("........place %s is SP", (*p).c_str());
				this->setPlaceMinFragID(*p, this->mMaxFragID);				
			}
			else {
				curMinFragID = this->getPlaceMinFragID(*p);				
				this->setPlaceMinFragID(*p, std::min(curMinFragID, curFragID));
			}
		}
		status("......places connected with t processed");
		
		curFragID++;
		status("....transition %s processed", (*t).c_str());
	}
	status("..roles assigned to nodes");

	status("..refactoring fragments");
	for (curFragID = 0; curFragID < this->mMaxFragID; curFragID++) {
		status("....processing fragment %d", curFragID);
		minFragID = curFragID;
		//for all places of curFragID
		curPlaces = this->mMinFragID2Places.equal_range(curFragID);
		for (minFragID2Places_t::iterator curPlace=curPlaces.first; curPlace!=curPlaces.second; ++curPlace) {
			//p = curPlace->second
			//for all transitions connected with p
			placeTransitions = this->getPlaceTransitions(*this->mNet, curPlace->second);			
			PNAPI_FOREACH(t, placeTransitions) {
				minFragID = std::min(minFragID, this->getTransitionFragID(*t));
			}
		}

		//for all places of curFragID
		placesToDo.clear();
		curPlaces = this->mMinFragID2Places.equal_range(curFragID);
		for (minFragID2Places_t::const_iterator curPlace=curPlaces.first; curPlace!=curPlaces.second; ++curPlace) {
			placesToDo.insert(curPlace->second);
		}

		FOREACH(p, placesToDo) {
			this->setPlaceMinFragID(*p, minFragID);
			//for all transitions connected with p
			placeTransitions = this->getPlaceTransitions(*this->mNet, *p);		
			PNAPI_FOREACH(t, placeTransitions) {
				transitionFragID = this->getTransitionFragID(*t);			
				if (transitionFragID < curFragID) {
					this->replaceFragIDs(transitionFragID, curFragID);
				}
				else {
					this->setTransitionFragID(*t, minFragID);
				}	
			}
		}
		status("....fragment %d processed", curFragID);
	}
	status("..fragments refactored");

	status("..creating fragments");
	PNAPI_FOREACH(t, this->mTransitions) {
		status("....processing %s", (*t).c_str());
		transitionPlaces = this->getTransitionPlaces(*this->mNet, *t);
		PNAPI_FOREACH(p, transitionPlaces) {
			this->addPlaceFragID(*p, this->getTransitionFragID(*t));
		}
		status("....%s processed", (*t).c_str());
	}
	status("..fragments created");

	status("..connecting fragments");
	this->connectFragments();
	status("..fragments connected");

	this->mRoleFragmentsBuild = true;

	status("buildRoleFragments() finished");
}


void Fragmentation::setTransitionRoleID(const transition_t & Transition, const role_id_t RoleID, const bool Override) {
	transition2RoleID_t::iterator curRole;
	roleID2Transitions_t::iterator curRoleTransition;
	pair<roleID2Transitions_t::iterator, roleID2Transitions_t::iterator> roleTransitions;

	curRole = this->mTransition2RoleID.find(Transition);
	if (curRole != this->mTransition2RoleID.end()) {
		if (!Override) {
			abort(6, "Fragmentation::setTransitionRoleID(%s, %d) is called again", Transition.c_str(), RoleID);
		}
		bool found = false;
		
		roleTransitions = this->mRoleID2Transitions.equal_range(curRole->second);
		for (curRoleTransition=roleTransitions.first; curRoleTransition!=roleTransitions.second; ++curRoleTransition) {
			if (curRoleTransition->second == Transition) {
				found = true;
				break;
			}
		}
		if (!found) {
			abort(9, "Fragmentation::setTransitionRoleID(%s, %d) failed", Transition.c_str(), RoleID);
		}
		this->mRoleID2Transitions.erase(curRoleTransition);		
		this->mTransition2RoleID.erase(curRole);
	}
	this->mTransition2RoleID.insert( std::make_pair(Transition, RoleID) );
	this->mRoleID2Transitions.insert( std::make_pair(RoleID, Transition) );
}

role_id_t Fragmentation::getTransitionRoleID(const transition_t & Transition) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::getTransitionRoleID(): buildRoleFragments() is necessary");
	}
	
	transition2RoleID_t::iterator curRole;

	curRole = this->mTransition2RoleID.find(Transition);
	if (curRole == this->mTransition2RoleID.end()) {
		abort(9, "Fragmentation::getTransitionRoleID(%s) failed", Transition.c_str());
	}
	return curRole->second;
}

void Fragmentation::setTransitionFragID(const transition_t & Transition, const frag_id_t FragID, const bool Override) {
	transition2FragID_t::iterator curTransition;
	fragID2Transitions_t::iterator curFragTransition;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;

	curTransition = this->mTransition2FragID.find(Transition);
	if (curTransition != this->mTransition2FragID.end()) {
		if (!Override) {
			abort(6, "Fragmentation::setTransitionFragID(%s, %d) is called again", Transition.c_str(), FragID);
		}
		bool found = false;
			
		curFragTransitions = this->mFragID2Transitions.equal_range(curTransition->second);		
		for (curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
			if (curFragTransition->second == Transition) {
				found = true;
				break;
			}
		}
		if (!found) {
			abort(9, "Fragmentation::setTransitionFragID(%s, %d) failed", Transition.c_str(), FragID);
		}
		this->mFragID2Transitions.erase(curFragTransition);		
		this->mTransition2FragID.erase(curTransition);
	}
	this->mTransition2FragID.insert( std::make_pair(Transition, FragID) );
	this->mFragID2Transitions.insert( std::make_pair(FragID, Transition) );
}

frag_id_t Fragmentation::getTransitionFragID(const transition_t & Transition) {
	transition2FragID_t::iterator curFrag;

	curFrag = this->mTransition2FragID.find(Transition);
	if (curFrag == this->mTransition2FragID.end()) {
		abort(9, "Fragmentation::getTransitionFragID(%s) failed", Transition.c_str());
	}
	return curFrag->second;
}

void Fragmentation::setPlaceMinFragID(const place_t & Place, const frag_id_t FragID) {
	place2MinFragID_t::iterator curMinFrag;
	minFragID2Places_t::iterator curFragPlace;
	pair<minFragID2Places_t::iterator, minFragID2Places_t::iterator> curFragPlaces;

	curMinFrag = this->mPlace2MinFragID.find(Place);
	if (curMinFrag != this->mPlace2MinFragID.end()) {
		bool found = false;
		
		curFragPlaces = this->mMinFragID2Places.equal_range(curMinFrag->second);
		for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {	
			if (curFragPlace->second == Place) {
				found = true;
				break;
			}
		}
		if (!found) {
			abort(9, "Fragmentation::setPlaceMinFragID(%s, %d) failed", Place.c_str(), FragID);
		}	
		this->mMinFragID2Places.erase(curFragPlace);
		this->mPlace2MinFragID.erase(curMinFrag);
	}
	this->mPlace2MinFragID.insert( std::make_pair(Place, FragID) );
	this->mMinFragID2Places.insert( std::make_pair(FragID, Place) );
}

frag_id_t Fragmentation::getPlaceMinFragID(const place_t & Place) {
	place2MinFragID_t::iterator curMinFrag;

	curMinFrag = this->mPlace2MinFragID.find(Place);
	if (curMinFrag == this->mPlace2MinFragID.end()) {
		abort(9, "Fragmentation::getPlaceMinFragID(%s) failed", Place.c_str());
	}
	return curMinFrag->second;
}

void Fragmentation::addPlaceRoleID(const place_t & Place, const role_id_t RoleID) {
	pair<place2RoleIDs_t::iterator, place2RoleIDs_t::iterator> curPlaceRoles;
	pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces;
	bool foundA;
	bool foundB;
	
	foundA = false;
	curPlaceRoles = this->mPlace2RoleIDs.equal_range(Place);
	for (place2RoleIDs_t::iterator curPlaceRole=curPlaceRoles.first; curPlaceRole!=curPlaceRoles.second; ++curPlaceRole) {
		if (curPlaceRole->second == RoleID) {
			foundA = true;
			break;
		}
	}
	
	foundB = false;
	curRolePlaces = this->mRoleID2Places.equal_range(RoleID);
	for (roleID2Places_t::iterator curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
		if (curRolePlace->second == Place) {
			foundB = true;
			break;
		}
	}

	if (!(foundA && foundB)) {
		this->mPlace2RoleIDs.insert( std::make_pair(Place, RoleID) );
		this->mRoleID2Places.insert( std::make_pair(RoleID, Place) );
	}
	else if (foundA && !foundB) {
		abort(9, "Fragmentation::addPlaceRoleID(%s, %d) is in mPlace2RoleIDs but not in mRoleID2Places", Place.c_str(), RoleID);
	}
	else if (!foundA && foundB) {
		abort(9, "Fragmentation::addPlaceRoleID(%s, %d) is in mRoleID2Places but not in mPlace2RoleIDs", Place.c_str(), RoleID);
	}
}

void Fragmentation::deletePlaceRoleID(const place_t & Place, const role_id_t RoleID) {
	roleID2Places_t::iterator curRolePlace;
	place2RoleIDs_t::iterator curPlaceRole;
	pair<place2RoleIDs_t::iterator, place2RoleIDs_t::iterator> curPlaceRoles;
	pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces;
	bool foundA;
	bool foundB;

	foundA = false;
	curPlaceRoles = this->mPlace2RoleIDs.equal_range(Place);
	for (curPlaceRole=curPlaceRoles.first; curPlaceRole!=curPlaceRoles.second; ++curPlaceRole) {
		if (curPlaceRole->second == RoleID) {
			foundA = true;
			break;
		}
	}
	
	foundB = false;
	curRolePlaces = this->mRoleID2Places.equal_range(RoleID);
	for (curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
		if (curRolePlace->second == Place) {
			foundB = true;
			break;
		}
	}

	if (foundA && foundB) {
		this->mPlace2RoleIDs.erase(curPlaceRole);
		this->mRoleID2Places.erase(curRolePlace);
	}
	else if (foundA && !foundB) {
		abort(9, "Fragmentation::deletePlaceRoleID(%s, %d) is in mPlace2RoleIDs but not in mRoleID2Places", Place.c_str(), RoleID);
	}
	else if (!foundA && foundB) {
		abort(9, "Fragmentation::deletePlaceRoleID(%s, %d) is in mRoleID2Places but not in mPlace2RoleIDs", Place.c_str(), RoleID);
	}
	else {
		abort(9, "Fragmentation::deletePlaceRoleID(%s, %d) failed", Place.c_str(), RoleID);
	}
}

void Fragmentation::addPlaceFragID(const place_t & Place, const frag_id_t FragID) {
	pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFrags;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	bool foundA;
	bool foundB;
	
	foundA = false;
	curPlaceFrags = this->mPlace2FragIDs.equal_range(Place);
	for (place2FragIDs_t::iterator curPlaceFrag=curPlaceFrags.first; curPlaceFrag!=curPlaceFrags.second; ++curPlaceFrag) {
		if (curPlaceFrag->second == FragID) {
			foundA = true;
			break;
		}
	}

	foundB = false;
	curFragPlaces = this->mFragID2Places.equal_range(FragID);
	for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		if (curFragPlace->second == Place) {
			foundB = true;
			break;
		}
	}
	if (!(foundA && foundB)) {
		this->mPlace2FragIDs.insert( std::make_pair(Place, FragID) );
		this->mFragID2Places.insert( std::make_pair(FragID, Place) );
	}
	else if (foundA && !foundB) {
		abort(9, "Fragmentation::addPlaceFragID(%s, %d) is in mPlace2FragIDs but not in mFragID2Places", Place.c_str(), FragID);
	}
	else if (!foundA && foundB) {
		abort(9, "Fragmentation::addPlaceFragID(%s, %d) is in mFragID2Places but not in mPlace2FragIDs", Place.c_str(), FragID);
	}
}

void Fragmentation::deletePlaceFragID(const place_t & Place, const frag_id_t FragID) {
	place2FragIDs_t::iterator curPlaceFrag;
	fragID2Places_t::iterator curFragPlace;
	pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFrags;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	bool foundA;
	bool foundB;

	foundA = false;
	curPlaceFrags = this->mPlace2FragIDs.equal_range(Place);
	for (curPlaceFrag=curPlaceFrags.first; curPlaceFrag!=curPlaceFrags.second; ++curPlaceFrag) {
		if (curPlaceFrag->second == FragID) {
			foundA = true;
			break;
		}
	}
	
	foundB = false;
	curFragPlaces = this->mFragID2Places.equal_range(FragID);
	for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		if (curFragPlace->second == Place) {
			foundB = true;
			break;
		}
	}
	if (foundA && foundB) {
		//delete
		this->mPlace2FragIDs.erase(curPlaceFrag);
		this->mFragID2Places.erase(curFragPlace);
	}
	else if (foundA && !foundB) {
		abort(9, "Fragmentation::deletePlaceFragID(%s, %d) is in mPlace2FragIDs but not in mFragID2Places", Place.c_str(), FragID);
	}
	else if (!foundA && foundB) {
		abort(9, "Fragmentation::deletePlaceFragID(%s, %d) is in mFragID2Places but not in mPlace2FragIDs", Place.c_str(), FragID);
	}
	else {
		//status("deletePlaceFragID(%s, %d) failed", Place.c_str(), FragID);
		abort(9, "deletePlaceFragID(%s, %d) failed", Place.c_str(), FragID);
	}
}

bool Fragmentation::isSharedPlace(const place_t & Place) {
	return (this->mPlace2RoleIDs.count(Place) > 1);
}

bool Fragmentation::isStartTransition(const transition_t & Transition) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::isStartTransition(): buildRoleFragments() is necessary");
	}
	
	places_t transitionPlaces;
	transitions_t placeTransitions;
	pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFragIDs;
	frag_id_t transitionFragID;
	bool ret;	

	ret = true;

	transitionFragID = this->getTransitionFragID(Transition);

	transitionPlaces = this->getTransitionPreset(*this->mNet, Transition);
	FOREACH(p, transitionPlaces) {
		if (this->mPlace2FragIDs.count(*p) == 1) {
			curPlaceFragIDs = this->mPlace2FragIDs.equal_range(*p);
			for (place2FragIDs_t::iterator curPlaceFrag=curPlaceFragIDs.first; curPlaceFrag!=curPlaceFragIDs.second; ++curPlaceFrag) {
				if (curPlaceFrag->second == transitionFragID) {
					ret = false;
					break;
				}
			}
		}
		if (this->isSharedPlace(*p)) {
			placeTransitions = this->getPlacePreset(*this->mNet, *p);
			FOREACH(t, placeTransitions) {
				if (transitionFragID == this->getTransitionFragID(*t)) {
					ret = false;
				}
			}
		}
	}

	return ret;
}

void Fragmentation::replaceSharedStructure(const frag_id_t FragID, const place_t & Place) {
	place_t newPlace;
	transition_t newTransition;
	transitions_t placeTransitions;
	set< pnapi::Arc *> arcsToDelete;

	status("......replaceSharedStructure(%d, %s) called...", FragID, Place.c_str());
				
	std::stringstream curFragID;
	curFragID << FragID;
	newPlace = this->SERVICE_PLACE_PREFIX + curFragID.str() + "_" + Place;

	if (this->mNet->findPlace(newPlace) == NULL) {
		this->createPlace(newPlace, FragID, this->getFragmentRoleID(FragID));
	}

	placeTransitions = this->getPlacePostset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		if (this->getTransitionFragID(*t) == FragID) {
			this->createArc(newPlace, *t);
			arcsToDelete.insert(this->mNet->findArc(*this->mNet->findNode(Place), *this->mNet->findNode(*t)));
		}
		else {
			abort(15, "Fragmentation::replaceSharedStructure(%d, %s): forbidden struture", FragID, Place.c_str());
		}
	}
	
	placeTransitions = this->getPlacePreset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		if (this->getTransitionFragID(*t) == FragID) {
			this->createArc(*t, newPlace);
			arcsToDelete.insert(this->mNet->findArc(*this->mNet->findNode(*t), *this->mNet->findNode(Place)));
		}
		else {
			std::stringstream curFragID;
			curFragID << this->getTransitionFragID(*t);
			newTransition = this->SERVICE_TRANSITION_PREFIX + curFragID.str() + "_" + Place;

			if (this->mNet->findTransition(newTransition) == NULL) {
				this->createTransition(newTransition, FragID, this->getFragmentRoleID(FragID));
				this->createArc(Place, newTransition);
				this->createArc(newTransition, newPlace);
			}
		}
	}

	FOREACH(p, arcsToDelete) {
		if (*p == NULL) {
			abort(9, "Fragmentation::replaceSharedStructure(%d, %s): Nullpointer", FragID, Place.c_str());
		}
		this->mNet->deleteArc(**p);
	}

	status("......replaceSharedStructure(%d, %s) finished", FragID, Place.c_str());
}

validStatus_e Fragmentation::isSharedPlaceValidForFragment(const place_t & Place, const frag_id_t FragID, const bool Repair) {
	status("....isSharedPlaceValidForFragment(%s, %d, %d) called...", Place.c_str(), FragID, Repair);

	if (!isSharedPlace(Place)) {
		abort(9, "Fragmentation::isSharedPlaceValid(%s, %d, %d) is no SP", Place.c_str(), FragID, Repair);
	}

	frag_id_t FragmentRoleID;
	bool isInput;
	bool isOutput;
	bool hasSuccessor;
	bool hasPredecessor;
	transitions_t placeTransitions;
	validStatus_e ret;

	FragmentRoleID = this->getFragmentRoleID(FragID);
	isInput = isOutput = hasSuccessor = hasPredecessor= false;
	ret = VALID_OK;

	placeTransitions = this->getPlacePreset(*this->mNet, Place);			
	PNAPI_FOREACH(t, placeTransitions) {
		if (this->getTransitionRoleID(*t) != FragmentRoleID) {
			isInput = true;
		}
		else {
			hasPredecessor = true;
		}
	}
	placeTransitions = this->getPlacePostset(*this->mNet, Place);
	PNAPI_FOREACH(t, placeTransitions) {
		if (this->getTransitionRoleID(*t) != FragmentRoleID) {
			isOutput = true;
		}
		else {
			hasSuccessor = true;
		}
	}
	if (hasSuccessor && hasPredecessor) {
		if (isInput && !isOutput) {
			if (!Repair) {
				status("......%s -> %s", Place.c_str(), _cimportant_("TODO"));
				ret = VALID_TODO;
			}
			this->replaceSharedStructure(FragID, Place);
		}
		else {
			status("......%s -> %s", Place.c_str(), _cbad_("BAD"));
			ret = VALID_BAD;
		}
	}
	status("....isSharedPlaceValidForFragment(%s, %d, %d) finished", Place.c_str(), FragID, Repair);
	return ret;
}

validStatus_e Fragmentation::isFragmentValid(const frag_id_t FragID, const bool Repair) {
	status("..isFragmentValid(%d, %d) called...", FragID, Repair);	

	bool hitToDo;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	validStatus_e PlaceStatus;
	validStatus_e ret;

	ret = VALID_OK;
	hitToDo = false;

	curFragPlaces = this->mFragID2Places.equal_range(FragID);
	for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		if (this->isSharedPlace(curFragPlace->second)) {
			PlaceStatus = this->isSharedPlaceValidForFragment(curFragPlace->second, FragID, Repair);
			if (PlaceStatus == VALID_TODO) {
				hitToDo = true; 
			}
			else if (PlaceStatus == VALID_BAD) {
				status("....%d -> %s", FragID, _cbad_("BAD"));
				ret = VALID_BAD; 
			}
		}
	}
	if (hitToDo && (ret != VALID_BAD)) {
		status("....%d -> %s", FragID, _cimportant_("TODO"));
		ret = VALID_TODO;
	}
	status("..isFragmentValid(%d, %d) finished...", FragID, Repair);
	return ret;
}

validStatus_e Fragmentation::isProcessValid(const bool Repair) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::isProcessValid(%d): buildRoleFragments() is necessary", Repair);
	}
	
	status("isProcessValid(%d) called...", Repair);

	validStatus_e FragmentStatus;
	bool hitToDo;
	bool hitBad;
	validStatus_e ret;

	ret = VALID_OK;
	hitToDo = hitBad = false;

	for (frag_id_t FragID = 0; FragID <= this->mMaxFragID; FragID++) {
		if (!this->isFragmentEmpty(FragID)) {
			FragmentStatus = isFragmentValid(FragID, Repair);
			if (FragmentStatus == VALID_TODO) {
				hitToDo = true;
			}
			else if (FragmentStatus == VALID_BAD) {
				hitBad = true;
			}
		}
	}
	if (hitBad) {
		ret = VALID_BAD;
	}
	if (hitToDo) {
		ret = VALID_TODO;
	}
	status("isProcessValid(%d) finished", Repair);
	return ret;
}

bool Fragmentation::hasUnassignedTransitions(const place_t & Place) {
	transitions_t placeTransitions;
	size_t count;
	bool ret;

	ret = false;

	placeTransitions = this->getPlaceTransitions(*this->mNet, Place);
	count = placeTransitions.size();		
	PNAPI_FOREACH(t, placeTransitions) {
		if (this->getTransitionRoleID(*t) == this->ROLE_UNASSIGNED) {
			ret = true;
		}
	}

	if (count == 0) {
		abort(9, "Fragmentation::hasUnassignedTransitions(%s): place has no transitions", Place.c_str());
	}

	return ret;
}

bool Fragmentation::isFragmentEmpty(const frag_id_t FragID) {
	return (this->mFragID2Transitions.find(FragID) == this->mFragID2Transitions.end());
}

role_id_t Fragmentation::getFragmentRoleID(const frag_id_t FragID) {
	fragID2Transitions_t::const_iterator curFragTransition;
	transition2RoleID_t::const_iterator curTransitionRole;

	curFragTransition = this->mFragID2Transitions.find(FragID);
	if (curFragTransition == this->mFragID2Transitions.end()) {
		abort(9, "Fragmentation::getFragmentRoleID(%d) is unknown", FragID);
	}
	
	curTransitionRole = this->mTransition2RoleID.find(curFragTransition->second);
	
	if (curTransitionRole == this->mTransition2RoleID.end()) {
		abort(9, "Fragmentation::getFragmentRoleID(%d): transition %s is unassigned", FragID, curFragTransition->second.c_str());
	}
	
	return curTransitionRole->second;
}

string Fragmentation::getFragmentRoleName(const frag_id_t FragID) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::getFragmentRoleName(): buildRoleFragments() is necessary");
	}
	
	role_id_t RoleID;
	roleID2RoleName_t::const_iterator curRole;
	string ret;

	RoleID = this->getFragmentRoleID(FragID);
	if ( RoleID == this->ROLE_UNASSIGNED ) {
		ret = "unassigned";
	}
	else {
		curRole = this->mRoleID2RoleName.find(RoleID);
		if (curRole == this->mRoleID2RoleName.end()) {
			abort(9, "Fragmentation::getFragmentRoleName(%d) failed", RoleID);
		}
		ret = curRole->second;
	}
	return ret;
}

void Fragmentation::replaceFragIDs(const frag_id_t FragOld, const frag_id_t FragNew) {
	if (FragOld == FragNew) {
		abort(9, "Fragmentation::replaceFragIDs(%d, %d): frags are identical", FragOld, FragNew);
	}
	
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	set< pair<place_t, frag_id_t> > toAdd;
	set< pair<place_t, frag_id_t> > toDelete;
	transitions_t placeTransitions;

	curFragPlaces = this->mFragID2Places.equal_range(FragOld);
	for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		toAdd.insert( std::make_pair(curFragPlace->second, FragNew) );
		toDelete.insert( std::make_pair(curFragPlace->second, FragOld) );
	}

	FOREACH(p, toAdd) {
		this->addPlaceFragID(p->first, p->second);
	}
	FOREACH(p, toDelete) {
		this->deletePlaceFragID(p->first, p->second);
		placeTransitions = this->getPlaceTransitions(*this->mNet, p->first);		
		PNAPI_FOREACH(t, placeTransitions) {
			if (this->getTransitionFragID(*t) == FragOld) {
				this->setTransitionFragID(*t, FragNew);
			}
		}
	}
}

void Fragmentation::connectFragments() {
	fragID2Places_t::iterator place_f_a;
	fragID2Places_t::iterator place_f_b;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> places_f_a;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> places_f_b;
	set< pair<frag_id_t, frag_id_t> > toConnect;
	bool pairProcessed;
	bool changed;

	changed = true;
	while (changed) {
	
		changed = false;
		toConnect.clear();

		for (frag_id_t f_a = 0; f_a < this->mMaxFragID; f_a++) {
			for (frag_id_t f_b = 0; f_b < this->mMaxFragID; f_b++) {
				if (f_a != f_b) {
					if (!this->isFragmentEmpty(f_a) && !this->isFragmentEmpty(f_b)) {
						if (this->getFragmentRoleID(f_a) == this->getFragmentRoleID(f_b)) {
							places_f_a = this->mFragID2Places.equal_range(f_a);
							places_f_b = this->mFragID2Places.equal_range(f_b);
							pairProcessed = false;
							for (place_f_a=places_f_a.first; place_f_a!=places_f_a.second; ++place_f_a) {
								for (place_f_b=places_f_b.first; place_f_b!=places_f_b.second; ++place_f_b) {
									if (place_f_a->second == place_f_b->second) {						
										toConnect.insert( std::make_pair( std::max(f_a, f_b), std::min(f_a, f_b) ) );
										pairProcessed = true;
										break;
									}
								}
								if (pairProcessed) break;
							}
						}
					}
				}
			}
		}

		for (set< pair<frag_id_t, frag_id_t> >::reverse_iterator curPair=toConnect.rbegin(); curPair!=toConnect.rend(); ++curPair) {
			this->replaceFragIDs(curPair->first, curPair->second);
		}
	}
}

string Fragmentation::fragmentsToString() {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::fragmentsToString(): buildRoleFragments() is necessary");
	}

	std::stringstream ret;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;
	string package;

	package = PACKAGE;
	
	ret << "Fragmentation:" << std::endl;

	for (frag_id_t fragID=0; fragID<=this->mMaxFragID; fragID++) {
		if (!this->isFragmentEmpty(fragID)) {

			curFragPlaces = this->mFragID2Places.equal_range(fragID);
			for (size_t i=0; i<package.length()+2; ++i) {
				ret << " ";
			}
			ret << "Frag [" << fragID << "] [Role: " << this->getFragmentRoleName(fragID) << "] -> ";		
			for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
				ret << "p";
			}

			ret << "|";

			curFragTransitions = this->mFragID2Transitions.equal_range(fragID);
			for (fragID2Transitions_t::iterator curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
				ret << "t";
			}
			ret << std::endl;
		}
	}
	return ret.str();
}

void Fragmentation::eraseColors() {
	PNAPI_FOREACH(n, this->mNet->getNodes()) {
		(*n)->setColor();
	}
}

void Fragmentation::colorFragmentsByFragID(const bool ColorUnassigned) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::colorFragmentsByFragID(%d): buildRoleFragments() is necessary", ColorUnassigned);
	}
	
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;	
	size_t curColorID;
	string curColorName;

	this->eraseColors();
	curColorID = 0;

	for (frag_id_t FragID = 0; FragID <= this->mMaxFragID; FragID++) {
		if (!this->isFragmentEmpty(FragID)) {

			if ((this->getFragmentRoleID(FragID) != this->ROLE_UNASSIGNED) || ColorUnassigned) {
				if (curColorID == this->mMaxColors) {
					abort(16, "Fragmentation::colorFragmentsByFragID(%d): all %d colors used", ColorUnassigned, this->mMaxColors);
				}
				curColorName = this->getColorName(curColorID);

				curFragPlaces = this->mFragID2Places.equal_range(FragID);	
				for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
					if (!this->isSharedPlace(curFragPlace->second)) {				
						this->mPlaceName2PlacePointer[curFragPlace->second]->setColor(curColorName);
					}
				}
			
				curFragTransitions = this->mFragID2Transitions.equal_range(FragID);
				for (fragID2Transitions_t::iterator curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
					this->mTransitionName2TransitionPointer[curFragTransition->second]->setColor(curColorName);
				}
				
				curColorID++;
			}
		}
	}
}

void Fragmentation::colorFragmentsByRoleID(const bool ColorUnassigned) {
	if (!this->mRoleFragmentsBuild) {
		abort(8, "Fragmentation::colorFragmentsByRoleID(%d): buildRoleFragments() is necessary", ColorUnassigned);
	}
	
	pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces;
	pair<roleID2Transitions_t::iterator, roleID2Transitions_t::iterator> curRoleTransitions;
	size_t curColorID;
	size_t unassignedNodes;
	string curColorName;

	this->eraseColors();

	curColorID = unassignedNodes = 0;
	curColorName = this->getColorName(curColorID);
	
	if (ColorUnassigned) {
		curRolePlaces = this->mRoleID2Places.equal_range(this->ROLE_UNASSIGNED);	
		for (roleID2Places_t::const_iterator curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
			if (!this->isSharedPlace(curRolePlace->second)) {
				unassignedNodes++;
				this->mPlaceName2PlacePointer[curRolePlace->second]->setColor(curColorName);
			}
		}
		
		curRoleTransitions = this->mRoleID2Transitions.equal_range(this->ROLE_UNASSIGNED);
		for (roleID2Transitions_t::const_iterator curRoleTransition=curRoleTransitions.first; curRoleTransition!=curRoleTransitions.second; ++curRoleTransition) {
			unassignedNodes++;
			this->mTransitionName2TransitionPointer[curRoleTransition->second]->setColor(curColorName);
		}
		
		if (unassignedNodes != 0) {
			curColorID++;
		}
			
	}
	

	for (roleID2RoleName_t::iterator curRole=this->mRoleID2RoleName.begin(); curRole!=this->mRoleID2RoleName.end(); ++curRole) {
		if (curColorID == this->mMaxColors) {
			abort(16, "Fragmentation::colorFragmentsByRoleID(%d): all %d colors used", ColorUnassigned, this->mMaxColors);
		}
		
		curColorName = this->getColorName(curColorID);

		curRolePlaces = this->mRoleID2Places.equal_range(curRole->first);	
		for (roleID2Places_t::const_iterator curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
			if (!this->isSharedPlace(curRolePlace->second)) {
				this->mPlaceName2PlacePointer[curRolePlace->second]->setColor(curColorName);
			}
		}
	
		curRoleTransitions = this->mRoleID2Transitions.equal_range(curRole->first);
		for (roleID2Transitions_t::const_iterator curRoleTransition=curRoleTransitions.first; curRoleTransition!=curRoleTransitions.second; ++curRoleTransition) {
			this->mTransitionName2TransitionPointer[curRoleTransition->second]->setColor(curColorName);
		}

		curColorID++;
	}

}

string Fragmentation::getColorName(const size_t ColorID) {
	colorID2ColorName_t::const_iterator curColor;
	
	curColor = this->mColorID2ColorName.find(ColorID);
	if (curColor == this->mColorID2ColorName.end()) {
		abort(9, "Fragmentation::getColorName(%d) is unknown", ColorID);
	}
	return curColor->second;
}

void Fragmentation::initColors() {
	this->mMaxColors = 30;
	
	this->mColorID2ColorName[0] = "cyan";
	this->mColorID2ColorName[1] = "antiquewhite4";
	this->mColorID2ColorName[2] = "aquamarine4";
	this->mColorID2ColorName[3] = "azure4";
	this->mColorID2ColorName[4] = "bisque3";
	this->mColorID2ColorName[5] = "blue1";
	this->mColorID2ColorName[6] = "brown";
	this->mColorID2ColorName[7] = "burlywood";
	this->mColorID2ColorName[8] = "cadetblue";
	this->mColorID2ColorName[9] = "chartreuse";
	this->mColorID2ColorName[10] = "chocolate";
	this->mColorID2ColorName[11] = "coral";
	this->mColorID2ColorName[12] = "cornflowerblue";
	this->mColorID2ColorName[13] = "cornsilk4";
	this->mColorID2ColorName[14] = "cyan3";
	this->mColorID2ColorName[15] = "darkgoldenrod3";
	this->mColorID2ColorName[16] = "darkolivegreen1";
	this->mColorID2ColorName[17] = "darkorange1";
	this->mColorID2ColorName[18] = "darkorchid1";
	this->mColorID2ColorName[19] = "darkseagreen";
	this->mColorID2ColorName[20] = "darkslateblue";
	this->mColorID2ColorName[21] = "darkslategray4";
	this->mColorID2ColorName[22] = "deeppink1";
	this->mColorID2ColorName[23] = "deepskyblue1";
	this->mColorID2ColorName[24] = "dimgrey";
	this->mColorID2ColorName[25] = "dodgerblue4";
	this->mColorID2ColorName[26] = "firebrick4";
	this->mColorID2ColorName[27] = "gold";
	this->mColorID2ColorName[28] = "goldenrod";
	this->mColorID2ColorName[29] = "gray";
}

void Fragmentation::createPetrinetByFragID(const frag_id_t FragID) {
	if (this->isFragmentEmpty(FragID)) {
		abort(9, "Fragmentation::createPetrinetByFragID(%d): fragment is empty", FragID);
	}

	fragID2Transitions_t::iterator curFragTransition;
	fragID2Places_t::iterator curFragPlace;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;
	pnapi::Place * place;
	pnapi::Transition * transition;

	this->mFragmentNet = new pnapi::PetriNet();

	curFragPlaces = this->mFragID2Places.equal_range(FragID);	
	for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		this->mFragmentNet->createPlace(curFragPlace->second);	
	}
	
	
	curFragTransitions = this->mFragID2Transitions.equal_range(FragID);
	for (curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
		this->mFragmentNet->createTransition(curFragTransition->second);	
	}

	for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
		place = this->mPlaceName2PlacePointer[curFragPlace->second];
		for (curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
			transition = this->mTransitionName2TransitionPointer[curFragTransition->second];
			if (this->mNet->findArc(*place, *transition)) {
				this->mFragmentNet->createArc(*this->mFragmentNet->findPlace(curFragPlace->second), *this->mFragmentNet->findTransition(curFragTransition->second));	
			}
			if (this->mNet->findArc(*transition, *place)) {
				this->mFragmentNet->createArc(*this->mFragmentNet->findTransition(curFragTransition->second), *this->mFragmentNet->findPlace(curFragPlace->second));	
			}
		}	
	}
}

pnapi::PetriNet Fragmentation::createPetrinetByRoleID(const role_id_t RoleID) {
	if (!this->mServicesCreated) {
		abort(8, "Fragmentation::createPetrinetByRoleID(%d): buildServices() is necessary", RoleID);
	}
	
	status("createPetrinetByRoleID(%d) called...", RoleID);

	roleID2Transitions_t::iterator curRoleTransition;
	roleID2Places_t::iterator curRolePlace;
	pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces;
	pair<roleID2Transitions_t::iterator, roleID2Transitions_t::iterator> curRoleTransitions;
	pnapi::Place * place;
	pnapi::Transition * transition;
	pnapi::PetriNet ret;

	status("..creating places");
	
	curRolePlaces = this->mRoleID2Places.equal_range(RoleID);	
	for (curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
		if (!this->isSharedPlace(curRolePlace->second)) {
			ret.createPlace(curRolePlace->second).setTokenCount(this->mPlaceName2PlacePointer[curRolePlace->second]->getTokenCount());
		}
		else {
			if (this->getTransitionRoleID(*this->getPlacePreset(*this->mNet, curRolePlace->second).begin()) != RoleID) {
				ret.createInputLabel(curRolePlace->second);
			}
			else {
				ret.createOutputLabel(curRolePlace->second);
			}
		}
	}

	status("..creating transitions");
	curRoleTransitions = this->mRoleID2Transitions.equal_range(RoleID);
	for (curRoleTransition=curRoleTransitions.first; curRoleTransition!=curRoleTransitions.second; ++curRoleTransition) {
		ret.createTransition(curRoleTransition->second);	
	}

	status("..creating arcs");
	for (curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
		place = this->mPlaceName2PlacePointer[curRolePlace->second];
		for (curRoleTransition=curRoleTransitions.first; curRoleTransition!=curRoleTransitions.second; ++curRoleTransition) {
			transition = this->mTransitionName2TransitionPointer[curRoleTransition->second];
			if (this->mNet->findArc(*place, *transition)) {
				if (this->isSharedPlace(curRolePlace->second)) {
					ret.findTransition(curRoleTransition->second)->addLabel(*ret.getInterface().findLabel(curRolePlace->second));
				}
				else {
					ret.createArc(*ret.findPlace(curRolePlace->second), *ret.findTransition(curRoleTransition->second));
				}
			}
			if (this->mNet->findArc(*transition, *place)) {
				if (this->isSharedPlace(curRolePlace->second)) {
					ret.findTransition(curRoleTransition->second)->addLabel(*ret.getInterface().findLabel(curRolePlace->second));
				}
				else {
					ret.createArc(*ret.findTransition(curRoleTransition->second), *ret.findPlace(curRolePlace->second));
				}
			}
		}	
	}

	status("createPetrinetByRoleID(%d) finished", RoleID);
	return ret;
}

void Fragmentation::processUnassignedFragment(const frag_id_t FragID) {
	if (this->getFragmentRoleID(FragID) != this->ROLE_UNASSIGNED) {
		abort(9, "Fragmentation::processUnassignedFragment(%d): fragment is not unassigned", FragID);
	}

	status("..processUnassignedFragment(%d) called...", FragID);

	time_t start_time, end_time;
    string outputParam;
    string fileName;
	string curPartFileName;
	std::stringstream number;
	std::ofstream DianeTemp;

	this->mCurProcessingFragID = FragID;
	
    // if the verbose-flag is set, it is also given to Diane
    if (args_info.verbose_flag) {
        outputParam += " -v ";
    }
	else {
		outputParam += " ";
	}

    // create a temporary file
	if (args_info.inputs_num == 0) {
		fileName = "STDIN_";
	}
	else {
		fileName = string(args_info.inputs[0]) + "_";
	}
	number << FragID;
	fileName += "Frag[" + number.str() + "]2Diane";

	if (fileExists(fileName)) {
		abort(5, "Fragmentation::processUnassignedFragment(%d): file %s already exists", _cfilename_(fileName));
	}

	DianeTemp.open(fileName.c_str());
	if (DianeTemp.fail()) {
    	abort(3, "Fragmentation::processUnassignedFragment(%d): file %s cannot be opened", FragID, _cfilename_(fileName));
    }

	// create petri net off given fragment
	this->createPetrinetByFragID(FragID);
	//write perti net
	DianeTemp << pnapi::io::lola << *this->mFragmentNet;
    // add the output filename
    outputParam += fileName;


    // select Diane binary and build Diane command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    string command_line(basename(args_info.diane_arg));
#else
    string command_line(args_info.diane_arg);
#endif

    command_line += outputParam;
    // call Diane
    status("....calling %s: '%s'", _ctool_("Diane"), _cparameter_(command_line));
	this->mDianeCalls++;
    {
        // set start time
        time(&start_time);
//ToDo: system call?! (statt Pipe)?
        // call Diane and open a pipe
//ToDo: Variable auslagern
        FILE* fp = popen(command_line.c_str(), "r");
		// close the pipe
        pclose(fp);
        // set end time
        time(&end_time);
    }
	DianeTemp.close();
	
	if (!args_info.noClean_flag) {
		if (remove(fileName.c_str()) != 0) {
			abort(12, "Fragmentation::processUnassignedFragment(%d): file %s cannot be deleted", FragID, _cfilename_(fileName));
		}
	}

    // status message
    status("....%s is done [%.0f sec]", _ctool_("Diane"), difftime(end_time, start_time));

	this->mFragmentUnprocessedNodes.clear();
	this->mTransition2DianeID.clear();
	this->mDianeID2Transitions.clear();
	this->mPlace2DianeIDs.clear();
	this->mDianeID2Places.clear();
	
	PNAPI_FOREACH(n, this->mFragmentNet->getNodes()) {
		this->mFragmentUnprocessedNodes.insert((*n)->getName());
	}
	delete this->mFragmentNet;

	status("....Fragment %d contains %d elements", FragID, this->mFragmentUnprocessedNodes.size());
	
	this->mDianeFragments = 0;
	curPartFileName = fileName + ".part0.owfn";
	while (fileExists(curPartFileName)) {
		this->readDianeOutputFile(curPartFileName);
		if (!args_info.noClean_flag) {
			if (remove(curPartFileName.c_str()) != 0) {
				abort(12, "Fragmentation::processUnassignedFragment(%d): file %s cannot be deleted", FragID, _cfilename_(curPartFileName));
			}
		}
		//try next file
		std::stringstream curDianeFragmentAsString;
		curDianeFragmentAsString << ++this->mDianeFragments;
		curPartFileName = fileName + ".part" + curDianeFragmentAsString.str() + ".owfn";
	}

	if (this->mFragmentUnprocessedNodes.size() != 0) {
		abort(9, "Fragmentation::processUnassignedFragment(%d): %d nodes are unprocessed", FragID, this->mFragmentUnprocessedNodes.size());
	}

	this->processDianeFragmentation();

	status("..processUnassignedFragment(%d) finished", FragID);
}

void Fragmentation::readDianeOutputFile(const string & FileName) {
	status("....readDianeOutputFile(%s) called...", _cfilename_(FileName));	

	std::ifstream DianeFragment;
	std::ostringstream s;
	places_t placesToDo;
	places_t Places;
	transitions_t Transitions;
	pnapi::PetriNet net;
	set<pnapi::Label *> interface;

	DianeFragment.open(FileName.c_str());
	if (DianeFragment.fail()) {
    	abort(3, "Fragmentation::readDianeOutputFile(%s): file cannot be opened", _cfilename_(FileName));
    }

	DianeFragment >> pnapi::io::owfn >> net;
	if (args_info.verbose_flag) {
		s << pnapi::io::stat << net;
		status("......%s", s.str().c_str());
    }
	DianeFragment.close();
	
	Places = this->getPlaces(net);
	PNAPI_FOREACH(p, Places) {
		placesToDo.insert(*p);
	}
	//handle interface as places...
	interface = net.getInterface().getAsynchronousLabels();
	PNAPI_FOREACH(i, interface) {
		placesToDo.insert((*i)->getName());
	}

	PNAPI_FOREACH(p, placesToDo) {
		this->addPlaceDianeID(*p, this->mDianeFragments);
		this->mFragmentUnprocessedNodes.erase(*p);
	}

	Transitions = this->getTransitions(net);
	PNAPI_FOREACH(t, Transitions) {
		this->setTransitionDianeID(*t, this->mDianeFragments, false);
		this->mFragmentUnprocessedNodes.erase(*t);
	}
	
	status("....readDianeOutputFile(%s) finished", _cfilename_(FileName));
}

void Fragmentation::processDianeFragmentation() {
	status("....processDianeFragmentation() called...");

	set<diane_id_t> toDo;
	set< pair<diane_id_t, pair<role_id_t, frag_id_t> > > toSet;
	pair<role_id_t, frag_id_t> bestConnection;
	bool progress;

	this->mCurrentDianeForces = 0;
	this->mCurrentDianeAlternatives = 0;

	for (diane_id_t d = 0; d < this->mDianeFragments; d++) {		
		toDo.insert(d);
	}

	while (toDo.size() != 0) {
		toSet.clear();
		
		for (diane_id_t d = 0; d < this->mDianeFragments; d++) {
			if (toDo.find(d) != toDo.end()) {
				bestConnection = this->getBestConnectionForDianeFragment(d, false);
				if (bestConnection.first != this->ROLE_UNASSIGNED) {
					toSet.insert( std::make_pair(d, bestConnection) );
				}
			}
		}

		if (toSet.size() != 0) {
			FOREACH(p, toSet) {
				this->setDianeFragmentConnection((*p).first, (*p).second);
				toDo.erase((*p).first);
			}
		}
		else {
			progress = false;
			for (diane_id_t d = 0; d < this->mDianeFragments; d++) {
				if (toDo.find(d) != toDo.end()) {
					bestConnection = this->getBestConnectionForDianeFragment(d, true);
					if (bestConnection.first != this->ROLE_UNASSIGNED) {
						this->setDianeFragmentConnection(d, bestConnection);
						toDo.erase(d);
						progress = true;
						break;
					}
				}
			}
			if (!progress) {
				abort(17, "Fragmentation::processDianeFragmentation(): no progress");
			}
		}
		
	}

	this->mOverallDianeForces += this->mCurrentDianeForces;
	this->mOverallDianeAlternatives += this->mCurrentDianeAlternatives;

	if (this->mCurrentDianeForces != 0) {
		status("......%d forced decisions", this->mCurrentDianeForces);
		status("........%d alternatives", this->mCurrentDianeAlternatives);
	}

	this->connectFragments();

	status("....processDianeFragmentation() finished");
}

void Fragmentation::setTransitionDianeID(const transition_t & Transition, const diane_id_t DianeID, const bool Override) {;
	pair<dianeID2Transitions_t::iterator, dianeID2Transitions_t::iterator> curDianeTransitions;	
	transition2DianeID_t::iterator curTransitionDiane;
	dianeID2Transitions_t::iterator curDianeTransition;	
	
	curTransitionDiane = this->mTransition2DianeID.find(Transition);
	if (curTransitionDiane != this->mTransition2DianeID.end()) {
		
		if (!Override) {
			abort(6, "Fragmentation::setTransitionDianeID(%s, %d) is called again", Transition.c_str(), DianeID);
		}
		bool found = false;
		
		curDianeTransitions = this->mDianeID2Transitions.equal_range(curTransitionDiane->second);		
		for (curDianeTransition=curDianeTransitions.first; curDianeTransition!=curDianeTransitions.second; ++curDianeTransition) {
			if (curDianeTransition->second == Transition) {
				found = true;
				break;
			}
		}
		if (!found) {
			abort(9, "Fragmentation::setTransitionDianeID(%s, %d) failed", Transition.c_str(), DianeID);
		}
		this->mDianeID2Transitions.erase(curDianeTransition);		
		this->mTransition2DianeID.erase(curTransitionDiane);
	}
	this->mTransition2DianeID.insert( std::make_pair(Transition, DianeID) );
	this->mDianeID2Transitions.insert( std::make_pair(DianeID, Transition) );
}

diane_id_t Fragmentation::getTransitionDianeID(const transition_t & Transition) {
	transition2DianeID_t::iterator curTransitionDiane;

	curTransitionDiane = this->mTransition2DianeID.find(Transition);
	if (curTransitionDiane == this->mTransition2DianeID.end()) {
		abort(9, "Fragmentation::getTransitionDianeID(%s) is unknown", Transition.c_str());
	}
	return curTransitionDiane->second;
}

void Fragmentation::addPlaceDianeID(const place_t & Place, const diane_id_t DianeID) {
	pair<place2DianeIDs_t::iterator, place2DianeIDs_t::iterator> curPlaceDianes;
	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	bool foundA;
	bool foundB;

	foundA = false;
	curPlaceDianes = this->mPlace2DianeIDs.equal_range(Place);
	for (place2DianeIDs_t::iterator curPlaceDiane=curPlaceDianes.first; curPlaceDiane!=curPlaceDianes.second; ++curPlaceDiane) {
		if (curPlaceDiane->second == DianeID) {
			foundA = true;
			break;
		}
	}

	foundB = false;
	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
		if (curDianePlace->second == Place) {
			foundB = true;
			break;
		}
	}
	if (!(foundA && foundB)) {
		this->mPlace2DianeIDs.insert( std::make_pair(Place, DianeID) );
		this->mDianeID2Places.insert( std::make_pair(DianeID, Place) );
	}
	else if (foundA && !foundB) {
		abort(9, "Fragmentation::addPlaceDianeID(%s, %d) is in mPlace2FragIDs but not in mDianeID2Places", Place.c_str(), DianeID);
	}
	else if (!foundA && foundB) {
		abort(9, "Fragmentation::addPlaceDianeID(%s, %d) is in mFragID2Places but not in mPlace2DianeIDs", Place.c_str(), DianeID);
	}
}

void Fragmentation::setDianeFragmentConnection(const diane_id_t DianeID, pair<role_id_t, frag_id_t> Connection) {
	status("......setDianeFragmentConnection(%d, <%d, %d>) called...", DianeID, Connection.first, Connection.second);

	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<dianeID2Transitions_t::iterator, dianeID2Transitions_t::iterator> curDianeTransitions;
	places_t placesToDo;
	transitions_t transitionsToDo;

	placesToDo.clear();
	transitionsToDo.clear();
	
	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
		placesToDo.insert(curDianePlace->second);
	}

	curDianeTransitions = this->mDianeID2Transitions.equal_range(DianeID);
	for (dianeID2Transitions_t::iterator curDianeTransition=curDianeTransitions.first; curDianeTransition!=curDianeTransitions.second; ++curDianeTransition) {
		transitionsToDo.insert(curDianeTransition->second);
	}

	FOREACH(t, transitionsToDo) {
		this->setTransitionRoleID(*t, Connection.first);
		this->setTransitionFragID(*t, Connection.second);
	}

	FOREACH(p, placesToDo) {
		this->addPlaceRoleID(*p, Connection.first);
		this->addPlaceFragID(*p, Connection.second);
	}

	FOREACH(p, placesToDo) {
		if (!this->hasUnassignedTransitions(*p)) {
			this->deletePlaceRoleID(*p, this->ROLE_UNASSIGNED);
			this->deletePlaceFragID(*p, this->mCurProcessingFragID);
		}
	}

	this->connectFragments();

	status("......setDianeFragmentConnection(%d, <%d, %d>) finished", DianeID, Connection.first, Connection.second);
}

pair<role_id_t, frag_id_t> Fragmentation::getBestConnectionForDianeFragment(const diane_id_t DianeID, const bool Force) {
	status("......getBestConnectionForDianeFragment(%d, %d) called...", DianeID, Force);

	transitions_t transitionInputProcessed;
	transitions_t transitionOutputProcessed;
	transitions_t placeTransitions;
	vector<role_id_t> candidates;
	roles_t knownCandidates;
	roles_t toDelete;
	roles_t topCovered;
	roles_t topAdjacent;
	multimap<role_id_t, frag_id_t> roleFragments;	
	map<role_id_t, size_t> inputRoleIDs;
	map<role_id_t, size_t> outputRoleIDs;
	map<role_id_t, size_t> coveredRoleIDs;
	map<role_id_t, size_t> adjacentRoleIDs;
	map<role_id_t, size_t>::iterator elem;
	size_t input;
	size_t output;
	size_t curSize;
	size_t count;
	size_t maxRoleFragments;
	role_id_t maxRoleID;
	role_id_t transitionRoleID;
	frag_id_t transitionFragID;
	bool found;
	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<multimap<role_id_t, frag_id_t>::iterator, multimap<role_id_t, frag_id_t>::iterator> curRoleFragments;
	pair<role_id_t, frag_id_t> ret;

	adjacentRoleIDs.clear();
	coveredRoleIDs.clear();
	transitionInputProcessed.clear();
	transitionOutputProcessed.clear();

	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
		input = 0;
		output = 0;
		inputRoleIDs.clear();
		outputRoleIDs.clear();
		
		//for all transitions (preset)
		placeTransitions = this->getPlacePreset(*this->mNet, curDianePlace->second);	
		PNAPI_FOREACH(t, placeTransitions) {
			if (transitionInputProcessed.find(*t) == transitionInputProcessed.end()) {
				//tranisition is unprocessed
				transitionRoleID = this->getTransitionRoleID(*t);
				input++;
				if (transitionRoleID != this->ROLE_UNASSIGNED) {
					//handle RoleID
					elem = inputRoleIDs.find(transitionRoleID);				
					curSize = 1;						
					if (elem != inputRoleIDs.end()) {
						curSize = elem->first;
						inputRoleIDs.erase(elem);
					}
					inputRoleIDs.insert( std::make_pair(transitionRoleID, curSize) );

					//handle FragID
					transitionFragID = this->getTransitionFragID(*t);
					found = false;	
					curRoleFragments = roleFragments.equal_range(transitionRoleID);					
					for (multimap<role_id_t, frag_id_t>::iterator curRoleFragment=curRoleFragments.first; curRoleFragment!=curRoleFragments.second; ++curRoleFragment) {
						if (curRoleFragment->second == transitionFragID) {
							found = true;
						}
					}
					if (!found) {
						roleFragments.insert( std::make_pair(transitionRoleID, transitionFragID) );
					}

				} 						
			} //transition is unprocessed
			else {
			}
			//add
			transitionInputProcessed.insert(*t);
		} //transitions in Preset

		for (map<role_id_t, size_t>::iterator inputRoleID=inputRoleIDs.begin(); inputRoleID!=inputRoleIDs.end(); ++inputRoleID) {
			curSize = 1;
			elem = adjacentRoleIDs.find(inputRoleID->first);				
			if (elem != adjacentRoleIDs.end()) {
				curSize = elem->second;
				adjacentRoleIDs.erase(elem);
			}
			adjacentRoleIDs.insert( std::make_pair(inputRoleID->first, curSize) );
		}
	
		if (input > 1) {
			for (map<role_id_t, size_t>::iterator inputRoleID=inputRoleIDs.begin(); inputRoleID!=inputRoleIDs.end(); ++inputRoleID) {
				curSize = 1;
				elem = coveredRoleIDs.find(inputRoleID->first);				
				if (elem != coveredRoleIDs.end()) {
					curSize = elem->second;
					coveredRoleIDs.erase(elem);
				}
				coveredRoleIDs.insert( std::make_pair(inputRoleID->first, curSize) );
			}
		}


		//for all transitions (postset)
		placeTransitions = this->getPlacePostset(*this->mNet, curDianePlace->second);	
		PNAPI_FOREACH(t, placeTransitions) {
			if (transitionOutputProcessed.find(*t) == transitionOutputProcessed.end()) {
				//tranisition is unprocessed
				transitionRoleID = this->getTransitionRoleID(*t);
				output++;
				if (transitionRoleID != this->ROLE_UNASSIGNED) {
					//handle RoleID
					elem = outputRoleIDs.find(transitionRoleID);				
					curSize = 1;						
					if (elem != outputRoleIDs.end()) {
						curSize = elem->first;
						outputRoleIDs.erase(elem);
					}
					outputRoleIDs.insert( std::make_pair(transitionRoleID, curSize) );

					//handle FragID
					transitionFragID = this->getTransitionFragID(*t);
					found = false;	
					curRoleFragments = roleFragments.equal_range(transitionRoleID);					
					for (multimap<role_id_t, frag_id_t>::iterator curRoleFragment=curRoleFragments.first; curRoleFragment!=curRoleFragments.second; ++curRoleFragment) {
						if (curRoleFragment->second == transitionFragID) {
							found = true;
						}
					}
					if (!found) {
						roleFragments.insert( std::make_pair(transitionRoleID, transitionFragID) );
					}

				} 						
			} //transition is unprocessed
			else {
			}
			//add
			transitionOutputProcessed.insert(*t);
		} //transitions in Preset
		for (map<role_id_t, size_t>::iterator outputRoleID=outputRoleIDs.begin(); outputRoleID!=outputRoleIDs.end(); ++outputRoleID) {
			curSize = 1;
			elem = 	adjacentRoleIDs.find(outputRoleID->first);				
			if (elem != adjacentRoleIDs.end()) {
				curSize = elem->second;
				adjacentRoleIDs.erase(elem);
			}
			adjacentRoleIDs.insert( std::make_pair(outputRoleID->first, curSize) );
		}
		
		if (output > 1) {
			for (map<role_id_t, size_t>::iterator outputRoleID=outputRoleIDs.begin(); outputRoleID!=outputRoleIDs.end(); ++outputRoleID) {
				curSize = 1;
				elem = coveredRoleIDs.find(outputRoleID->first);				
				if (elem != coveredRoleIDs.end()) {
					curSize = elem->second;
					coveredRoleIDs.erase(elem);
				}
				coveredRoleIDs.insert( std::make_pair(outputRoleID->first, curSize) );
			}
		}

	}

	//auswerten
	ret = std::make_pair(this->ROLE_UNASSIGNED, this->mMaxFragID);

	knownCandidates.clear();
	candidates.clear();
	topCovered = getTopRoleIDs(coveredRoleIDs);
	topAdjacent = getTopRoleIDs(adjacentRoleIDs);

	status("........Diane-Fragment %d has %d covered roles", DianeID, coveredRoleIDs.size());
	FOREACH(r, topCovered) {
		candidates.push_back(*r);
		knownCandidates.insert(*r);
	}
	
	status("........Diane-Fragment %d has %d adjacent roles", DianeID, topAdjacent.size());
	FOREACH(r, topAdjacent) {
		if (knownCandidates.find(*r) == knownCandidates.end()) {
			candidates.push_back(*r);
			knownCandidates.insert(*r);
		}
	}

	count = 0;
	maxRoleFragments = 0;
	maxRoleID = this->ROLE_UNASSIGNED;

	FOREACH(r, knownCandidates) {
		count = roleFragments.count(*r);
		if (maxRoleFragments < count) {
			maxRoleFragments = count;
			maxRoleID = *r;
		}
	}
	
	toDelete.clear();
	for (vector<role_id_t>::iterator r=candidates.begin(); r<candidates.end(); r++) {	
		if (roleFragments.count(*r) < count) {
			toDelete.insert(*r);
		}
	}
	FOREACH(r, toDelete) {
		knownCandidates.erase(*r);
		for (vector<role_id_t>::iterator rc=candidates.begin(); rc<candidates.end(); rc++) {
			if (*r == *rc) {
				candidates.erase(rc);
				break;
			}
		}
	}
	

	if (knownCandidates.size() == 0) {
		status("........Diane-Fragment %d has no candidates -> skip fragment", DianeID);
	}
	else {
		status("........Diane-Fragment %d has %d possible candidates", DianeID, knownCandidates.size());
		toDelete.clear();
		FOREACH(r, knownCandidates) {
			if (!this->willDianeAssignementValid(DianeID, *r)) {
				status("..........check %d %s", *r, _cbad_("FAILED"));
				toDelete.insert(*r);
			}
			else {
				status("..........check %d %s", *r, _cgood_("PASSED"));
			}
		}

		FOREACH(r, toDelete) {
			knownCandidates.erase(*r);
			for (vector<role_id_t>::iterator rc=candidates.begin(); rc<candidates.end(); rc++) {
				if (*r == *rc) {
					candidates.erase(rc);
					break;
				}
			}
		}

		status("........Diane-Fragment %d has %d real candidates", DianeID, knownCandidates.size());
		if ((knownCandidates.size() > 1) && (!Force)) {
			status("........Diane-Fragment %d should not be decided -> skip fragment", DianeID);
			ret = std::make_pair(this->ROLE_UNASSIGNED, this->mMaxFragID);
		}
		else if (knownCandidates.size() > 0) {
			ret = std::make_pair(*candidates.begin(), roleFragments.find(*candidates.begin())->second);
		}

	}


	if ((ret.first != this->ROLE_UNASSIGNED) && (ret.second != this->mMaxFragID)) {
		if (Force) {
			status("........Diane-Fragment %d is %s to connection (%d, %d)", DianeID, _cwarning_("FORCED"), ret.first, ret.second);
			this->mCurrentDianeForces++;
			this->mCurrentDianeAlternatives += knownCandidates.size();
		}
		else {
			status("........Diane-Fragment %d becomes connection (%d, %d)", DianeID, ret.first, ret.second);
		}
	}

	status("......getBestConnectionForDianeFragment(%d, %d) finished", DianeID, Force);
	return ret;
}

bool Fragmentation::willDianeAssignementValid(const diane_id_t DianeID, const role_id_t RoleID) {
	status("............willDianeAssignementValid(%d, %d) called...", DianeID, RoleID);

	role_id_t predecessor;
	role_id_t successor;
	transitions_t placePreset;
	transitions_t placePostset;
	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<dianeID2Transitions_t::iterator, dianeID2Transitions_t::iterator> curDianeTransitions;
	bool ret;

	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	curDianeTransitions = this->mDianeID2Transitions.equal_range(DianeID);
	ret = true;

	for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
		predecessor = successor = this->ROLE_UNASSIGNED;

		placePreset = this->getPlacePreset(*this->mNet, curDianePlace->second);
		placePostset = this->getPlacePostset(*this->mNet, curDianePlace->second);
		
		for (dianeID2Transitions_t::iterator curDianeTransition=curDianeTransitions.first; curDianeTransition!=curDianeTransitions.second; ++curDianeTransition) {
			if (placePreset.find(curDianeTransition->second) != placePreset.end()) {
				predecessor = RoleID;
			}
			if (placePostset.find(curDianeTransition->second) != placePostset.end()) {
				successor = RoleID;
			}
		}

		if (!this->willPlaceValid(curDianePlace->second, predecessor, successor)) {
			ret = false;
			break;
		}
	}

	status("............willDianeAssignementValid(%d, %d) finished", DianeID, RoleID);
	return ret;

}

bool Fragmentation::willPlaceValid(const place_t & Place, const role_id_t Predecessor, const role_id_t Successor) {
	status("..............willPlaceValid(%s, %d, %d) called...", Place.c_str(), Predecessor, Successor);

	roles_t predecessors;
	roles_t successors;
	transitions_t placeTransitions;
	bool ret;

	ret = true;

	placeTransitions = this->getPlacePreset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		predecessors.insert(this->getTransitionRoleID(*t));
	}
	predecessors.erase(this->ROLE_UNASSIGNED);

	placeTransitions = this->getPlacePostset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		successors.insert(this->getTransitionRoleID(*t));
	}
	successors.erase(this->ROLE_UNASSIGNED);

	if (Predecessor != this->ROLE_UNASSIGNED) {
		predecessors.insert(Predecessor);
	}

	if (Successor != this->ROLE_UNASSIGNED) {
		successors.insert(Successor);
	}

	if ((setIntersection(predecessors, successors).size() != 0) && (successors.size() > 1)) {
		ret = false;
	}

	status("..............willPlaceValid(%s, %d, %d): %d finished", Place.c_str(), Predecessor, Successor, ret);
	return ret;

}

roles_t Fragmentation::getTopRoleIDs(const map<role_id_t, size_t> & RoleMap) const {
	size_t maxRoleSize;
	roles_t ret;

	maxRoleSize = 0;
	for (role_id_t r = 0; r < this->mRoleName2RoleID.size(); r++) {
		if (RoleMap.find(r) != RoleMap.end()) {
			if (RoleMap.find(r)->second > maxRoleSize) {
				maxRoleSize = RoleMap.find(r)->second;
			}
		}
	}

	for (role_id_t r = 0; r < this->mRoleName2RoleID.size(); r++) {
		if (RoleMap.find(r) != RoleMap.end()) {
			if (RoleMap.find(r)->second == maxRoleSize) {
				ret.insert(r);
			}
		}
	}
	return ret;
}

void Fragmentation::createArc(const node_t & Source, const node_t & Target) {
	pnapi::Node * source;
	pnapi::Node * target;

	source = this->mNet->findNode(Source);
	target = this->mNet->findNode(Target);

	if (source == NULL) {
		abort(9, "Fragmentation::createArc(%s, %s): source is unknown", Source.c_str(), Target.c_str());
	}
	if (target == NULL) {
		abort(9, "Fragmentation::createArc(%s, %s): destination is unknown", Source.c_str(), Target.c_str());
	}
	if (this->mNet->findArc(*source, *target) != NULL) {
		abort(9, "Fragmentation::createArc(%s, %s): arc is already known", Source.c_str(), Target.c_str());
	}
	this->mNet->createArc(*source, *target);
}

void Fragmentation::createPlace(const place_t & Place, const frag_id_t FragID, const role_id_t RoleID) {
	if (this->mNet->findPlace(Place) != NULL) {
		abort(9, "Fragmentation::createPlace(%s, %d, %d): place is already known", Place.c_str(), FragID, RoleID);
	}

	pnapi::Place * ret;
	
	ret = &this->mNet->createPlace(Place);
	this->mPlaceName2PlacePointer[Place] = ret;
	this->mPlacePointer2PlaceName[ret] = Place;
	this->addPlaceFragID(Place, FragID);
	this->addPlaceRoleID(Place, RoleID);
}

void Fragmentation::createTransition(const transition_t & Transition, const frag_id_t FragID, const role_id_t RoleID) {
	if (this->mNet->findTransition(Transition) != NULL) {
		abort(9, "Fragmentation::createTransition(%s, %d, %d): transition is already known", Transition.c_str(), FragID, RoleID);
	}

	pnapi::Transition * ret;
	
	ret = &this->mNet->createTransition(Transition);

	this->mTransitionName2TransitionPointer[Transition] = ret;
	this->mTransitionPointer2TransitionName[ret] = Transition;
	this->setTransitionFragID(Transition, FragID);
	this->setTransitionRoleID(Transition, RoleID);
}

places_t Fragmentation::getPlaces(pnapi::PetriNet & Petrinet) const {
	places_t ret;
	
	PNAPI_FOREACH(p, Petrinet.getPlaces()) {
		ret.insert((*p)->getName());
	}
	return ret;
}

transitions_t Fragmentation::getPlaceTransitions(pnapi::PetriNet & Petrinet, const place_t & Place) const {
	return setUnion(getPlacePreset(Petrinet, Place), getPlacePostset(Petrinet, Place));
}

transitions_t Fragmentation::getPlacePreset(pnapi::PetriNet & Petrinet, const place_t & Place) const {
	transitions_t ret;
	
	PNAPI_FOREACH(t, Petrinet.findPlace(Place)->getPreset()) {
		ret.insert((*t)->getName());
	}
	return ret;
}

transitions_t Fragmentation::getPlacePostset(pnapi::PetriNet & Petrinet, const place_t & Place) const {
	transitions_t ret;
	PNAPI_FOREACH(t, Petrinet.findPlace(Place)->getPostset()) {
		ret.insert((*t)->getName());
	}
	return ret;
}

transitions_t Fragmentation::getTransitions(pnapi::PetriNet & Petrinet) const {
	transitions_t ret;
	
	PNAPI_FOREACH(t, Petrinet.getTransitions()) {
		ret.insert((*t)->getName());
	}
	return ret;
}

places_t Fragmentation::getTransitionPlaces(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
	return setUnion(getTransitionPreset(Petrinet, Transition), getTransitionPostset(Petrinet, Transition));
}

places_t Fragmentation::getTransitionPreset(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
	places_t ret;
	
	PNAPI_FOREACH(p, Petrinet.findTransition(Transition)->getPreset()) {
		ret.insert((*p)->getName());
	}
	return ret;
}

places_t Fragmentation::getTransitionPostset(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
	places_t ret;
	
	PNAPI_FOREACH(p, Petrinet.findTransition(Transition)->getPostset()) {
		ret.insert((*p)->getName());
	}
	return ret;
}

bool Fragmentation::isFreeChoice() {
	return this->mIsFreeChoice;
}

bool Fragmentation::hasCycles() {
	return this->mHasCycles;
}

size_t Fragmentation::getLolaCalls() {
	return this->mLolaCalls;
}

size_t Fragmentation::getDianeCalls() {
	return this->mDianeCalls;
}

size_t Fragmentation::getDianeForces() {
	return this->mOverallDianeForces;
}

size_t Fragmentation::getDianeAlternatives() {
	return this->mOverallDianeAlternatives;
}
