#include "helpers.h"

const std::string intToStr(const int x) {
	std::ostringstream o;
	if (!(o << x)) return "ERROR";
	return o.str();
}

std::string getText(FILE* solution) {

	char psBuffer[16];
	std::string iresult = "";

	while( !feof( solution ) )
	{
		if( fgets( psBuffer, 16, solution ) != NULL )
			iresult += psBuffer;
	}

	if ( iresult.find("infeasible") != std::string::npos ) {
		std::cerr << "Not WT-controllable" << std::endl;
		exit(EXIT_FAILURE);
	}


	return iresult;
}

void cleanstring(std::string& result) {

	std::string::size_type pos = 0;
	while ( (pos = result.find("Value of objective function: ")) != std::string::npos ) {
		result.erase( pos, 29 );
	}

	while ( (pos = result.find("This problem is ")) != std::string::npos ) {
		result.erase( pos, 16 );
	}

	while ( (pos = result.find(".00000000")) != std::string::npos ) {
		result.erase( pos, 9 );
	}

	while ( (pos = result.find("\n")) != std::string::npos ) {
		result.erase( pos, 1 );
	}

	while ( (pos = result.find("\r")) != std::string::npos ) {
		result.erase( pos, 1 );
	}

	while ( (pos = result.find((char) 1)) != std::string::npos ) {
		result.replace( pos, 1,"\n" );
	}

}
