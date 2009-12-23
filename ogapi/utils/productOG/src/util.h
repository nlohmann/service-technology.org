#ifndef UTIL_H_
#define UTIL_H_

#include <list>
#include <map>
#include <set>
#include <vector>

namespace util
{

// calculate the set intersection of a vector of sets
template <typename T>
std::set<T> intersection(std::vector<std::set<T> > sets)
{
  // typedefs
  typedef typename std::set<T>::iterator setIt;
  typedef typename std::list<setIt*>::iterator listIt;
  
  std::set<T> result; // resulting set
  unsigned int size = sets.size(); // amount of given sets
  std::list<setIt*> iterators; // sorted list of set iterators
  // mapping of iterators to appropriate end iterators
  std::map<setIt*, setIt> end_iterators;  
  
  // initialize iterators
  for(unsigned int i = 0; i < size; ++i)
  {
    // create iterator pointer and map to end iterator
    setIt * itP = new setIt(sets[i].begin());
    end_iterators[itP] = sets[i].end();
    // insert sorted
    for(listIt it = iterators.begin(); it != iterators.end(); ++it)
    {
      if((**itP) < (***it))
      {
        iterators.insert(it,itP);
        break;
      }
    }
    if(iterators.size() == i)
      iterators.push_back(itP);
  }
  
  /* The set intersection is calculated as follows:
   * If the first and the last iterator of the list hold equal elements,
   * all elements in iterators are equal, i.e. this element is in each set
   * and will be inserted to the resulting set.
   * 
   * However, the first iterator (i.e. the one holding the smallest element)
   * of the list will be removed from iterators, incremented and inserted sorted
   * in iterators again.
   * 
   * If an iterator becomes an end iterator after incrementing, there will be
   * no more element in the resulting set, thus cleanup will begin.
   */
  while(true)
  {
    // get the first iterator
    setIt * itP = iterators.front();
    // if all iterators hold equal elements
    if((**itP) == (**(iterators.back())))
    {
      // add element to resulting set
      result.insert(**itP);
    }
    // remove first iterator from iterators
    iterators.pop_front();
    
    // increment recent iterator
    ++(*itP);
    
    // if recent iterator is now an end iterator
    if((*itP) == end_iterators[itP])
    {
      // we're done
      delete itP;
      break;
    }
    
    // insert recent iterator again in the sorted list
    for(listIt it = iterators.begin(); it != iterators.end(); ++it)
    {
      if((**itP) < (***it))
      {
        iterators.insert(it,itP);
        break;
      }
    }
    if(iterators.size() < size)
      iterators.push_back(itP);
  }
  
  // cleanup
  while(!iterators.empty())
  {
    delete (iterators.front());
    iterators.pop_front();
  }
  
  return result;
}

} /* namespace util */

#endif /*UTIL_H_*/
