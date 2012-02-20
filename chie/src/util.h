/*
 * utility functions and macros here
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <map>
#include <vector>

/*!
  \def FOREACH(I, C)
  \brief a shortcut for STL iterations
  \details Assume you have the code:
  \code
  for (std::set<std::string>::iterator it = mySet.begin(); it != mySet.end(); ++it) {
    // stuff
  }
  \endcode
  Then this code can be replaced by the following code
  \code
  FOREACH(it, mySet) {
    // stuff
  }
  \endcode
  \param I an iterator variable
  \param C an container to iterate
  \note The code for the FOREACH macro was taken from a post on Stack Overflow
        http://stackoverflow.com/questions/197375/visual-c-for-each-portability/197926#197926
  \note The type of the iterator is derived from the begin() function of the
        passed container C. There is no need to care about constness.
*/
#define FOREACH(I,C) for(__typeof((C).begin()) I=((C).begin()); I != (C).end(); ++I)


/*!
 * \brief generic hashmap class
 */
template<typename T, unsigned int (*hash)(T&)>
class HashMap {
    private: /* private variables */
        // actual map
        std::map<unsigned int, std::vector<T*> > map_;
        // iterator to "current" element
        __typeof(map_.begin()) it_;
        // index to "current" element
        unsigned int index_;

    public: /* public methods */
        // get the first element
        void resetToFirst();
        // get next element
        T* getNext();
        // insert function
        T* insert(T&);
        // clear map
        void clear();
        // clear map and delete objects
        void clearDelete();
        // removes an item if stored
        void remove(T&);
        // gets first
};


/*!
 * \brief get the first element
 */
template<typename T, unsigned int (*hash)(T&)>
void HashMap<T, hash>::resetToFirst() {
    it_ = map_.begin();
    index_ = 0;
}

/*!
 * \brief get next element
 */
template<typename T, unsigned int (*hash)(T&)>
T* HashMap<T, hash>::getNext() {
    if (it_ == map_.end()) {
        return NULL;
    }

    // get result
    T* result = (it_->second)[index_++];

    // set "current" element to next element
    if (index_ >= it_->second.size()) {
        ++it_;
        index_ = 0;
    }

    // return result
    return result;
}

/*!
 * \brief insert function
 * \return pointer to given object or pointer to already stored object equal to the given object
 */
template<typename T, unsigned int (*hash)(T&)>
T* HashMap<T, hash>::insert(T& obj) {
    // calculate hash
    unsigned int hash_ = hash(obj);

    // check for already inserted object
    for (unsigned int i = 0; i < map_[hash_].size(); ++i) {
        if ((*map_[hash_][i]) == obj) {
            // equal object already inserted; skip insertion
            return map_[hash_][i];
        }
    }

    // no equal object found; insert obj
    map_[hash_].push_back(&obj);

    // return result
    return &obj;
}

/*!
 * \brief clear map
 */
template<typename T, unsigned int (*hash)(T&)>
void HashMap<T, hash>::clear() {
    map_.clear();
}

/*!
 * \brief clear map and delete objects
 */
template<typename T, unsigned int (*hash)(T&)>
void HashMap<T, hash>::clearDelete() {
    FOREACH(entry, map_) {
        for (unsigned int i = 0; i < entry->second.size(); ++i) {
            delete((entry->second)[i]);
        }
    }

    map_.clear();
}

/*!
 * \brief removes an item if stored
 */
template<typename T, unsigned int (*hash)(T&)>
void HashMap<T, hash>::remove(T& obj) {
    // calculate hash
    unsigned int hash_ = hash(obj);

    // check for already inserted object
    for (unsigned int i = 0; i < map_[hash_].size(); ++i) {
        if (map_[hash_][i] == &obj) {
            // object inserted; remove it
            map_[hash_][i] = map_[hash_][map_[hash_].size() - 1];
            map_[hash_].pop_back();
            break;
        }
    }
}

#endif /* UTILS_H_ */
