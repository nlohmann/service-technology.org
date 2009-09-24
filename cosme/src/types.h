#ifndef __TYPES_H
#define __TYPES_H

#include <limits>
#include <stdint.h>

#define RBTS_NODE_KEY_UNUSED 4294967295U

// the range of labels (max. 255)
typedef uint8_t label_id_t;

// the range of labels leaving a node
typedef label_id_t label_index_t;
// the range of nodes in graphs (max. 255)
typedef uint32_t og_service_index_t;

#endif //__TYPES_H
