#ifndef __TYPES_H
#define __TYPES_H

#include <limits>
#include <stdint.h>
#include <string>

using std::set;
using std::string;

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
