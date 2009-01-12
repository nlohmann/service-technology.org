#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <vector>
#include <set>
#include <string>
#include <map>

using std::vector;
using std::set;
using std::string;
using std::map;

namespace PNapi
{


/******************************************************************************
 * Data structure
 *****************************************************************************/

/*!
 * \brief   place and transition types
 *
 *          Enumeration of the possible types of a place or transition, i.e.
 *          whether a place or transition is internal, receives a message,
 *          sends a message or both.
 *
 * \ingroup petrinet
 */
typedef enum
{
  INTERNAL,     ///< non-communicating (standard)
  IN,           ///< input of an open workflow net (oWFN)
  OUT,          ///< output of an open workflow net (oWFN)
  INOUT         ///< input and output of an open workflow net (oWFN)
} communication_type;





/*!
 * \brief   arc types
 *
 *          Enumeration of the possible types of an arc, i.e. whether it is a
 *          standard arc or a read arc. The latter will be "unfolded" to a
 *          loop.
 *
 * \ingroup petrinet
 */
typedef enum
{
  STANDARD,     ///< standard arc
  READ          ///< read arc (will be unfolded to a loop)
} arc_type;





/*!
* \brief   node types
 *
 *          Enumeration of the possible types of a node, i.e. whether it is a
 *          place or a transition.
 *
 * \ingroup petrinet
 */
typedef enum
{
  PLACE,        ///< a place
  TRANSITION        ///< a transition
} node_type;





/*!
* \brief   file formats
 *
 *          Enumeration of the possible output file formats.
 *
 * \ingroup petrinet
 */
typedef enum
{
  FORMAT_APNN,      ///< Abstract Petri Net Notation (APNN)
  FORMAT_DOT,       ///< Graphviz dot
  FORMAT_INA,       ///< INA
  FORMAT_SPIN,      ///< INA
  FORMAT_INFO,      ///< Info File
  FORMAT_LOLA,      ///< LoLA
  FORMAT_OWFN,      ///< Fiona open workflow net (oWFN)
  FORMAT_PEP,       ///< Low-Level PEP Notation
  FORMAT_PNML,      ///< Petri Net Markup Language (PNML)
  FORMAT_GASTEX ///< GasTeX format
} output_format;

/*!
 * \brief   file formats
 *
 *          Enumeration of the possible input file formats.
 *
 * \ingroup petrinet
 */
typedef enum
{
  INPUT_OWFN      ///< Fiona open workflow net (oWFN)
} input_format;

/*!
* \brief   Marking
 *
 *          A set of tokens in places according to the marking_id in each place.
 *
 * \ingroup petrinet
 */
typedef vector<unsigned int> Marking;


struct PetrifyResult
{
  set<string> transitions;
  set<string> places;
  set<string> initialMarked;
  set<string> interface;
  map<string, set<string> > arcs;
};


} /* namespace PNapi */

#endif /*TYPEDEFS_H_*/
