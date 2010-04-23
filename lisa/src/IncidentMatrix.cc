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
	_rowsCount = net.getTransitions().size();
	_columnsCount = net.getPlaces().size();	

	//Initialize matrix
	vector<int> innerVector(_rowsCount);
	for( int i = 0; i < _columnsCount; ++i )
	  _matrix.push_back(innerVector);

	//Calculate incident matrix
	calculateMatrix();

	/*
	int k = 0;	
	for(int i=0;i<_columnsCount;i++){
	  for(int j=0;j<_rowsCount;j++)
	    _matrix[i][j] = ++k;
	}
	*/
	
}

/*! 

Calculates the incident matrix for the underlying net

*/

void IncidentMatrix::calculateMatrix() {
	set<Transition*>::const_iterator it1;	
	for(it1 = _net.getTransitions().begin(); it1 != _net.getTransitions().end(); ++it1)
	  _transitions.push_back(*it1);	
	set<Place*>::const_iterator it2;	
	for(it2 = _net.getPlaces().begin(); it2!= _net.getPlaces().end(); ++it2)
	  _places.push_back(*it2);

	//Calculate D+ matrix
	for(int i=0;i<_rowsCount;i++){
	  for(int j=0;j<_columnsCount;j++){
	    if(_net.findArc(*_transitions[i], *_places[j]) != NULL)
	      _matrix[i][j] = 1;
	    else
 	      _matrix[i][j] = 0;	    
	  }
	}	
	
	//Subtract D- matrix from D+ matrix
	for(int i=0;i<_rowsCount;i++){
	  for(int j=0;j<_columnsCount;j++){
	    if(_net.findArc(*_places[j], *_transitions[i]) != NULL)
	      _matrix[i][j] += -1;
	    else
 	      _matrix[i][j] += 0;	    
	  }
	}
	
}

void IncidentMatrix::printMatrix() {
	for(int i=0;i<_rowsCount;i++){
	  std::cerr << std::endl;
	  for(int j=0;j<_columnsCount;j++)
	    std::cerr << _matrix[i][j] << " ";
	}
	std::cerr << std::endl;
}


