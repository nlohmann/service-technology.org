#include "mynew.h"
#include<iostream>
#include"dimensions.h"

using namespace std;

void reportconfiguration()
{
	cout << endl;
	cout << "Tools4oWFN" << endl;
	cout << "---------------------------" << endl << endl;
	cout << "written by Daniela Weinberg, Peter Massuthe" << endl;
	cout << endl;
	cout << "what it does!!!" << endl;
	cout << "parameters:" << endl << endl;
	cout << "  -t OG" << " calculate the operating guideline of the given net" << endl;
	cout << "  -t IG" << " calculate the interaction graph of the given net (default, if -t is not set)" << endl;
	cout << "  -c d" << " set communication depth to d" << endl;
	cout << "  -a" << " all states of the nodes of the graphs (OG, IG) are stored" << endl;
	cout << "  -r" << " calculates the interaction graph using appropriate reduction techniques (under construction)" << endl;
	cout << "  -d x" << " set debug level to x; from 1 to 5 (most details)" << endl;
	cout << endl;
	cout << "NO WARRANTY!" << endl;
	cout << "http://www.informatik.hu-berlin.de/top/" << endl << endl;
} 


