#include "helpers.h"


const std::string intToStr(const int x) {
	std::ostringstream o;
	if (!(o << x)) return "ERROR";
	return o.str();
}


