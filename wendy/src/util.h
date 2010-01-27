/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _UTIL_H
#define _UTIL_H

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
        passed container C. There is no need to car about constness.
*/
#define FOREACH(I,C) for(__typeof((C).begin()) I=((C).begin()); I != (C).end(); ++I)


#endif
