/*****************************************************************************\
 ProductOG -- Computing product OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 ProductOG is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 ProductOG is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with ProductOG.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

#ifndef FORMULA_H_
#define FORMULA_H_

#include <string>
#include <set>
#include <iostream>


// forward declartation
class Literal;

/*!
 * \brief abstract formula class
 */
class Formula
{
public:
  // clone the formula
  virtual Formula * clone() const = 0;
  // disjunctive normal form
  virtual Formula * dnf() const = 0;
  // whether formula is satisfied
  virtual bool sat(const std::set<std::string> &) const = 0;
  // simplifies the formula
  virtual Formula * simplify(const std::set<std::string> &) const = 0;
  // comparison operator
  virtual bool operator==(bool) const;
  // comparison operator
  virtual bool operator==(const Formula &) const = 0;
  // whether (NOT this) implies (NOT the other formula)
  virtual bool operator<=(const Formula &) const = 0;
  // create a conjunction
  Formula * operator&(const Formula &) const;
  // create a disjunction
  Formula * operator|(const Formula &) const;
  // write to stream
  virtual std::ostream & out(std::ostream &) = 0;
};

/*!
 * \brief abstract operator class
 */
class Operator : public Formula
{
protected:
  // child formulae
  std::set<Formula*> children_;
  // clear children
  void clear();
public:
  // whether operator contains a given literal
  bool isIn(const Literal &) const;
};

/*!
 * \brief conjunction class
 */
class Conjunction : public Operator
{
  friend class Disjunction;
private:
  // yet another constructor
  Conjunction(const std::set<Formula *> &);
  // remove "and true" and "x and x"
  Formula * killLiterals() const;
public:
  // constructor
  Conjunction(Formula *, Formula *);
  // copyconstructor
  Conjunction(const Conjunction &);
  // destructor
  ~Conjunction();
  // clone the formula
  Formula * clone() const;
  // disjunctive normal form
  Formula * dnf() const;
  // whether formula is satisfied
  bool sat(const std::set<std::string> &) const;
  // simplifies the formula
  Formula * simplify(const std::set<std::string> &) const;
  // comparison operator
  bool operator==(const Formula &) const;
  // whether (NOT this) implies (NOT the other formula)
  bool operator<=(const Formula &) const;
  // write to stream
  std::ostream & out(std::ostream &);
};

/*!
 * \brief disjunction class
 */
class Disjunction : public Operator
{
  friend class Conjunction;
private:
  // remove "or false" and "x or x"
  Formula * killChildren(const std::set<std::string> &) const;
public:
  // constructor
  Disjunction(Formula *, Formula *);
  // yet another constructor
  Disjunction(const std::set<Formula *> &);
  // copyconstructor
  Disjunction(const Disjunction &);
  // destructor
  ~Disjunction();
  // clone the formula
  Formula * clone() const;
  // disjunctive normal form
  Formula * dnf() const;
  // whether formula is satisfied
  bool sat(const std::set<std::string> &) const;
  // simplifies the formula
  Formula * simplify(const std::set<std::string> &) const;
  // comparison operator
  bool operator==(const Formula &) const;
  // whether (NOT this) implies (NOT the other formula)
  bool operator<=(const Formula &) const;
  // write to stream
  std::ostream & out(std::ostream &);
};

/*!
 * \brief literal class
 * \note  the constants false, final and true are also literals
 */
class Literal : public Formula
{
private:
  // actual literal
  std::string literal_;
public:
  // constructor
  Literal(const std::string &);
  // constructor
  Literal(bool);
  // copyconstructor
  Literal(const Literal &);
  // clone the formula
  Formula * clone() const;
  // comparison operator
  bool operator==(const std::string &) const;
  // comparison operator
  bool operator==(const Formula &) const;
  // comparison operator
  bool operator==(bool) const;
  // whether (NOT this) implies (NOT the other formula)
  bool operator<=(const Formula &) const;
  // disjunctive normal form
  Formula * dnf() const;
  // whether formula is satisfied
  bool sat(const std::set<std::string> &) const;
  // simplifies the formula
  Formula * simplify(const std::set<std::string> &) const;
  // write to stream
  std::ostream & out(std::ostream &);
};

// syntactic sugar
std::ostream & operator<<(std::ostream &, Formula *);

#endif /*FORMULA_H_*/
