#include <iostream>
#include <map>
#include <string>
#include <set>
#include <list>

using namespace std;


extern int net_parse();
extern int net_lineno;
extern char *net_text;
extern FILE *net_in;

extern int path_parse();
extern int path_lineno;
extern char *path_text;
extern FILE *path_in;


extern list<string> places;
extern map<string, int> marking;
extern map <string, map<string, int> > transitions;
extern list<string> sequence;


int net_error(const char *msg)
{
  cerr << "NET " << msg << " in line " << net_lineno << endl;
  cerr << "last read: " << net_text << endl;
  exit(1);
}


int path_error(const char *msg)
{
  cerr << "PATH " << msg << " in line " << path_lineno << endl;
  cerr << "last read: " << path_text << endl;
  exit(1);
}


void printMarking(unsigned int state)
{
  cout << "STATE " << state << endl;

  bool firstPlace = true;
  for (list<string>::iterator place = places.begin(); place != places.end(); place++)
  {
    if (marking[*place] != 0)
    {
      if (firstPlace)
      {
	cout << *place << " : " << marking[*place];
	firstPlace = false;
      }
      else
	cout << "," << endl << *place << " : " << marking[*place];
    }
  }
  cout << endl;
}


void calculateMarking()
{
  unsigned int currentState = 0;
  printMarking(currentState++);
  
  for (list<string>::iterator transition = sequence.begin(); transition != sequence.end(); transition++)
  {
    cout << *transition << " -> " << currentState << endl << endl;
    for (list<string>::iterator place = places.begin(); place != places.end(); place++)
      marking[*place] += transitions[*transition][*place];
    printMarking(currentState++);
  }
}


int main(int argc, char *argv[])
{
  char* filename = NULL;
  
  filename = argv[1];
  net_in = fopen(filename, "r");    
  net_parse();
  fclose(net_in);

  cerr << places.size() << " places" << endl;
  cerr << transitions.size() << " transitions" << endl;

  filename = argv[2];
  path_in = fopen(filename, "r");
  path_parse();
  fclose(path_in);

  cerr << sequence.size() << " transitions in path" << endl;

  calculateMarking();


  return 1;
}
