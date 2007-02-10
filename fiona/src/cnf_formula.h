#ifndef CNF_FORMULA_H
#define CNF_FORMULA_H

#include <set>
#include <string>

/*!
 * A class to organize and simplify CNF (conjunctive normal form) formulas.
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *
 * \todo  Handling of "false" inside formulas.
 */
class CNF_Formula
{
  /// a clause is a set of literals (strings)
  typedef std::set<std::string>	Clause;

  /// a formula is a set of clauses
  typedef std::set<Clause>	Formula;

  private:
    /// the CNF formula as set of set of literals
    Formula my_formula;

    /// Is a set of literals a superset of another set of literals?
    bool is_superset(const Clause *, const Clause *);

  public:
    /// creates a string representation of the CNF formula
    std::string to_string();

    /// simplifies the CNF formula
    void simplify();

    /// constructs a CNF formula object from a string
    CNF_Formula(std::string &);
};

#endif
