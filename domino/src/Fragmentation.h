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

#ifndef __FRAGMENTATION_H
#define __FRAGMENTATION_H

#include <map>
#include <stack>
#include <vector>
#include <utility>
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "types.h"
#include "util.h"
#include "Output.h"
#include "verbose.h"
#include "cmdline.h"

#include "Tarjan.h"

extern gengetopt_args_info args_info;
extern map<place_t, unsigned int> Place2MaxTokens;

using std::map;
using std::multimap;
using std::pair;
using std::set;
using std::string;
using std::stack;
using std::vector;


typedef map<string, role_id_t> roleName2RoleID_t;
typedef map<role_id_t, string> roleID2RoleName_t;
typedef map<transition_t, frag_id_t> transition2FragID_t;
typedef multimap<frag_id_t, transition_t> fragID2Transitions_t;
typedef map<transition_t, role_id_t> transition2RoleID_t;
typedef multimap<role_id_t, transition_t> roleID2Transitions_t;
typedef map<place_t, frag_id_t> place2MinFragID_t;
typedef multimap<frag_id_t, place_t> minFragID2Places_t;
typedef multimap<place_t, frag_id_t> place2FragIDs_t;
typedef multimap<frag_id_t, place_t> fragID2Places_t;
typedef multimap<place_t, role_id_t> place2RoleIDs_t;
typedef multimap<role_id_t, place_t> roleID2Places_t;
typedef map<transition_t, diane_id_t> transition2DianeID_t;
typedef multimap<diane_id_t, transition_t> dianeID2Transitions_t;
typedef multimap<place_t, diane_id_t> place2DianeIDs_t;
typedef multimap<diane_id_t, place_t> dianeID2Places_t;
typedef map<place_t, pnapi::Place *> placeName2PlacePointer_t;
typedef map<pnapi::Place *, place_t> placePointer2PlaceName_t;
typedef map<transition_t, pnapi::Transition *> transitionName2TransitionPointer_t;
typedef map<pnapi::Transition *, transition_t> transitionPointer2TransitionName_t;

typedef map<size_t, string> colorID2ColorName_t;

class Fragmentation {

	private:

		role_id_t ROLE_UNASSIGNED;
		place_t PLACE_UNASSIGNED;
		place_t GLOBALSTART_REACHED;
		place_t SERVICE_PLACE_PREFIX;
		transition_t SERVICE_TRANSITION_PREFIX;

		//___roles___
		//RoleName -> RoleID
		roleName2RoleID_t mRoleName2RoleID;
		//RoleID -> RoleName
		roleID2RoleName_t mRoleID2RoleName;		
		
		//___transitions___
		transitions_t mTransitions;
		//Transition -> FragID
		transition2FragID_t mTransition2FragID;
		//FragID --> Transitions
		fragID2Transitions_t mFragID2Transitions;
		//Transition -> RoleID
		transition2RoleID_t mTransition2RoleID;
		//RoleID --> Transitions
		roleID2Transitions_t mRoleID2Transitions;
		//TransitionName -> TransitionPointer
		transitionName2TransitionPointer_t mTransitionName2TransitionPointer;
		//TransitionPointer -> TransitionName
		transitionPointer2TransitionName_t mTransitionPointer2TransitionName;

		//__places___
		places_t mPlaces;
		//Place -> MinFragID
		place2MinFragID_t mPlace2MinFragID;
		//MinFragID --> Places
		minFragID2Places_t mMinFragID2Places;
		//Place --> FragIDs
		place2FragIDs_t mPlace2FragIDs;
		//FragID --> Places
		fragID2Places_t mFragID2Places;
		//Place --> RoleIDs
		place2RoleIDs_t mPlace2RoleIDs;
		//RoleID --> Places
		roleID2Places_t mRoleID2Places;
		//PlaceName -> PlacePointer
		placeName2PlacePointer_t mPlaceName2PlacePointer;
		//PlacePointer -> PlaceName
		placePointer2PlaceName_t mPlacePointer2PlaceName;


		//local
		place_t mGlobalStartPlace;
		pnapi::PetriNet *mNet;
		size_t mMaxFragID;
		size_t mMaxColors;
		bool mRoleFragmentsBuild;
		bool mUnassignedFragmentsProcessed;
		bool mServicesCreated;
		size_t mLolaCalls;
		bool mIsFreeChoice;
		bool mHasCycles;
		//ColorID -> ColorName
		colorID2ColorName_t mColorID2ColorName;

		//for Diane fragmentation
		pnapi::PetriNet *mFragmentNet;
		set<node_t> mFragmentUnprocessedNodes;
		size_t mDianeFragments;
		size_t mCurrentDianeForces;
		size_t mOverallDianeForces;
		size_t mCurrentDianeAlternatives;
		size_t mOverallDianeAlternatives;
		size_t mDianeCalls;
		frag_id_t mCurProcessingFragID;
		//Transition -> DianeID
		transition2DianeID_t mTransition2DianeID;
		//DianeID --> Transitions
		dianeID2Transitions_t mDianeID2Transitions;
		//Place --> FragIDs
		place2DianeIDs_t mPlace2DianeIDs;
		//FragID --> Places
		dianeID2Places_t mDianeID2Places;

		void deletePlaceRoleID(const place_t &, const role_id_t);
		void deletePlaceFragID(const place_t &, const frag_id_t);

		void setPlaceMinFragID(const place_t &, const frag_id_t);
		frag_id_t getPlaceMinFragID(const place_t &);

		bool hasUnassignedTransitions(const place_t &);

		void addPlaceRoleID(const place_t &, const role_id_t);
		void addPlaceFragID(const place_t &, const frag_id_t);

		void replaceFragIDs(const frag_id_t, const frag_id_t);
		void connectFragments();	

		void setTransitionRoleID(const transition_t &, const role_id_t, const bool = true);
		void setTransitionFragID(const transition_t &, const frag_id_t, const bool = true);

		void init();
		void initColors();

		void createPlace(const place_t &, const frag_id_t, const role_id_t);
		void createTransition(const transition_t &, const frag_id_t, const role_id_t);
		void createArc(const node_t &, const node_t &);

		bool processUnassignedFragment(const frag_id_t);
		void createPetrinetByFragID(const frag_id_t);
		void readDianeOutputFile(const string &);
		bool processDianeFragmentation();
		void setTransitionDianeID(const transition_t &, const diane_id_t, const bool = true);
		void addPlaceDianeID(const place_t &, const diane_id_t);
		diane_id_t getTransitionDianeID(const transition_t &);
		pair<role_id_t, frag_id_t> getBestConnectionForDianeFragment(const diane_id_t, const bool);
		void setDianeFragmentConnection(const diane_id_t, pair<role_id_t, frag_id_t>);
		roles_t getTopRoleIDs(const map<role_id_t, size_t> & RoleMap) const;
		void replaceSharedStructure(const frag_id_t, const place_t &);

		bool willDianeAssignementValid(const diane_id_t, const role_id_t);

		void addTransitionPredecessors(stack<transition_t> &, transitions_t &, const transition_t &);

		places_t getPlaces(pnapi::PetriNet &) const;
		transitions_t getPlaceTransitions(pnapi::PetriNet &, const place_t &) const;
		transitions_t getPlacePreset(pnapi::PetriNet &, const place_t &) const;
		transitions_t getPlacePostset(pnapi::PetriNet &, const place_t &) const;
		transitions_t getTransitions(pnapi::PetriNet &) const;
		places_t getTransitionPlaces(pnapi::PetriNet &, const transition_t &) const;
		places_t getTransitionPreset(pnapi::PetriNet &, const transition_t &) const;
		places_t getTransitionPostset(pnapi::PetriNet &, const transition_t &) const;


		placeStatus_e getPlaceStatus(const place_t &, const bool, const role_id_t, const role_id_t);

	public:
		Fragmentation(pnapi::PetriNet &);
		~Fragmentation();
		
		role_id_t getTransitionRoleID(const transition_t &);
		frag_id_t getTransitionFragID(const transition_t &);
		role_id_t getFragmentRoleID(const frag_id_t);
		string getFragmentRoleName(const frag_id_t);
		role_id_t getRoleID(const string &);

		bool isSharedPlace(const place_t &);
		bool isStartTransition(const transition_t &);
		bool isFragmentEmpty(const frag_id_t);
		
		bool buildRoleFragments();
		bool processUnassignedFragments();
		bool buildServices();

		size_t getLolaCalls();
		size_t getDianeCalls();
		size_t getDianeForces();
		size_t getDianeAlternatives();
		bool hasCycles();
		bool isFreeChoice();
	
		validStatus_e isFragmentationValid(const bool = false);

		string fragmentsToString();

		pnapi::PetriNet createPetrinetByRoleID(const role_id_t);

		void eraseColors();
		void colorFragmentsByFragID(const bool = false);
		void colorFragmentsByRoleID(const bool = false);
		string getColorName(const size_t);

};

#endif //__FRAGMENTATION_H
