/*
 * boiunds.cc
 *
 *  Created on: 21.04.2009
 *      Author: Jan
 */

#include "bounds.h"
#include "helpers.h"

BOUND getBoundFromLPSolveString(std::string s) {
	std::string::size_type pos;

	pos = s.find(":");
	if (pos != std::string::npos) {
		s = s.substr(pos+2);
		pos = s.find(".");
		if (pos != std::string::npos) {
			s = s.substr(0,pos);
//			std::cerr << "val is: " << s << "\n";
//			if (s.compare("1000000000000000000000000000000") == 0) {
//				return UNBOUNDED;
//			}
		}
		return atoi(s.c_str());

	}

	return UNBOUNDED;

}
