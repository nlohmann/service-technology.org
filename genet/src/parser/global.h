#ifndef GLOBAL_h
#define GLOBAL_h

#include <list>
#include <string>

using namespace std;

class Ttrans {
public:
 string source;
 string target;
 string event;

 Ttrans() {
 }


 Ttrans(string s, string e, string t) {
  source = s;
  target = t;
  event = e;
 }

 Ttrans(char *s, char *e, char *t) {
  source = s;
  target = t;
  event = e;
 }
};


class Tts {
public:
  list<Ttrans> ltrans;
  string initial;

  Tts() {
    ltrans = list<Ttrans>(0);
  }
  
  Tts(const Tts &t) {
    ltrans = t.ltrans;
    initial = t.initial;
  }
  
  ~Tts() {
  	ltrans.clear();
  } 
  
  void insert(char const *s1,char const *s2,char const *s3) {
  	//ltrans.push_back(Ttrans(string(s1),string(s2),string(s3)));
  	ltrans.push_back(Ttrans(s1,s2,s3));
  }
  
  Tts &operator=(const Tts &t) {
    ltrans = t.ltrans;
    initial = t.initial;
    return *this;
  }

};


#endif
