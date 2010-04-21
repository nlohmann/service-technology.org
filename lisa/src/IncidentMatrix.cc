#include <vector>
#include <math.h>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "IncidentMatrix.h"

/***************
 * CONSTRUCTOR *
 ***************/

/*! 

Constructor - initializes matrix and sets underlying petri net

*/

IncidentMatrix::IncidentMatrix(PetriNet& net) : _net(net) {
	_columnsCount = 3;
	_rowsCount = 3;	
	//int k = 0;
	vector<int> innerVector(_columnsCount);
	for( int i = 0; i < _rowsCount; ++i )
	  _matrix.push_back(innerVector);
	/*	
	for(int i=0;i<_columnsCount;i++){
	  for(int j=0;j<_rowsCount;j++)
	    _matrix[i][j] = ++k;
	}
	*/
}

void IncidentMatrix::printMatrix() {
	for(int i=0;i<_columnsCount;i++){
	  std::cout << std::endl;
	  for(int j=0;j<_rowsCount;j++)
	    std::cout << _matrix[i][j] << " ";
	}
	std::cout << std::endl;
}


