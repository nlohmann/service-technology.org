#ifndef MULTISET_H
#define MULTISET_H

#include <map>
#include <set>

template <class T>
class Multiset {
  std::set<T> mCarrier;
  unsigned int mSize;
  std::map<T, unsigned int> mMap;
  public:
  unsigned int insert(T& element);
  void removeOnce(T& element);
  void removeAll(T& element);
  unsigned int size() const;
  unsigned int occ(T& element) const;
  Multiset();
  std::set<T>& getCarrier();
  bool operator== (const Multiset<T>& other) const;
  bool operator< (const Multiset<T>& other) const;
};

template <class T>
Multiset<T>::Multiset() : mSize(0) {}

template <class T>
unsigned int Multiset<T>::insert(T& element) {
  
  if (mMap.find(element) != mMap.end()) {
    ++mMap[element]; 
  } else {
    mMap[element] = 1;
    mCarrier.insert(element);
  }
  ++mSize;
  return mMap[element];
}

template <class T>
void Multiset<T>::removeOnce(T& element) {

  if (mMap.find(element) != mMap.end()) {
    if (mMap[element] > 1) {
      --mMap[element];
    } else {
      mMap.erase(element);
      mCarrier.erase(element);
    }
    --mSize;
  } 
  
}

template <class T>
void Multiset<T>::removeAll(T& element) {
  mSize = mSize - occ(element);
  mMap.erase(element);
  mCarrier.erase(element);   
}

template <class T>
unsigned int Multiset<T>::size() const {
  return mSize;
}

template <class T>
unsigned int Multiset<T>::occ(T& element) const {
  if (mMap.find(element) == mMap.end()) {
    return 0;
  } 
  return mMap.find(element)->second;
}

template <class T>
std::set<T>& Multiset<T>::getCarrier() {
  return mCarrier;
}

template <class T>
bool Multiset<T>::operator == (const Multiset<T> & other) const {
  if (mSize != other.size()) {
    return false;
  }
  if (mCarrier != other.mCarrier) {
    return false;
  }
  return mMap == other.mMap;
}

template <class T>
bool Multiset<T>::operator< (const Multiset<T> & other) const {
  return mMap < other.mMap;
}

#endif
