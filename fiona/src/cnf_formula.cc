#include "cnf_formula.h"





/*!
 * The test whether a clause is a superset of another is used in the
 * simplification function "simplify".
 *
 * \param a	a clause (a set of string literals)
 * \param b	another clause (a set of string literals)
 * \returns	true iff a is a superset of b
 */
bool CNF_Formula::is_superset(const Clause *a, const Clause *b)
{
  for (Clause::iterator it = b->begin(); it != b->end(); it++)
    if (a->find(*it) == a->end())
      return false;

  return true;
}





/*!
 * The given string is parsed and cut into literals which form clauses: every
 * text in brackets is a clause, separated by the conjunction sign "*". Each
 * clause consists of literals separated by the disjunction sign "+" sign.
 *
 * The generated formula consists of a set of clauses, each clause consisting
 * of a set of literals. Clauses and literals are sorted an uniquified during
 * parsing of the given string.
 *
 * \param formula_text	a string representation of a CNF formula
 *
 * \pre   The string "formula_text" follows the syntactical rules:
 *        - clauses are always put in parentheses
 *        - disjunction of literals are expressed with "[literal] + [literal]"
 *        - conjunction of clauses are expressed with "([clause] * [clause])"
 *
 * \post  The passed string "formula_text" is changed during the execution of
 *        this function: it is passed as reference and not copied for
 *        performance reasons.
 */
CNF_Formula::CNF_Formula(std::string &formula_text)
{
  do // iterate the clauses
  {
    std::string clause_text;    
    unsigned int next_and = formula_text.find_first_of("*");
    if (next_and == UINT_MAX) // last clause
    {
      clause_text = formula_text.substr(1, formula_text.size() -2); // strip the brackets
      formula_text.erase(); // remove rest of formula
    }
    else
    {
      clause_text = formula_text.substr(1, next_and -3);  // strip the brackets
      formula_text.erase(0, next_and +2);      
    }

    Clause current_clause;

    do // iterate the literals
    {
      std::string literal_text;
      unsigned int next_or = clause_text.find_first_of("+");
      if (next_or == UINT_MAX) // last literal
      {
	literal_text = clause_text;
	clause_text.erase();  // remove rest of clause
      }
      else
      {
	literal_text = clause_text.substr(0, next_or -1); // strip whitespace
	clause_text.erase(0, next_or +2);
      }

      current_clause.insert(literal_text);

    } while (clause_text != "");


    my_formula.insert(current_clause); // add the clause to the formula
  }
  while (formula_text != "");
}





/*!
 * The CNF formula is returned as a string using the syntax of the input string.
 *
 * \return  the string representation of the formula
 */
std::string CNF_Formula::to_string()
{
  std::string result;

  for (Formula::iterator clause = my_formula.begin(); clause != my_formula.end(); clause++)
  {
    if (clause != my_formula.begin())
      result += ") * ("; // separate clauses

    for (Clause::iterator literal = (*clause).begin(); literal != (*clause).end(); literal++)
    {
      if (literal != (*clause).begin())
	result += " + "; // separate literals

      result += *literal;
    }
  }

  return "(" + result + ")"; // add outer-most brackets
}





/*!
 * The formula can be simplified by removing all superset clauses. A clause c1
 * is a superset clause when there exists a distinct clause c2 being a subset
 * of c1. Then c1 can be removed as all fulfilling assignments of c1 are also
 * fulfilling assignments of c2; that is, c2 is "stricter" than c1.
 *
 * \post  The formula does not contain any superset clauses.
 */
void CNF_Formula::simplify()
{
  // find superset clauses
  Formula supersets;
  for (Formula::iterator c1 = my_formula.begin(); c1 != my_formula.end(); c1++)
  {
    for (Formula::iterator c2 = my_formula.begin(); c2 != my_formula.end(); c2++)
    {
      if (*c1 != *c2)
      {
	if (is_superset(&*c1, &*c2))
	  supersets.insert(*c1);
      }
    }
  }

  // remove superset clauses
  for (Formula::iterator clause = supersets.begin(); clause != supersets.end(); clause++)
    my_formula.erase(*clause);
}
