#ifndef ENUMS_H_
#define ENUMS_H_

#ifdef LOG_NEW
#include "mynew.h"
#endif

enum vertexColor {BLACK, RED, BLUE};		//!< BLACK == vertex has not been checked; RED == bad vertex; BLUE == good one
enum edgeType {sending, receiving}; 		//!< ENUM possible types of an edge
enum analysisResult {TERMINATE, CONTINUE};  //!< needed as feedback of the "analysis" function, whether this node is an end node or not


#endif /*ENUMS_H_*/
