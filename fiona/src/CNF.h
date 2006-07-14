/*!
 * 	\class clause
    \brief class for storing a clause (set of literals)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */

#ifndef CNF_H_
#define CNF_H_


#include "enums.h"
#include <string>
#include <set>		// set

#ifdef LOG_NEW
#include "mynew.h"
#endif

using namespace std;

class graphEdge;
class State;


/*!
 * 	\class clause
    \brief class for storing a clause (set of literals)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */
class clause {
public:
	clause();
	clause(graphEdge *);
	~clause();	
	
	graphEdge * edge;
	clause * nextElement; 

	void addLiteral(char *);
	
	void setEdge(graphEdge *);
	void setEdges(graphEdge *);
	
	string getClauseString();
	
	vertexColor getColor();
};


/*!
 * 	\class CNF
    \brief class for storing a CNF (conjunction of clauses)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */
class CNF {
public:	
	CNF();
	~CNF();

	clause * cl;		
	CNF * nextElement;			// !< next element in CNF
	
	vertexColor getColor();
	vertexColor calcColor();
	
	void addClause(clause *);
	void setEdge(graphEdge *);
	
	string getCNF();
};

#endif /* CNF_H_ */
