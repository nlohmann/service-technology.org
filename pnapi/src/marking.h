#ifndef MARKING_H
#define MARKING_H

#include <map>
using std::map;

namespace pnapi
{

/// forward declarations
class PetriNet;
class Transition;
class Place;

  /*!
   * \brief   Marking of all places of a net
   *
   * \note    In the future this class might contain a highly efficient
   *          implementation if necessary. For now, we use a simple one to
   *          determine the needed functionality.
   */
  class Marking
  {
  public:
    Marking(PetriNet &n, bool internalsOnly = false);
    Marking(const Marking &m);
    virtual ~Marking() {}

    map<Place *, unsigned int> getMap() const;
    PetriNet & getPetriNet() const;
    bool internalsOnly() const;

    unsigned int size();

    bool activates(const Transition &t);
    Marking & successor(const Transition &t);

    unsigned int & operator[](Place *offset);
    bool operator==(const Marking &m) const;
    Marking & operator=(const Marking &m);

  private:
    map<Place *, unsigned int> m_;
    PetriNet &net_;
    bool internalsOnly_;
  };

} /* end of namespace pnapi */

#endif /* MARKING_H */
