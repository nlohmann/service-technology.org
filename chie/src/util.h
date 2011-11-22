/*
 * utility functions and macros here
 */

#ifndef UTILS_H_
#define UTILS_H_

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


#endif /* UTILS_H_ */
