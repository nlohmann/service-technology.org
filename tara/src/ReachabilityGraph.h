#include <map>
#include <vector>
#include <set>

class ReachabilityGraph {
  public:
  int root;
  std::map<std::pair<int,int>, int> delta;
  std::vector<char*> transitions;
  
  int insertTransition(char* t);
  
  std::set<int> enabledTransitions(int s);
  bool enables(int s, int t);
  int yields(int s, int t);  
  
  void print();
  void print_r(int s, std::set<int> visited);

};
