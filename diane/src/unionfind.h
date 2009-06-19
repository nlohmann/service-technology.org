/*
 * unionfind.h
 *
 *  Created on: Jun 9, 2009
 *      Author: stephan
 */

#ifndef UNIONFIND_H
#define UNIONFIND_H

/// makes a singleton set of the given element
void MakeSet(int, int *);

/// makes a union of both given parameters
void Union(int, int, int *);

/// finds the parent of the given parameter
int Find(int, int *);

#endif /* UNIONFIND_H */
