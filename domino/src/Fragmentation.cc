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

/*----------------------------------------------------------------------------.
| public global methods														  |
`----------------------------------------------------------------------------*/
Fragmentation::Fragmentation(pnapi::PetriNet &Petrinet) {
	this->ROLE_UNASSIGNED = -1;	
	this->PLACE_UNASSIGNED = "";
	this->GLOBALSTART_REACHED = "AL_1983_07_12";
	this->SERVICE_PLACE_PREFIX = "P_BOUND_";
	this->SERVICE_TRANSITION_PREFIX = "T_CON_";

	this->mCurrentDianeForces = 0;
	this->mOverallDianeForces = 0;
	this->mCurrentDianeAlternatives = 0;
	this->mOverallDianeAlternatives = 0;

	this->mInterfaceCorrections = 0;
	this->mFragmentConnections = 0;
	this->mPlacesInsert = 0;
	this->mTransitionsInsert = 0;
	this->mArcsInsert = 0;
	this->mPlacesDelete = 0;
	this->mTransitionsDelete = 0;
	this->mArcsDelete = 0;

	this->mLolaCalls = 0;
	this->mDianeCalls = 0;
	this->mIsFreeChoice = Petrinet.isFreeChoice();
	this->mHasCycles = false;
	
	this->mFragmentNet = NULL;
	this->mNet = &Petrinet;
	this->mSharedFragID = -1;
	assert(this->mSharedFragID > 0);
	this->mUsedFragIDs.clear();
	this->mGlobalStartPlace = PLACE_UNASSIGNED;
	
	this->init();
	
	this->mRoleFragmentsBuild = false;
	this->mUnassignedFragmentsProcessed = false;
	this->mServicesCreated = false;

}

/*----------------------------------------------------------------------------.
| private global methods													  |
`----------------------------------------------------------------------------*/
void Fragmentation::init() {
	status("init() called...");

	set<std::string> roles;
	places_t places;
	transitions_t transitions;
	pnapi::Place * place;
	pnapi::Transition * transition;
	size_t i;

	this->initColors();
	
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

/*----------------------------------------------------------------------------.
| main public methods														  |
`----------------------------------------------------------------------------*/
bool Fragmentation::buildRoleFragments() {
	assert(!this->mRoleFragmentsBuild);
	
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
	set < pair<frag_id_t, frag_id_t> > toReplace;
	bool ret;
	placeStatus_e placeStatus;
	
	ret = true;	

	status("..initiating places");
	PNAPI_FOREACH(p, this->mPlaces) {
		this->setPlaceMinFragID(*p, this->mSharedFragID);
	}
	status("..places initiated");

	status("..assigning roles to nodes");
	curFragID = getNextFragID();
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
			abort(6, "Fragmentation::buildRoleFragments(): transition %s has more than one role", (*t).c_str());
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
				this->setPlaceMinFragID(*p, this->mSharedFragID);				
			}
			else {
				curMinFragID = this->getPlaceMinFragID(*p);				
				this->setPlaceMinFragID(*p, std::min(curMinFragID, curFragID));
			}
		}
		status("......places connected with t processed");
		
		curFragID = getNextFragID();
		status("....transition %s processed", (*t).c_str());
	}
	status("..roles assigned to nodes");

	status("..refactoring fragments");
	for (curFragID = 0; curFragID < this->getMaxFragID(); curFragID++) {
		status("....processing fragment %d", curFragID);
		toReplace.clear();
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
					toReplace.insert( std::make_pair(transitionFragID, curFragID) );
				}
				else {
					this->setTransitionFragID(*t, minFragID);
				}	
			}
		}
		
		FOREACH(P, toReplace) {
			this->replaceFragIDs(P->first, P->second);
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

	status("..checking for bad structure(s)");
	transitionPlaces = this->getPlaces(*this->mNet);
	PNAPI_FOREACH(p, transitionPlaces) {
		placeStatus = this->getPlaceStatus(*p, false, this->ROLE_UNASSIGNED, this->ROLE_UNASSIGNED);
		if (placeStatus == PLACE_STATUS_BAD) {
			status(_cbad_("....%s %s"), _cbad_((*p).c_str()), _cbad_("is not usable"));
			ret = false;
		}
		else if (placeStatus == PLACE_STATUS_NOTBILATERAL) {
			status(_cwarning_("....%s %s"), _cwarning_((*p).c_str()), _cwarning_("is not bilateral"));
		}
	}

	if (!ret) {
		status(_cbad_("..bad structure(s) found"));
	}
	else {
		status(_cgood_("..no bad structure(s) found"));
	}

	this->mRoleFragmentsBuild = ret;
	status("buildRoleFragments() finished");
	return ret;
}

bool Fragmentation::processUnassignedFragments() {
	assert(this->mRoleFragmentsBuild);
	assert(!this->mUnassignedFragmentsProcessed);

	status("processUnassignedFragments() called...");

	bool ret = true;
	size_t unassignedFragments = 0;

	FOREACH(fragID, this->mUsedFragIDs) {
		assert(!this->isFragmentEmpty(*fragID));
		if (this->getFragmentRoleID(*fragID) == this->ROLE_UNASSIGNED) {
			unassignedFragments++;
		}
	}
	status("..%d unassigned fragments", unassignedFragments);

	while ((unassignedFragments > 0) && (ret == true)) {
		FOREACH(fragID, this->mUsedFragIDs) {
			assert(!this->isFragmentEmpty(*fragID));
			if (this->getFragmentRoleID(*fragID) == this->ROLE_UNASSIGNED) {
				unassignedFragments--;				
				if (!processUnassignedFragment(*fragID)) {
					ret = false;	
				}
				break;				
			}
		}
	}

	this->mUnassignedFragmentsProcessed = ret;
	
	if (this->mOverallDianeForces != 0) {
		status(_cimportant_("..%d forced decision(s)"), this->mOverallDianeForces);
		status(_cimportant_("....with %d alternatives"), this->mOverallDianeAlternatives);
	}

	status("processUnassignedFragments() finished");
	return ret;
}

bool Fragmentation::buildServices() {
	assert(this->mUnassignedFragmentsProcessed);
	assert(!this->mServicesCreated);

	status("buildServices() called...");

	bool ret;
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
	frag_id_t newFragID;
	map<place_t, unsigned int>::const_iterator curPlaceMaxToken;
	size_t reactivatingCount;
	unsigned int curPlaceTokens;
	bool changed;
	multimap< role_id_t, frag_id_t > roleFragments;
	pair<multimap< role_id_t, frag_id_t >::iterator, multimap< role_id_t, frag_id_t >::iterator> curRoleFragments;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;
	Tarjan tarjan(*this->mNet);

	ret = true;

	tarjan.calculateSCC();
	this->mHasCycles = tarjan.hasNonTrivialSCC();

	status(_cimportant_("..decomposition contains non trivial SCC: %d"), this->mHasCycles);
	status(_cimportant_("..workflow is free choice: %d"), this->mIsFreeChoice);

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
			//no reactiviating transitions
			assert(!((tarjan.isNonTrivialSCC(tarjan.getNodeSCC(*t))) && (reactivatingCount == 0)));
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
					if (curPlaceMaxToken->second > curPlaceTokens) {
						curPlaceTokens = curPlaceMaxToken->second;
					}
				}
				assert(curPlaceTokens != 0);
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
			this->addPlaceFragID(newPlace, this->getTransitionFragID((*p).second));
		}
	}

	if (tarjan.hasNonTrivialSCC()) {
		status("..reactivatings:");
		FOREACH(p, reactivatings) {
			newPlace = this->SERVICE_PLACE_PREFIX + (*p).first;
			status("....%s --> %s", (*p).second.c_str(), (*p).first.c_str());
			this->createArc((*p).second, newPlace);
			this->addPlaceFragID(newPlace, this->getTransitionFragID((*p).second));
		}
	}

	this->connectFragments();

	status("..connecting unconnected fragments");
	changed = false;
	FOREACH (f_b, this->mUsedFragIDs) {
		assert(!this->isFragmentEmpty(*f_b));
		roleFragments.insert( std::make_pair(this->getFragmentRoleID(*f_b), *f_b) );
	}
	for (roleID2RoleName_t::iterator curRole=this->mRoleID2RoleName.begin(); curRole!=this->mRoleID2RoleName.end(); ++curRole) {
		assert(roleFragments.count(curRole->first) != 0);
		
		if (roleFragments.count(curRole->first) > 1) {
			changed = true;

			status(_cimportant_("....role %s has %d fragments"), curRole->second.c_str(), roleFragments.count(curRole->first));

			newFragID = this->getNextFragID();
			std::stringstream curRoleID;
			curRoleID << curRole->first;

			newTransition = this->SERVICE_TRANSITION_PREFIX + curRoleID.str();
			newPlace = "P_" + newTransition;

			this->createTransition(newTransition, newFragID, curRole->first);
			this->createPlace(newPlace, newFragID, curRole->first);
			this->createArc(newPlace, newTransition);

			curRoleFragments = roleFragments.equal_range(curRole->first);
			for (multimap< role_id_t, frag_id_t >::iterator curRoleFragment=curRoleFragments.first; curRoleFragment!=curRoleFragments.second; ++curRoleFragment) {
				status("......handle fragment %d", curRoleFragment->second);
				
				place_t startplace = "";

				curFragTransitions = this->mFragID2Transitions.equal_range(curRoleFragment->second);
				for (fragID2Transitions_t::iterator curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
					if (restrictions.find(curFragTransition->second) != restrictions.end()) {
						status("........use restricted place");
						startplace = this->SERVICE_PLACE_PREFIX + curFragTransition->second;
						break;
					}
					else {
						transitionPlaces = this->getTransitionPreset(*this->mNet, curFragTransition->second);
						FOREACH(p, transitionPlaces) {
							if (this->mPlaceName2PlacePointer[*p]->getTokenCount() != 0) {
								status("........use bounded place");
								startplace = *p;
								break;
							}
						}
						if (startplace != "") {break;}
					}
				}

				assert(startplace != "");
				this->createArc(newTransition, startplace);
				this->addPlaceFragID(startplace, newFragID);
			}
		}

	}

	if (changed) {
		this->connectFragments();
	}

	this->mServicesCreated = ret;

	status("buildServices() finished");
	return ret;
}

/*----------------------------------------------------------------------------.
| public petrinet methods													  |
`----------------------------------------------------------------------------*/
pnapi::PetriNet Fragmentation::createPetrinetByRoleID(const role_id_t RoleID) {
	assert(this->mServicesCreated);
	
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
			assert(this->getPlacePostset(*this->mNet, curRolePlace->second).size() != 0);
			if (this->getTransitionRoleID(*(this->getPlacePostset(*this->mNet, curRolePlace->second).begin())) == RoleID) {
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

/*----------------------------------------------------------------------------.
| private petrinet methods													  |
`----------------------------------------------------------------------------*/
void Fragmentation::createPetrinetByFragID(const frag_id_t FragID) {
	assert(!this->isFragmentEmpty(FragID));

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

/*----------------------------------------------------------------------------.
| private diane methods														  |
`----------------------------------------------------------------------------*/
bool Fragmentation::processUnassignedFragment(const frag_id_t FragID) {
	assert(this->getFragmentRoleID(FragID) == this->ROLE_UNASSIGNED);

	bool ret = true;

	status("..processUnassignedFragment(%d) called...", FragID);

	time_t start_time, end_time;
    string outputParam;
    string fileName;
	string curPartFileName;
	std::stringstream number;
	std::ofstream DianeTemp;
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	places_t placesToDo;

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

	if (fileExists(fileName) && args_info.noOverride_flag) {
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
        // call Diane and open a pipe
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

	assert(this->mFragmentUnprocessedNodes.size() == 0);

	if (!this->processDianeFragmentation()) {
		ret = false;
	}
	else {
		assert(this->mFragID2Transitions.count(FragID) == 0);
		assert(this->mFragID2Places.count(FragID) == 0);
	}

	status("..processUnassignedFragment(%d) finished", FragID);
	return ret;
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
	
	status("......places:");
	Places = this->getPlaces(net);
	PNAPI_FOREACH(p, Places) {
		status("........%s", (*p).c_str());
		placesToDo.insert(*p);
	}
	//handle interface as places...
	status("......interface:");
	interface = net.getInterface().getAsynchronousLabels();
	PNAPI_FOREACH(i, interface) {
		status("........%s", (*i)->getName().c_str());
		placesToDo.insert((*i)->getName());
	}

	PNAPI_FOREACH(p, placesToDo) {
		this->addPlaceDianeID(*p, this->mDianeFragments);
		this->mFragmentUnprocessedNodes.erase(*p);
	}

	status("......transitions:");
	Transitions = this->getTransitions(net);
	PNAPI_FOREACH(t, Transitions) {
		status("........%s", (*t).c_str());
		this->setTransitionDianeID(*t, this->mDianeFragments);
		this->mFragmentUnprocessedNodes.erase(*t);
	}
	
	status("....readDianeOutputFile(%s) finished", _cfilename_(FileName));
}

bool Fragmentation::processDianeFragmentation() {
	status("....processDianeFragmentation() called...");

	set<diane_id_t> toDo;
	set< pair<diane_id_t, role_id_t> > toSet;
	role_id_t bestConnection;
	bool progress;
	bool ret;

	ret = true;

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
				if (bestConnection != this->ROLE_UNASSIGNED) {
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
					if (bestConnection != this->ROLE_UNASSIGNED) {
						this->setDianeFragmentConnection(d, bestConnection);
						toDo.erase(d);
						progress = true;
						break;
					}
				}
			}
			if (!progress) {
				ret = false;
				status(_cbad_("......no progress"));
				break;
			}
		}
		
	}

	if (ret) {

		this->mOverallDianeForces += this->mCurrentDianeForces;
		this->mOverallDianeAlternatives += this->mCurrentDianeAlternatives;

		if (this->mCurrentDianeForces != 0) {
			status("......%d forced decisions", this->mCurrentDianeForces);
			status("........%d alternatives", this->mCurrentDianeAlternatives);
		}

	}

	status("....processDianeFragmentation() finished");
	return ret;
}

role_id_t Fragmentation::getBestConnectionForDianeFragment(const diane_id_t DianeID, const bool Force) {
	status("......getBestConnectionForDianeFragment(%d, %d) called...", DianeID, Force);

	transitions_t transitionInputProcessed;
	transitions_t transitionOutputProcessed;
	transitions_t placeTransitions;
	vector<role_id_t> candidates;
	roles_t unpreferredCandidates;
	roles_t knownCandidates;
	roles_t toDelete;
	roles_t coveredRoleIDs;
	roles_t adjacentRoleIDs;
	multimap<role_id_t, frag_id_t> roleFragments;	
	map<role_id_t, size_t> inputRoleIDs;
	map<role_id_t, size_t> outputRoleIDs;
	map<role_id_t, size_t>::iterator elem;
	size_t curSize;
	size_t count;
	size_t maxRoleFragments;
	role_id_t maxRoleID;
	role_id_t transitionRoleID;
	frag_id_t transitionFragID;
	bool found;
	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<multimap<role_id_t, frag_id_t>::iterator, multimap<role_id_t, frag_id_t>::iterator> curRoleFragments;
	role_id_t ret;
	validStatus_e assignmentStatus;

	adjacentRoleIDs.clear();
	coveredRoleIDs.clear();
	transitionInputProcessed.clear();
	transitionOutputProcessed.clear();

	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
		inputRoleIDs.clear();
		outputRoleIDs.clear();
		
		//for all transitions (preset)
		placeTransitions = this->getPlacePreset(*this->mNet, curDianePlace->second);	
		PNAPI_FOREACH(t, placeTransitions) {
			if (transitionInputProcessed.find(*t) == transitionInputProcessed.end()) {
				//tranisition is unprocessed
				transitionRoleID = this->getTransitionRoleID(*t);
				if (transitionRoleID != this->ROLE_UNASSIGNED) {
					//handle RoleID
					elem = inputRoleIDs.find(transitionRoleID);				
					curSize = 1;						
					if (elem != inputRoleIDs.end()) {
						curSize = elem->second;
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
			//add
			transitionInputProcessed.insert(*t);
		} //transitions in Preset

		for (map<role_id_t, size_t>::iterator inputRoleID=inputRoleIDs.begin(); inputRoleID!=inputRoleIDs.end(); ++inputRoleID) {
			adjacentRoleIDs.insert(inputRoleID->first);
		}
	
		for (map<role_id_t, size_t>::iterator inputRoleID=inputRoleIDs.begin(); inputRoleID!=inputRoleIDs.end(); ++inputRoleID) {
			if (inputRoleID->second > 1) {
				coveredRoleIDs.insert(inputRoleID->first);
			}
		}


		//for all transitions (postset)
		placeTransitions = this->getPlacePostset(*this->mNet, curDianePlace->second);	
		PNAPI_FOREACH(t, placeTransitions) {
			if (transitionOutputProcessed.find(*t) == transitionOutputProcessed.end()) {
				//tranisition is unprocessed
				transitionRoleID = this->getTransitionRoleID(*t);
				if (transitionRoleID != this->ROLE_UNASSIGNED) {
					//handle RoleID
					elem = outputRoleIDs.find(transitionRoleID);				
					curSize = 1;						
					if (elem != outputRoleIDs.end()) {
						curSize = elem->second;
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
			//add
			transitionOutputProcessed.insert(*t);
		} //transitions in Preset
		for (map<role_id_t, size_t>::iterator outputRoleID=outputRoleIDs.begin(); outputRoleID!=outputRoleIDs.end(); ++outputRoleID) {
			adjacentRoleIDs.insert(outputRoleID->first);
		}
		
		for (map<role_id_t, size_t>::iterator outputRoleID=outputRoleIDs.begin(); outputRoleID!=outputRoleIDs.end(); ++outputRoleID) {
			if (outputRoleID->second > 1) {
				coveredRoleIDs.insert(outputRoleID->first);
			}
		}

	}

	//auswerten
	ret = this->ROLE_UNASSIGNED;

	knownCandidates.clear();
	candidates.clear();
	unpreferredCandidates.clear();

	FOREACH(r, coveredRoleIDs) {
		candidates.push_back(*r);
		knownCandidates.insert(*r);
	}
	
	status("........Diane-Fragment has %d adjacent roles", adjacentRoleIDs.size());
	status("..........%d are covered roles", coveredRoleIDs.size());
	FOREACH(r, adjacentRoleIDs) {
		if (knownCandidates.find(*r) == knownCandidates.end()) {
			candidates.push_back(*r);
			knownCandidates.insert(*r);
		}
	}
	
	if (knownCandidates.size() == 0) {
		status("........Diane-Fragment has no candidates -> skip fragment");
	}
	else {
		status("........Diane-Fragment has %d possible candidates", knownCandidates.size());
		toDelete.clear();
		FOREACH(r, knownCandidates) {
			assignmentStatus = this->getDianeAssignementValidStatus(DianeID, *r);
			if (assignmentStatus == VALID_BAD) {
				status("..........check role %d: %s", *r, _cbad_("FAILED"));
				toDelete.insert(*r);
			}
			else if (assignmentStatus == VALID_TODO) {
				unpreferredCandidates.insert(*r);
			}
			else {
				assert(assignmentStatus == VALID_OK);
				status("..........check role %d: %s", *r, _cgood_("PASSED"));
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

		status("........Diane-Fragment has %d real candidates", knownCandidates.size());
		status("..........%d are unpreferred", unpreferredCandidates.size());
		if ((knownCandidates.size() > 1) && (!Force)) {
			status("........Diane-Fragment should not be decided -> skip fragment");
			ret = this->ROLE_UNASSIGNED;
		}
		else if (knownCandidates.size() > 0) {
			if ((knownCandidates.size() - unpreferredCandidates.size() == 1) || (knownCandidates.size() == unpreferredCandidates.size())) {
				ret = *knownCandidates.begin();
			}
			else {
				count = maxRoleFragments = 0;
				maxRoleID = this->ROLE_UNASSIGNED;
			
				for (vector<role_id_t>::iterator curRole=candidates.begin(); curRole!=candidates.end(); ++curRole) {
					if (unpreferredCandidates.find(*curRole) == unpreferredCandidates.end()) {
						count = roleFragments.count(*curRole);
						if (maxRoleFragments < count) {
							maxRoleFragments = count;
							maxRoleID = *curRole;
						}
					}
				}
				assert(maxRoleID != this->ROLE_UNASSIGNED);
				ret = maxRoleID;
			}
		}

	}


	if (ret != this->ROLE_UNASSIGNED) {
		if (Force) {
			status("........Diane-Fragment is %s to connection %d", _cwarning_("FORCED"), ret);
			this->mCurrentDianeForces++;
			this->mCurrentDianeAlternatives += knownCandidates.size();
		}
		else {
			status("........Diane-Fragment becomes connection %d", ret);
		}
	}

	status("......getBestConnectionForDianeFragment(%d, %d) finished", DianeID, Force);
	return ret;
}

validStatus_e Fragmentation::getDianeAssignementValidStatus(const diane_id_t DianeID, const role_id_t RoleID) {
	//status("............getDianeAssignementValidStatus(%d, %d) called...", DianeID, RoleID);

	role_id_t predecessor;
	role_id_t successor;
	transitions_t placePreset;
	transitions_t placePostset;
	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<dianeID2Transitions_t::iterator, dianeID2Transitions_t::iterator> curDianeTransitions;
	validStatus_e ret;
	placeStatus_e placeStatus;

	curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
	curDianeTransitions = this->mDianeID2Transitions.equal_range(DianeID);
	ret = VALID_OK;

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

		placeStatus = this->getPlaceStatus(curDianePlace->second, false, predecessor, successor);
		if (placeStatus == PLACE_STATUS_BAD) {
			ret = VALID_BAD;
			break;
		}
		else if (placeStatus == PLACE_STATUS_NOTBILATERAL) {
			ret = VALID_TODO;
		}
	}

	//status("............getDianeAssignementValidStatus(%d, %d) finished", DianeID, RoleID);
	return ret;
}

void Fragmentation::setDianeFragmentConnection(const diane_id_t DianeID, role_id_t Connection) {
	status("......setDianeFragmentConnection(%d, %d) called...", DianeID, Connection);

	pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces;
	pair<dianeID2Transitions_t::iterator, dianeID2Transitions_t::iterator> curDianeTransitions;
	places_t placesToDo;
	transitions_t transitionsToDo;
	frag_id_t newFragID = this->getNextFragID();

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
		this->setTransitionRoleID(*t, Connection);
		this->setTransitionFragID(*t, newFragID);
	}

	FOREACH(p, placesToDo) {
		this->addPlaceRoleID(*p, Connection);
		this->addPlaceFragID(*p, newFragID);
		this->deletePlaceFragID(*p, this->mCurProcessingFragID);
	}

	FOREACH(p, placesToDo) {
		if (!this->hasUnassignedTransitions(*p)) {
			this->deletePlaceRoleID(*p, this->ROLE_UNASSIGNED);
		}
	}

	this->connectFragments();

	status("......setDianeFragmentConnection(%d, %d) finished", DianeID, Connection);
}

/*----------------------------------------------------------------------------.
| public properties methods													  |
`----------------------------------------------------------------------------*/
validStatus_e Fragmentation::isFragmentationValid(const bool Repair) {
	assert(this->mRoleFragmentsBuild);
	
	status("isFragmentationValid(%d) called...", Repair);

	places_t places;
	placeStatus_e placeStatus;
	bool hitToDo;
	bool hitBad;
	validStatus_e ret;

	ret = VALID_OK;
	hitToDo = hitBad = false;

	places = this->getPlaces(*this->mNet);
	PNAPI_FOREACH(p, places) {
		placeStatus = this->getPlaceStatus(*p, Repair, this->ROLE_UNASSIGNED, this->ROLE_UNASSIGNED);
		if (placeStatus == PLACE_STATUS_NOTBILATERAL) {
			hitToDo = true;
		}
		else if (placeStatus == PLACE_STATUS_BAD) {
			hitBad = true;
		}
	}	

	if (hitBad) {
		ret = VALID_BAD;
	}
	if (hitToDo) {
		ret = VALID_TODO;
	}
	status("isFragmentationValid(%d) finished", Repair);
	return ret;
}

/*----------------------------------------------------------------------------.
| private properties methods												  |
`----------------------------------------------------------------------------*/
placeStatus_e Fragmentation::getPlaceStatus(const place_t & Place, const bool Repair, const role_id_t Predecessor, const role_id_t Successor) {
	//status("....getPlaceStatus(%s, %d, %d, %d) called...", Place.c_str(), Repair, Predecessor, Successor);

	roles_t predecessors;
	roles_t successors;
	transitions_t placeTransitions;
	placeStatus_e ret;
	frag_id_t sucFragID;
	role_id_t sucRoleID;
	std::stringstream sucRoleIDString;
	set< pnapi::Arc *> arcsToDelete;
	set< pair<node_t, node_t> > arcsToCreate;
	
	ret = PLACE_STATUS_BAD;

	sucFragID = sucRoleID = -1;	

	predecessors.insert(Predecessor);
	successors.insert(Successor);
	
	placeTransitions = this->getPlacePreset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		predecessors.insert(this->getTransitionRoleID(*t));
	}

	placeTransitions = this->getPlacePostset(*this->mNet, Place);
	FOREACH(t, placeTransitions) {
		successors.insert(this->getTransitionRoleID(*t));
		sucFragID = this->getTransitionFragID(*t);
		sucRoleID = this->getTransitionRoleID(*t);
	}
	
	sucRoleIDString << sucRoleID;

	predecessors.erase(this->ROLE_UNASSIGNED);
	successors.erase(this->ROLE_UNASSIGNED);

	if (successors.size() > 1)  {
		ret = PLACE_STATUS_BAD;
	}
	else if ((predecessors.size() == 0) && (successors.size() == 0)) {
		ret = PLACE_STATUS_UNASSIGNED;
	}
	else if ((predecessors.size() == 0) && (successors.size() != 0)) {
		ret = PLACE_STATUS_START;
	}
	else if ((successors.size() == 0) && (predecessors.size() != 0)) {
		ret = PLACE_STATUS_END;
	}
	else if ((predecessors.size() == 1) && (successors.size() == 1)) {
		if (setIntersection(predecessors, successors).size() == 1) {
			ret = PLACE_STATUS_INTERN;
		}
		else {
			ret = PLACE_STATUS_SHARED;
		}
	}
	else if (predecessors.size() > 1) {
		if (!Repair) {
			ret = PLACE_STATUS_NOTBILATERAL;
		}
		else {
			this->mInterfaceCorrections++;
			role_id_t curRoleID;
			frag_id_t curFragID;	
			place_t sucPlace;			
			place_t newPlace;
			transition_t newTransition;
	
			arcsToCreate.clear();
			arcsToDelete.clear();

			sucPlace = Place + "_PART_" + sucRoleIDString.str();
			this->createPlace(sucPlace, sucFragID, sucRoleID);

			placeTransitions = this->getPlacePreset(*this->mNet, Place);
			FOREACH(t, placeTransitions) {
				curFragID = this->getTransitionFragID(*t);
				curRoleID = this->getTransitionRoleID(*t);

				std::stringstream curRoleIDString;
				curRoleIDString << curRoleID;

				if (curRoleID == sucRoleID) {
					newPlace = sucPlace;
				}
				else {
					newPlace = Place + "_PART_" + curRoleIDString.str();
					newTransition = "T_" + newPlace;
					if (this->mNet->findPlace(newPlace) == NULL) {
						this->createPlace(newPlace, curFragID, curRoleID);
						this->addPlaceRoleID(newPlace, sucRoleID);
						this->addPlaceFragID(newPlace, sucFragID);
						this->createTransition(newTransition, sucFragID, sucRoleID);
							arcsToCreate.insert( std::make_pair(newPlace, newTransition) );
							arcsToCreate.insert( std::make_pair(newTransition, sucPlace) );
					}
				}

				arcsToCreate.insert( std::make_pair(*t, newPlace) );
				arcsToDelete.insert(this->mNet->findArc(*this->mNet->findNode(*t), *this->mNet->findNode(Place)));
			}

			placeTransitions = this->getPlacePostset(*this->mNet, Place);
			FOREACH(t, placeTransitions) {
				assert(sucRoleID == this->getTransitionRoleID(*t));
				curFragID = this->getTransitionFragID(*t);

				this->addPlaceFragID(sucPlace, curFragID);

				arcsToCreate.insert( std::make_pair(sucPlace, *t) );
				arcsToDelete.insert(this->mNet->findArc(*this->mNet->findNode(Place), *this->mNet->findNode(*t)));
			}

			FOREACH(p, arcsToCreate) {
				this->createArc((*p).first, (*p).second);
			}

			FOREACH(p, arcsToDelete) {
				this->mNet->deleteArc(**p);
				this->mArcsDelete++;
			}

			this->deletePlace(Place);

			ret = PLACE_STATUS_SHARED;
		}
	}

	//status("....getPlaceStatus(%s, %d, %d, %d): %d finished", Place.c_str(), Repair, Predecessor, Successor, ret);
	return ret;
}

/*----------------------------------------------------------------------------.
| public color methods														  |
`----------------------------------------------------------------------------*/
void Fragmentation::colorFragmentsByFragID(const bool ColorUnassigned) {
	assert(this->mRoleFragmentsBuild);
	
	pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces;
	pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions;	
	size_t curColorID;
	string curColorName;

	this->eraseColors();
	curColorID = 0;

	FOREACH(fragID, this->mUsedFragIDs) {
		assert(!this->isFragmentEmpty(*fragID));
		if ((this->getFragmentRoleID(*fragID) != this->ROLE_UNASSIGNED) || ColorUnassigned) {
			if (curColorID == this->mMaxColors) {
				abort(8, "Fragmentation::colorFragmentsByFragID(%d): all %d colors used", ColorUnassigned, this->mMaxColors);
			}
			curColorName = this->getColorName(curColorID);

			curFragPlaces = this->mFragID2Places.equal_range(*fragID);	
			for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
				if (this->mPlace2FragIDs.count(curFragPlace->second) == 1) {				
					this->mPlaceName2PlacePointer[curFragPlace->second]->setColor(curColorName);
				}
			}
		
			curFragTransitions = this->mFragID2Transitions.equal_range(*fragID);
			for (fragID2Transitions_t::iterator curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
				this->mTransitionName2TransitionPointer[curFragTransition->second]->setColor(curColorName);
			}
			
			curColorID++;
		}
	}
}

void Fragmentation::colorFragmentsByRoleID(const bool ColorUnassigned) {
	assert(this->mRoleFragmentsBuild);
	
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
			abort(8, "Fragmentation::colorFragmentsByRoleID(%d): all %d colors used", ColorUnassigned, this->mMaxColors);
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
