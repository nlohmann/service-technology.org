#include <fstream>
#include <cstdio>
#include <iostream>

using std::ostream;

class graphreport {
public: 
	virtual void tell() = 0; //print portion of state space to graph report file

};


class graphreport_no : public graphreport {
public:
graphreport_no() {}
virtual void tell(){}
};

class graphreport_yes : public graphreport{
public:
ostream* graphstream;
graphreport_yes(char *); // named file
graphreport_yes(); // stdout
virtual void tell();
};

extern graphreport * TheGraphReport;
