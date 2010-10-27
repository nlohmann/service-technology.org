#ifndef __TYPES_H
#define __TYPES_H

#include <limits>
#include <stdint.h>
#include <string>

using std::set;
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

//ToDo: remove
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

#endif //__TYPES_H
