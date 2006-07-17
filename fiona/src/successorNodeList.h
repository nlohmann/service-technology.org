/*!
 * 	\class successorNodeList
    \brief list of successor/ predecessor nodes

	\author Daniela Weinberg
	\par History
		- 2005-09-13 creation
		- 2005-12-03 added comments
 */
 
#ifndef SUCCESSORNODELIST_H
#define SUCCESSORNODELIST_H

#include "mynew.h"
#include "graphEdge.h"
#include "enums.h"
#include <stddef.h>
#include <string>

using namespace std;

class vertex;

class successorNodeList {

private:    
    graphEdge * firstElement;		//!< pointer to the first element of the list
    graphEdge * nextElement;		//!< pointer to the next element in the list while iterating through the list
    
    bool end;			/*!< we have reached the end of the iterating process, or not */
    
public:

    successorNodeList();    
    ~successorNodeList(); 
    
    void deleteList(graphEdge *);   
//    bool addNextNode(vertex *, char *, edgeType);
    bool addNextNode(graphEdge *);
    int elementCount();
    
    void setFirstElement(graphEdge *);
    graphEdge * getFirstElement();
    graphEdge * findElement(vertex * node);
    
    void resetIterating();
    graphEdge * getNextElement();
    
    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(successorNodeList)
#define new NEW_NEW
};
#endif //SUCCESSORNODELIST_H
