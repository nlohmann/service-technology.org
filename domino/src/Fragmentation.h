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
#include <config.h>

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

class Fragmentation {

	private:

		//constants
		role_id_t ROLE_UNASSIGNED;
		place_t PLACE_UNASSIGNED;

		//members
		//roles
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
		//places
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
		//colors
			//ColorID -> ColorName
			colorID2ColorName_t mColorID2ColorName;
		//global
			place_t mGlobalStartPlace;
			pnapi::PetriNet *mNet;
			frag_id_t mSharedFragID;
			set <frag_id_t> mUsedFragIDs;
			transitions_t mBilateralTransitions;
			size_t mMaxColors;
			bool mIsFreeChoice;
			bool mHasCycles;
		//status
			bool mRoleFragmentsBuild;
			bool mUnassignedFragmentsProcessed;
			bool mServicesCreated;
		//statistic
			size_t mInterfaceCorrections;
			size_t mBoundnessCorrections;
			size_t mBoundnessOrConnections;
			size_t mBoundnessAndConnections;
			size_t mMergings;
			size_t mFragmentConnections;
			size_t mArcweightCorrections;
			size_t mInitialMarkings;
			size_t mSelfreactivatings;
			size_t mPlacesInsert;
			size_t mTransitionsInsert;
			size_t mArcsInsert;
			size_t mPlacesDelete;
			size_t mTransitionsDelete;
			size_t mArcsDelete;
			size_t mRolesAnnotated;
			size_t mNotAnnotatedTransitions;
			bool mConcatenateAnnotationNecessary;
		//lola
			bool mLolaCalled;
		//diane
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

		//methods
			//global
				void init();
				void initColors();
				inline frag_id_t getMaxFragID() {
					assert(this->mUsedFragIDs.size() != 0);
					return *this->mUsedFragIDs.rbegin();
				}
				inline frag_id_t getNextFragID() {
					if (this->mUsedFragIDs.size() == 0) {
						return 0;
					}
					for (frag_id_t i=0; i<this->getMaxFragID(); ++i) {
						if (this->mUsedFragIDs.find(i) == this->mUsedFragIDs.end()) {
							return i;
						}
					}
					assert((this->getMaxFragID() + 1) > 0);
					return (this->getMaxFragID() + 1);
				}
			//member support
				//set/add
					inline void addPlaceRoleID(const place_t & Place, const role_id_t RoleID) {
						bool foundA = false;
						pair<place2RoleIDs_t::iterator, place2RoleIDs_t::iterator> curPlaceRoles = this->mPlace2RoleIDs.equal_range(Place);
						for (place2RoleIDs_t::iterator curPlaceRole=curPlaceRoles.first; curPlaceRole!=curPlaceRoles.second; ++curPlaceRole) {
							if (curPlaceRole->second == RoleID) {
								foundA = true;
								break;
							}
						}
						bool foundB = false;
						pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces = this->mRoleID2Places.equal_range(RoleID);
						for (roleID2Places_t::iterator curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
							if (curRolePlace->second == Place) {
								foundB = true;
								break;
							}
						}
						assert((foundA && foundB) == (foundA || foundB));
						if (!foundA) {
							this->mPlace2RoleIDs.insert( std::make_pair(Place, RoleID) );
							this->mRoleID2Places.insert( std::make_pair(RoleID, Place) );
						}
					}
					inline void addPlaceFragID(const place_t & Place, const frag_id_t FragID) {
						bool foundA = false;
						pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFrags = this->mPlace2FragIDs.equal_range(Place);
						for (place2FragIDs_t::iterator curPlaceFrag=curPlaceFrags.first; curPlaceFrag!=curPlaceFrags.second; ++curPlaceFrag) {
							if (curPlaceFrag->second == FragID) {
								foundA = true;
								break;
							}
						}
						bool foundB = false;
						pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces = this->mFragID2Places.equal_range(FragID);
						for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
							if (curFragPlace->second == Place) {
								foundB = true;
								break;
							}
						}
						assert((foundA && foundB) == (foundA || foundB));
						if (!foundA) {
							this->mUsedFragIDs.insert(FragID);
							this->mPlace2FragIDs.insert( std::make_pair(Place, FragID) );
							this->mFragID2Places.insert( std::make_pair(FragID, Place) );
						}
					}
					inline void addPlaceDianeID(const place_t & Place, const diane_id_t DianeID) {
						bool foundA = false;
						pair<place2DianeIDs_t::iterator, place2DianeIDs_t::iterator> curPlaceDianes = this->mPlace2DianeIDs.equal_range(Place);
						for (place2DianeIDs_t::iterator curPlaceDiane=curPlaceDianes.first; curPlaceDiane!=curPlaceDianes.second; ++curPlaceDiane) {
							if (curPlaceDiane->second == DianeID) {
								foundA = true;
								break;
							}
						}
						bool foundB = false;
						pair<dianeID2Places_t::iterator, dianeID2Places_t::iterator> curDianePlaces = this->mDianeID2Places.equal_range(DianeID);
						for (dianeID2Places_t::iterator curDianePlace=curDianePlaces.first; curDianePlace!=curDianePlaces.second; ++curDianePlace) {
							if (curDianePlace->second == Place) {
								foundB = true;
								break;
							}
						}
						assert((foundA && foundB) == (foundA || foundB));
						if (!foundA) {
							this->mPlace2DianeIDs.insert( std::make_pair(Place, DianeID) );
							this->mDianeID2Places.insert( std::make_pair(DianeID, Place) );
						}
					}
					inline void setPlaceMinFragID(const place_t & Place, const frag_id_t FragID) {
						minFragID2Places_t::iterator curFragPlace;
						place2MinFragID_t::iterator curMinFrag = this->mPlace2MinFragID.find(Place);
						if (curMinFrag != this->mPlace2MinFragID.end()) {
							bool found = false;
							pair<minFragID2Places_t::iterator, minFragID2Places_t::iterator> curFragPlaces = this->mMinFragID2Places.equal_range(curMinFrag->second);
							for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {	
								if (curFragPlace->second == Place) {
									found = true;
									break;
								}
							}
							assert(found);
							this->mMinFragID2Places.erase(curFragPlace);
							this->mPlace2MinFragID.erase(curMinFrag);
						}
						this->mPlace2MinFragID.insert( std::make_pair(Place, FragID) );
						this->mMinFragID2Places.insert( std::make_pair(FragID, Place) );
					}
					inline void setTransitionRoleID(const transition_t & Transition, const role_id_t RoleID, const bool Override = true) {
						roleID2Transitions_t::iterator curRoleTransition;
						transition2RoleID_t::iterator curRole = this->mTransition2RoleID.find(Transition);
						if (curRole != this->mTransition2RoleID.end()) {
							assert(Override);
							bool found = false;
							pair<roleID2Transitions_t::iterator, roleID2Transitions_t::iterator> roleTransitions = this->mRoleID2Transitions.equal_range(curRole->second);
							for (curRoleTransition=roleTransitions.first; curRoleTransition!=roleTransitions.second; ++curRoleTransition) {
								if (curRoleTransition->second == Transition) {
									found = true;
									break;
								}
							}
							assert(found);
							this->mRoleID2Transitions.erase(curRoleTransition);		
							this->mTransition2RoleID.erase(curRole);
						}
						this->mTransition2RoleID.insert( std::make_pair(Transition, RoleID) );
						this->mRoleID2Transitions.insert( std::make_pair(RoleID, Transition) );
					}
					inline void setTransitionFragID(const transition_t & Transition, const frag_id_t FragID, const bool Override = true) {
						fragID2Transitions_t::iterator curFragTransition;
						transition2FragID_t::iterator curTransition = this->mTransition2FragID.find(Transition);
						if (curTransition != this->mTransition2FragID.end()) {
							assert(Override);
							bool found = false;
							frag_id_t deleteFragID = -1;
							pair<fragID2Transitions_t::iterator, fragID2Transitions_t::iterator> curFragTransitions = this->mFragID2Transitions.equal_range(curTransition->second);		
							for (curFragTransition=curFragTransitions.first; curFragTransition!=curFragTransitions.second; ++curFragTransition) {
								if (curFragTransition->second == Transition) {
									found = true;
									deleteFragID = curFragTransition->first;
									break;
								}
							}
							assert(found);
							this->mFragID2Transitions.erase(curFragTransition);		
							this->mTransition2FragID.erase(curTransition);
							if (this->mFragID2Places.count(deleteFragID) + this->mFragID2Transitions.count(deleteFragID) == 0) {
								this->mUsedFragIDs.erase(deleteFragID);
							}
						}
						this->mUsedFragIDs.insert(FragID);
						this->mTransition2FragID.insert( std::make_pair(Transition, FragID) );
						this->mFragID2Transitions.insert( std::make_pair(FragID, Transition) );
					}
					inline void setTransitionDianeID(const transition_t & Transition, const diane_id_t DianeID) {
						dianeID2Transitions_t::iterator curDianeTransition;
						transition2DianeID_t::iterator curTransitionDiane = this->mTransition2DianeID.find(Transition);
						assert(curTransitionDiane == this->mTransition2DianeID.end());
						this->mTransition2DianeID.insert( std::make_pair(Transition, DianeID) );
						this->mDianeID2Transitions.insert( std::make_pair(DianeID, Transition) );
					}
				//get
					inline frag_id_t getPlaceMinFragID(const place_t & Place) {
						place2MinFragID_t::iterator curMinFrag = this->mPlace2MinFragID.find(Place);
						assert(curMinFrag != this->mPlace2MinFragID.end());
						return curMinFrag->second;
					}
					inline diane_id_t getTransitionDianeID(const transition_t & Transition) {
						transition2DianeID_t::iterator curTransitionDiane = this->mTransition2DianeID.find(Transition);
						assert(curTransitionDiane != this->mTransition2DianeID.end());
						return curTransitionDiane->second;
					}
				//delete
					inline void deletePlaceRoleID(const place_t & Place, const role_id_t RoleID) {
						place2RoleIDs_t::iterator curPlaceRole;
						roleID2Places_t::iterator curRolePlace;
						bool foundA = false;
						pair<place2RoleIDs_t::iterator, place2RoleIDs_t::iterator> curPlaceRoles = this->mPlace2RoleIDs.equal_range(Place);
						for (curPlaceRole=curPlaceRoles.first; curPlaceRole!=curPlaceRoles.second; ++curPlaceRole) {
							if (curPlaceRole->second == RoleID) {
								foundA = true;
								break;
							}
						}
						bool foundB = false;
						pair<roleID2Places_t::iterator, roleID2Places_t::iterator> curRolePlaces = this->mRoleID2Places.equal_range(RoleID);
						for (curRolePlace=curRolePlaces.first; curRolePlace!=curRolePlaces.second; ++curRolePlace) {
							if (curRolePlace->second == Place) {
								foundB = true;
								break;
							}
						}
						assert(foundA && foundB);
						this->mPlace2RoleIDs.erase(curPlaceRole);
						this->mRoleID2Places.erase(curRolePlace);
					}
					inline void deletePlaceFragID(const place_t & Place, const frag_id_t FragID) {
						place2FragIDs_t::iterator curPlaceFrag;
						fragID2Places_t::iterator curFragPlace;
						bool foundA = false;
						pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFrags = this->mPlace2FragIDs.equal_range(Place);
						for (curPlaceFrag=curPlaceFrags.first; curPlaceFrag!=curPlaceFrags.second; ++curPlaceFrag) {
							if (curPlaceFrag->second == FragID) {
								foundA = true;
								break;
							}
						}
						bool foundB = false;
						pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces = this->mFragID2Places.equal_range(FragID);
						for (curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
							if (curFragPlace->second == Place) {
								foundB = true;
								break;
							}
						}
						assert((foundA && foundB) == (foundA || foundB));
						if (foundA) {
							this->mPlace2FragIDs.erase(curPlaceFrag);
							this->mFragID2Places.erase(curFragPlace);
							if (this->mFragID2Places.count(FragID) + this->mFragID2Transitions.count(FragID) == 0) {
								this->mUsedFragIDs.erase(FragID);
							}
						}
					}
			//petrinet
				//create
				inline void createPlace(const place_t & Place, const frag_id_t FragID, const role_id_t RoleID, const size_t Tokens = 0) {
					if (this->mNet->containsNode(Place)) {
						abort(10, "Fragmentation::createPlace(%s): already known", Place.c_str());
					}
					pnapi::Place *ret = &this->mNet->createPlace(Place, Tokens);
					this->mPlaceName2PlacePointer[Place] = ret;
					this->mPlacePointer2PlaceName[ret] = Place;
					this->addPlaceFragID(Place, FragID);
					this->addPlaceRoleID(Place, RoleID);
					this->setPlaceMinFragID(Place, 0);
					this->mPlacesInsert++;
				}
				inline void createTransition(const transition_t & Transition, const frag_id_t FragID, const role_id_t RoleID) {
					if (this->mNet->containsNode(Transition)) {
						abort(10, "Fragmentation::createTransition(%s): already known", Transition.c_str());
					}
					pnapi::Transition *ret = &this->mNet->createTransition(Transition);
					this->mTransitionName2TransitionPointer[Transition] = ret;
					this->mTransitionPointer2TransitionName[ret] = Transition;
					this->setTransitionFragID(Transition, FragID);
					this->setTransitionRoleID(Transition, RoleID);
					this->mTransitionsInsert++;
				}
				inline void createArc(const node_t & Source, const node_t & Target, const size_t Weight = 1) {
					pnapi::Node *source = this->mNet->findNode(Source);
					pnapi::Node *target = this->mNet->findNode(Target);
					assert(source != NULL);
					assert(target != NULL);
					assert(this->mNet->findArc(*source, *target) == NULL);
					this->mNet->createArc(*source, *target, Weight);
					this->mArcsInsert++;
				}
				inline void mergePlaces(const place_t & Target, const place_t & Source, const bool SumMarkings) {
					pnapi::Node *source = this->mNet->findNode(Source);
					pnapi::Node *target = this->mNet->findNode(Target);
					assert(source != NULL);
					assert(target != NULL);
					set<pnapi::Arc *> toDelete;
					pnapi::Arc *curArc;
					FOREACH(a, source->getPresetArcs()) {
						curArc = this->mNet->findArc((*a)->getSourceNode(), *target);
						if (curArc == NULL) {
							this->createArc((*a)->getSourceNode().getName(), Target, (*a)->getWeight());
						}
						toDelete.insert((*a));
					}
					FOREACH(a, source->getPostsetArcs()) {
						curArc = this->mNet->findArc(*target, (*a)->getTargetNode());
						if (curArc == NULL) {
							this->createArc(Target, (*a)->getTargetNode().getName(), (*a)->getWeight());
						}
						toDelete.insert((*a));
					}
					FOREACH(a, toDelete) {
						this->mNet->deleteArc(**a);
					}
					if (SumMarkings) {this->mPlaceName2PlacePointer[Target]->setTokenCount(this->mPlaceName2PlacePointer[Target]->getTokenCount() + this->mPlaceName2PlacePointer[Source]->getTokenCount());}
					this->deletePlace(Source);
				}
				void createPetrinetByFragID(const frag_id_t);
				//delete
    			inline void deletePlace(const place_t & Place) {
					pnapi::Place * place = this->mNet->findPlace(Place);
					assert(place != NULL);			
					//MinFragID
					minFragID2Places_t::iterator curMinFragPlace;
					pair<minFragID2Places_t::iterator, minFragID2Places_t::iterator> curFragPlaces = this->mMinFragID2Places.equal_range(this->getPlaceMinFragID(Place));
					for (curMinFragPlace=curFragPlaces.first; curMinFragPlace!=curFragPlaces.second; ++curMinFragPlace) {	
						if (curMinFragPlace->second == Place) {
							break;
						}
					}
					assert(curMinFragPlace->second == Place);
					this->mMinFragID2Places.erase(curMinFragPlace);	
					this->mPlace2MinFragID.erase(Place);
					//handle multimaps
					//FragID
					size_t totalElems = this->mPlace2FragIDs.count(Place);
					size_t deleteElems = 0;
					fragID2Places_t::iterator curFragPlace;
					bool found = true;
					while (found) {
						found = false;
						for (curFragPlace=this->mFragID2Places.begin(); curFragPlace!=this->mFragID2Places.end(); ++curFragPlace) {
							if (curFragPlace->second == Place) {
								deleteElems++;
								found = true;
								this->deletePlaceFragID(curFragPlace->second, curFragPlace->first);
								break;
							}
						}
					}
					assert(deleteElems == totalElems);
					//RoleID
					totalElems = this->mPlace2RoleIDs.count(Place);
					deleteElems = 0;
					roleID2Places_t::iterator curRolePlace;
					found = true;
					while (found) {
						found = false;
						for (curRolePlace=this->mRoleID2Places.begin(); curRolePlace!=this->mRoleID2Places.end(); ++curRolePlace) {
							if (curRolePlace->second == Place) {
								this->mRoleID2Places.erase(curRolePlace);
								deleteElems++;
								found = true;
								break;
							}
						}
					}
					assert(deleteElems == totalElems);
					this->mPlace2RoleIDs.erase(Place);

					this->mPlaceName2PlacePointer.erase(Place);
					this->mPlacePointer2PlaceName.erase(place);
					this->mPlaces.erase(Place);	

					this->mNet->deletePlace(*this->mNet->findPlace(Place));
					this->mPlacesDelete++;
				}
				//get
					inline places_t getPlaces(pnapi::PetriNet & Petrinet) const {
						places_t ret;
						PNAPI_FOREACH(p, Petrinet.getPlaces()) {
							ret.insert((*p)->getName());
						}
						return ret;
					}
					inline transitions_t getPlaceTransitions(pnapi::PetriNet & Petrinet, const place_t & Place) const {
						return setUnion(this->getPlacePreset(Petrinet, Place), this->getPlacePostset(Petrinet, Place));
					}
					inline transitions_t getPlacePreset(pnapi::PetriNet & Petrinet, const place_t & Place) const {
						transitions_t ret;
						PNAPI_FOREACH(t, Petrinet.findPlace(Place)->getPreset()) {
							ret.insert((*t)->getName());
						}
						return ret;
					}
					inline transitions_t getPlacePostset(pnapi::PetriNet & Petrinet, const place_t & Place) const {
						transitions_t ret;
						PNAPI_FOREACH(t, Petrinet.findPlace(Place)->getPostset()) {
							ret.insert((*t)->getName());
						}
						return ret;
					}
					inline transitions_t getTransitions(pnapi::PetriNet & Petrinet) const {
						transitions_t ret;
						PNAPI_FOREACH(t, Petrinet.getTransitions()) {
							ret.insert((*t)->getName());
						}
						return ret;
					}
					inline places_t getTransitionPlaces(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
						return setUnion(this->getTransitionPreset(Petrinet, Transition), this->getTransitionPostset(Petrinet, Transition));
					}
					inline places_t getTransitionPreset(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
						places_t ret;
						PNAPI_FOREACH(p, Petrinet.findTransition(Transition)->getPreset()) {
							ret.insert((*p)->getName());
						}
						return ret;
					}
					inline places_t getTransitionPostset(pnapi::PetriNet & Petrinet, const transition_t & Transition) const {
						places_t ret;
						PNAPI_FOREACH(p, Petrinet.findTransition(Transition)->getPostset()) {
							ret.insert((*p)->getName());
						}
						return ret;
					}
			//diane
				bool processUnassignedFragment(const frag_id_t);
				void readDianeOutputFile(const string &);
				bool processDianeFragmentation();
				role_id_t getBestConnectionForDianeFragment(const diane_id_t, const bool);
				validStatus_e getDianeAssignementValidStatus(const diane_id_t, const role_id_t);
				void setDianeFragmentConnection(const diane_id_t, role_id_t);				
			//properties
				inline bool hasUnassignedTransitions(const place_t & Place) {
					transitions_t placeTransitions = this->getPlaceTransitions(*this->mNet, Place);	
					assert(placeTransitions.size() != 0);
					PNAPI_FOREACH(t, placeTransitions) {
						if (this->getTransitionRoleID(*t) == this->ROLE_UNASSIGNED) {
							return true;
						}
					}
					return false;
				}
				placeStatus_e getPlaceStatus(const place_t &, const bool, const role_id_t, const role_id_t);
			//helper
				inline void addTransitionPredecessors(stack<transition_t> & ToDo, transitions_t & Done, const transition_t & Transition) {
					transitions_t transitions;
					places_t places = this->getTransitionPreset(*this->mNet, Transition);
					FOREACH(p, places) {
						if (*p == this->mGlobalStartPlace) {
							if (Done.find(this->mGlobalStartPlace) == Done.end()) {
								ToDo.push(this->mGlobalStartPlace);
								Done.insert(this->mGlobalStartPlace);
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
				inline transitions_t getTransitionNearestPredecessorsSCC(const transition_t & Start, Tarjan & tarjan, map<transition_t, unsigned int> & TransitionBound) {				
					map<transition_t, unsigned int>::const_iterator curTransitionBound;
					map<transition_t, unsigned int>::const_iterator curTransitionBound2;
					curTransitionBound = TransitionBound.find(Start);
					assert(curTransitionBound != TransitionBound.end());					
					int curSCC = tarjan.getNodeSCC(Start);
					role_id_t startRoleID = this->getTransitionRoleID(Start);
					stack<transition_t> toDo;
					transitions_t done;
					transitions_t ret;

					toDo.push(Start);
					while (!toDo.empty()) {
						places_t places = this->getTransitionPreset(*this->mNet, toDo.top());
						done.insert(toDo.top());
						toDo.pop();
						FOREACH(p, places) {
							transitions_t transitions = this->getPlacePreset(*this->mNet, *p);
							FOREACH(t, transitions) {
								if (!((this->mBilateralTransitions.find(*t) != this->mBilateralTransitions.end()) && (this->getTransitionRoleID(*t) == startRoleID))) {
									if ((this->getTransitionRoleID(*t) == startRoleID) && (tarjan.getNodeSCC(*t) == curSCC)) {
										curTransitionBound2 = TransitionBound.find(*t);
										assert(curTransitionBound2 != TransitionBound.end());
										if ((curTransitionBound->second == 1) || (curTransitionBound->second == curTransitionBound2->second)) {ret.insert(*t); status("............%s", (*t).c_str());}
									}
									else {if (done.find(*t) == done.end()) {toDo.push(*t);}}
								}
							}
						}
					}
					return ret;
				}
				inline places_t getPossibleSplitPlacesCyclic(const transition_t & Start, const transition_t & End, Tarjan & tarjan) {		
					int curSCC = tarjan.getNodeSCC(Start);
					assert(tarjan.getNodeSCC(End) == curSCC);
					assert(this->getTransitionRoleID(End) == this->getTransitionRoleID(Start));
					stack<transition_t> toDo;
					transitions_t done;
					places_t ret;

					toDo.push(Start);
					while (!toDo.empty()) {
						places_t places = this->getTransitionPostset(*this->mNet, toDo.top());
						done.insert(toDo.top());
						toDo.pop();
						FOREACH(p, places) {
							if (tarjan.getNodeSCC(*p) == curSCC) {
								if (this->getPlacePreset(*this->mNet, *p).size() > 1) {ret.insert(*p);}
								transitions_t transitions = this->getPlacePostset(*this->mNet, *p);
								FOREACH(t, transitions) {
									if ((*t != End) && (done.find(*t) == done.end()) && (tarjan.getNodeSCC(*t) == curSCC)) {toDo.push(*t);}
								}
							}
						}
					}
					return ret;
				}
				inline places_t getPossibleSplitPlacesNonCyclic(const transition_t & Start, Tarjan & tarjan) {		
					assert(!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(Start)));
					stack<transition_t> toDo;
					transitions_t done;
					places_t ret;

					toDo.push(Start);
					while (!toDo.empty()) {
						places_t places = this->getTransitionPreset(*this->mNet, toDo.top());
						done.insert(toDo.top());
						toDo.pop();
						FOREACH(p, places) {
							if (this->getPlacePostset(*this->mNet, *p).size() > 1) {ret.insert(*p);}
							transitions_t transitions = this->getPlacePreset(*this->mNet, *p);
							FOREACH(t, transitions) {
								if (!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(*t))) {toDo.push(*t);}
							}
						}
					}
					return ret;
				}
				inline places_t getTransitionPredeccessorsNonCyclic(const transition_t & Start, Tarjan & tarjan, map<transition_t, unsigned int> & TransitionBound) {
					assert(!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(Start)));
					role_id_t startRoleID = this->getTransitionRoleID(Start);
					stack<transition_t> toDo;
					transitions_t done;
					transitions_t ret;
					map<transition_t, unsigned int>::const_iterator curTransitionBound;

					toDo.push(Start);
					while (!toDo.empty()) {
						places_t places = this->getTransitionPreset(*this->mNet, toDo.top());
						done.insert(toDo.top());
						toDo.pop();
						FOREACH(p, places) {
							transitions_t transitions = this->getPlacePreset(*this->mNet, *p);
							FOREACH(t, transitions) {
								if (!tarjan.isNonTrivialSCC(tarjan.getNodeSCC(*t))) {
									if (this->getTransitionRoleID(*t) == startRoleID) {
										curTransitionBound = TransitionBound.find(*t);
										assert(curTransitionBound != TransitionBound.end());
										if (curTransitionBound->second == 1) {ret.insert(*t); status("..........%s", (*t).c_str());}
									}
									if (done.find(*t) == done.end()) {toDo.push(*t);}
								}
							}
						}
					}
					if (ret.size() == 0) {
						curTransitionBound = TransitionBound.find(Start);
						assert(curTransitionBound != TransitionBound.end());
						if (curTransitionBound->second == 1) {ret.insert(Start); status("..........%s", Start.c_str());} 
					}
					return ret;
				}
				inline void replaceFragIDs(const frag_id_t FragOld, const frag_id_t FragNew) {
					assert(FragOld != FragNew);
					set< pair<place_t, frag_id_t> > toAdd;
					set< pair<place_t, frag_id_t> > toDelete;
					pair<fragID2Places_t::iterator, fragID2Places_t::iterator> curFragPlaces = this->mFragID2Places.equal_range(FragOld);
					for (fragID2Places_t::iterator curFragPlace=curFragPlaces.first; curFragPlace!=curFragPlaces.second; ++curFragPlace) {
						toAdd.insert( std::make_pair(curFragPlace->second, FragNew) );
						toDelete.insert( std::make_pair(curFragPlace->second, FragOld) );
					}
					FOREACH(p, toAdd) {
						this->addPlaceFragID(p->first, p->second);
					}
					FOREACH(p, toDelete) {
						this->deletePlaceFragID(p->first, p->second);
						transitions_t placeTransitions = this->getPlaceTransitions(*this->mNet, p->first);		
						PNAPI_FOREACH(t, placeTransitions) {
							if (this->getTransitionFragID(*t) == FragOld) {
								this->setTransitionFragID(*t, FragNew);
							}
						}
					}
				}
				inline bool connectFragments() {
					bool ret = false;
					set<frag_id_t> toConnect;
					places_t netPlaces = this->getPlaces(*this->mNet);
					FOREACH (p, netPlaces) {
						pair<place2FragIDs_t::iterator, place2FragIDs_t::iterator> curPlaceFragIDs = this->mPlace2FragIDs.equal_range(*p);
						bool isSharedPlace = this->isSharedPlace(*p);
						for (place2FragIDs_t::iterator curPlaceFragID=curPlaceFragIDs.first; curPlaceFragID!=curPlaceFragIDs.second; ++curPlaceFragID) {
							role_id_t fragmentRoleID = this->getFragmentRoleID(curPlaceFragID->second);
							if ((!isSharedPlace) || (fragmentRoleID == this->ROLE_UNASSIGNED)) {
								toConnect.insert(curPlaceFragID->second);
							}
						}
						if (toConnect.size() > 1) {
							ret = true;
							frag_id_t minFragID = *toConnect.begin();
							toConnect.erase(minFragID);
							FOREACH(r, toConnect) {
								//status("....replace %d with %d", *r, minFragID);
								this->replaceFragIDs(*r, minFragID);
							}
						}
						toConnect.clear();
					}
					return ret;
				}
				inline role_t concatenateRoles(const set<role_t> & Roles) {
					size_t size = Roles.size();					
					assert(size != 0);
					if (size == 1) {return *Roles.begin();}
					role_t ret = "";
					size_t curRole = 0;
					FOREACH(r, Roles) {
						curRole++;
						if (curRole != size) {
							ret += *r + ", ";
						}
						else {
							ret += *r;
						}
					}
					return ret;
				}

	public:
		//methods
			//global
				Fragmentation(pnapi::PetriNet &);
				inline ~Fragmentation() {};
			//member support
				//set/add
				//get
					inline bool getLolaCalled() {return this->mLolaCalled;}
					inline size_t getDianeCalls() {return this->mDianeCalls;}
					inline size_t getDianeForces() {return this->mOverallDianeForces;}
					inline size_t getDianeAlternatives() {return this->mOverallDianeAlternatives;}
					inline bool hasCycles() {return this->mHasCycles;}
					inline bool isFreeChoice() {return this->mIsFreeChoice;}
					inline size_t getInterfaceCorrections() {return this->mInterfaceCorrections;}
					inline size_t getBoundnessCorrections() {return this->mBoundnessCorrections;}
					inline size_t getBoundnessOrConnections() {return this->mBoundnessOrConnections;}
					inline size_t getBoundnessAndConnections() {return this->mBoundnessAndConnections;}
					inline size_t getMergings() {return this->mMergings;}
					inline size_t getFragmentConnections() {return this->mFragmentConnections;}
					inline size_t getArcweightCorrections() {return this->mArcweightCorrections;}
					inline size_t getInitialMarkings() {return this->mInitialMarkings;}
					inline size_t getSelfreactivatings() {return this->mSelfreactivatings;}
					inline size_t getPlacesInsert() {return this->mPlacesInsert;}
					inline size_t getTransitionsInsert() {return this->mTransitionsInsert;}
					inline size_t getArcsInsert() {return this->mArcsInsert;}
					inline size_t getPlacesDelete() {return this->mPlacesDelete;}
					inline size_t getTransitionsDelete() {return this->mTransitionsDelete;}
					inline size_t getArcsDelete() {return this->mArcsDelete;}
					inline size_t getRolesAnnotated() {return this->mRolesAnnotated;}
					inline size_t getNotAnnotatedTransitions() {return this->mNotAnnotatedTransitions;}
					inline bool getConcatenateAnnotationNecessary() {return this->mConcatenateAnnotationNecessary;}
					inline role_id_t getTransitionRoleID(const transition_t & Transition) {
						transition2RoleID_t::iterator curRole = this->mTransition2RoleID.find(Transition);
						assert(curRole != this->mTransition2RoleID.end());
						return curRole->second;
					}
					inline frag_id_t getTransitionFragID(const transition_t & Transition) {
						transition2FragID_t::iterator curFrag = this->mTransition2FragID.find(Transition);
						assert(curFrag != this->mTransition2FragID.end());
						return curFrag->second;
					}
					inline role_id_t getFragmentRoleID(const frag_id_t FragID) {
						fragID2Transitions_t::const_iterator curFragTransition = this->mFragID2Transitions.find(FragID);
						assert(curFragTransition != this->mFragID2Transitions.end());
						transition2RoleID_t::const_iterator curTransitionRole = this->mTransition2RoleID.find(curFragTransition->second);
						assert(curTransitionRole != this->mTransition2RoleID.end());
						return curTransitionRole->second;
					}
					inline role_id_t getRoleID(const string & Role) {
						roleName2RoleID_t::iterator curRole = this->mRoleName2RoleID.find(Role);
						assert(curRole != this->mRoleName2RoleID.end());
						return curRole->second;
					}
					inline string getColorName(const size_t ColorID) {
						colorID2ColorName_t::const_iterator curColor = this->mColorID2ColorName.find(ColorID);
						assert(curColor != this->mColorID2ColorName.end());
						return curColor->second;
					}
					inline roleName2RoleID_t getRoles() const {
						return this->mRoleName2RoleID;
					}
				//delete		
					inline void eraseColors() {
						PNAPI_FOREACH(n, this->mNet->getNodes()) {
							(*n)->setColor();
						}
					}
			//petrinet
				pnapi::PetriNet createPetrinetByRoleID(const role_id_t);
			//color
				void colorFragmentsByFragID(const bool = false);
				void colorFragmentsByRoleID(const bool = false);
			//properties
				inline bool isSharedPlace(const place_t & Place) {return (this->mPlace2RoleIDs.count(Place) > 1);}
				inline bool isFragmentEmpty(const frag_id_t FragID) {return (this->mFragID2Transitions.find(FragID) == this->mFragID2Transitions.end());}
				inline bool isRoleEmpty(const role_id_t RoleID) {return (this->mRoleID2Transitions.find(RoleID) == this->mRoleID2Transitions.end());}
				inline bool isStartTransition(const transition_t & Transition) {
					assert(this->mRoleFragmentsBuild);
					role_id_t transitionRoleID = this->getTransitionRoleID(Transition);
					places_t transitionPlaces = this->getTransitionPreset(*this->mNet, Transition);
					FOREACH(p, transitionPlaces) {
						if (this->mPlace2RoleIDs.count(*p) == 1) {
							if (this->mPlace2RoleIDs.find(*p)->second == transitionRoleID) {
								return false;
							}
						}
					}
					return true;
				}
				validStatus_e isFragmentationValid(const bool = false);
			//main
				bool buildRoleFragments();
				bool processUnassignedFragments();
				bool buildServices();
	
};

#endif //__FRAGMENTATION_H
