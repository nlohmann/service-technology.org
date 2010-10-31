#ifndef __TYPES_H
#define __TYPES_H

#include <limits>
#include <stdint.h>
#include <string>

using std::set;
using std::map;
using std::multimap;

using std::string;

enum placeStatus_e {
	PLACE_STATUS_UNASSIGNED,
	PLACE_STATUS_START,
	PLACE_STATUS_END,
	PLACE_STATUS_INTERN,
	PLACE_STATUS_SHARED,
	PLACE_STATUS_NOTDISJUNCT,
	PLACE_STATUS_NOTBILATERAL,
	PLACE_STATUS_BAD
};

enum validStatus_e {
	VALID_OK,
	VALID_TODO,
	VALID_BAD
};

typedef uint32_t frag_id_t;
typedef uint32_t role_id_t;
typedef uint32_t diane_id_t;

typedef string transition_t;
typedef string place_t;
typedef string node_t;

typedef set<role_id_t> roles_t;
typedef set<place_t> places_t;
typedef set<transition_t> transitions_t;

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

#endif //__TYPES_H
