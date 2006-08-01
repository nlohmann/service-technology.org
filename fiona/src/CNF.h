/*!
 * 	\class clause
    \brief class for storing a clause (set of literals)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */

#ifndef CNF_H_
#define CNF_H_


#include "mynew.h"
#include "enums.h"
#include <string>
#include <set>		// set

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
	
	bool fake;
	
	graphEdge * edge;
	clause * nextElement; 

	void addLiteral(char *);
	
	void setEdge(graphEdge *);
	void setEdges(graphEdge *);
	
	string getClauseString();
	
	vertexColor getColor();

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(clause)
#define new NEW_NEW
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
	bool isFinalState;			// indicates whether this clause belongs to a final state or not
	
	vertexColor getColor();
	vertexColor calcClauseColor();
	
	void addClause(clause *);
	void setEdge(graphEdge *);

	int numberOfElements();
	
	string getCNFString();

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(CNF)
#define new NEW_NEW
};

#endif /* CNF_H_ */
