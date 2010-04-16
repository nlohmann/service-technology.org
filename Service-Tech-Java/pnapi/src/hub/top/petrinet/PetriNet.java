/*****************************************************************************\
 * Copyright (c) 2010. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Petri Net API/Java
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.petrinet;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

/**
 * A Petri net. Base class of the Petri net API. Provides all basic functions for
 * manipulating the net (create/delete nodes and arcs, ...)
 * 
 * @author Dirk Fahland
 */
public class PetriNet {

  // all places of the net
  private HashSet<Place> places;
  // all transitions of the net
  private HashSet<Transition> transitions;
  // all arcs of the net
  private HashSet<Arc> arcs;
  
  /**
   * Create an empty Petri net
   */
  public PetriNet () {
    this.places = new HashSet<Place>();
    this.transitions = new HashSet<Transition>();
    this.arcs = new HashSet<Arc>();
  }
  
  /**
   * Add place with the given name to the net. The net must not contain a place
   * of the same name.
   * 
   * @param name
   * @return the created place
   */
  public Place addPlace(String name) {
    if (findPlace(name) != null) {
      assert false : "Tried to add existing place";
      return null;
    }
    
    Place p = new Place(name);
    places.add(p);
    return p;
  }

  /**
   * Add transition with the given name to the net. The net must not contain a transition
   * of the same name.
   * 
   * @param name
   * @return the created transition
   */
  public Transition addTransition(String name) {
    if (findTransition(name) != null) {
      
      assert false : "Tried to add existing transition";
      return null;
    }
    
    Transition t = new Transition(name);
    transitions.add(t);
    return t;
  }
  
  /**
   * Create an arc from source to target in the net, if the arc is not
   * present (otherwise return existing arc)
   * 
   * @param src
   * @param tgt
   * @return arc from src to tgt
   */
  private Arc _addArc(Node src, Node tgt) {
    for (Arc a : arcs) {
      if (a.getSource() == src && a.getTarget() == tgt) return a;
    }
    
    Arc a = new Arc(src,tgt);
    arcs.add(a);
    return a;
  }
  
  /**
   * Create a new arc from {@link Transition} t to {@link Place} p. 
   * @param t
   * @param p
   * @return created arc
   */
  public Arc addArc(Transition t, Place p) {
    if (!transitions.contains(t) || !places.contains(p)) {
      assert false : "Tried to draw arc between non-existing nodes";
      return null;
    }
    return _addArc(t,p);
  }
  
  /**
   * Create a new arc from {@link Place} p to {@link Transition} t.
   * @param p
   * @param t
   * @return created arc
   */
  public Arc addArc(Place p, Transition t) {
    if (!places.contains(p) || !transitions.contains(t)) {
      assert false : "Tried to draw arc between non-existing nodes";
      return null;
    }
    return _addArc(p,t);
  }
  
  /**
   * Create arc from source node to target node. The net must contain nodes
   * with the given names.
   * @param srcName
   * @param tgtName
   * @return created arc
   */
  public Arc addArc(String srcName, String tgtName) {
    // try place -> transitions
    Node src = findPlace(srcName);
    Node tgt = findTransition(tgtName);
    if (src != null && tgt != null) return _addArc(src, tgt);
    
    // else: src == null || tgt == null
    // try transitions -> place
    src = findTransition(srcName);
    tgt = findPlace(tgtName);
    if (src != null && tgt != null) return _addArc(src, tgt);

    assert false : "Tried to draw arc between non-existing nodes";
    return null; 
  }
  
  /**
   * @param name
   * @return the place with the given name if it exists in the net
   */
  public Place findPlace(String name) {
    for (Place p : places) {
      if (p.getName().equals(name))
        return p;
    }
    return null;
  }

  /**
   * @param name
   * @return the transition with the given name if it exists
   */
  public Transition findTransition(String name) {
    for (Transition t : transitions) {
      if (t.getName().equals(name))
        return t;
    }
    return null;
  }
  
  /**
   * Mark the place with the given <code>placeName</code> with <code>num</code> tokens.
   * @param placeName
   * @param num
   */
  public void setTokens(String placeName, int num) {
    Place p = findPlace(placeName);
    if (p != null) p.setTokens(num);
  }
  
  /**
   * Mark the given place <code>p</code> with <code>num</code> tokens.
   * @param p
   * @param num
   */
  public void setTokens(Place p, int num) {
    p.setTokens(num);
  }
  
  /**
   * Change name of node n to newName. Checks if a node having newName already exists.
   * If yes, then an exception is thrown.
   * 
   * @param n
   * @param newName
   */
  public void setName(Node n, String newName) {
    if (n.getName().equals(newName)) return;
    
    if (findPlace(newName) != null) {
      assert false : "Place with name "+newName+" already exists.";
      return;
    }
    if (findTransition(newName) != null) {
      assert false : "Transition with name "+newName+" already exists.";
      return;
    }
    n.setName(newName);
  }

  /**
   * @param t
   * @return pre-places of transition t
   */
  public List<Place> getPreSet(Transition t) {
    LinkedList<Place> preSet = new LinkedList<Place>();
    for (Arc a : getIncoming(t))
      preSet.add((Place)a.getSource()); 
    return preSet;
  }
  
  /**
   * @param p
   * @return pre-transitions of p
   */
  public List<Transition> getPreSet(Place p) {
    LinkedList<Transition> preSet = new LinkedList<Transition>();
    for (Arc a : getIncoming(p))
      preSet.add((Transition)a.getSource());
    return preSet;
  }

  /**
   * @param t
   * @return post-places of t
   */
  public List<Place> getPostSet(Transition t) {
    LinkedList<Place> postSet = new LinkedList<Place>();
    for (Arc a : getOutgoing(t))
      postSet.add((Place)a.getTarget());
    return postSet;
  }
  
  /**
   * @param p
   * @return post-transition of p
   */
  public List<Transition> getPostSet(Place p) {
    LinkedList<Transition> postSet = new LinkedList<Transition>();
    for (Arc a : getOutgoing(p))
      postSet.add((Transition)a.getTarget());
    return postSet;
  }
  
  /**
   * @param n
   * @return incoming arcs of node n
   */
  public List<Arc> getIncoming(Node n) {
    LinkedList<Arc> arcSet = new LinkedList<Arc>();
    for (Arc a : arcs) {
      if (a.getTarget() == n) arcSet.add(a); 
    }
    return arcSet;
  }

  /**
   * @param n
   * @return outgoing arcs of node n
   */
  public List<Arc> getOutgoing(Node n) {
    LinkedList<Arc> arcSet = new LinkedList<Arc>();
    for (Arc a : arcs) {
      if (a.getSource() == n) arcSet.add(a); 
    }
    return arcSet;
  }


  
  /* -----------------------------------------------------------------------
   * 
   *   Net manipulations
   * 
   * ----------------------------------------------------------------------- */
  

  /**
   * Merge the transitions t1 and t2 in the net. Adds pre- and post-places of
   * t2 to the pre- or post-set of t1 and removes t2 from the net.
   * 
   * @param t1
   * @param t2
   */
  public void mergeTransitions(Transition t1, Transition t2) {
    for (Place p : getPreSet(t2)) {
      if (!getPreSet(t1).contains(p)) {
        addArc(p, t1); 
      }
    }
    for (Place p : getPostSet(t2)) {
      if (!getPostSet(t1).contains(p)) {
        addArc(t1,p);
      }
    }
    removeTransition(t2);
  }
  
  /**
   * Merge the places p1 and p2 in the net. Creates a new place pMerge; the
   * pre-and post-transitions of pMerge are the pre- and post-transitions of
   * p1 and p2, respectively.
   * 
   * The original places p1 and p2 are not removed from the net. Use
   * {@link #removePlace(PtNet, Place)} to remove either once this is appropriate.
   * 
   * @param p1
   * @param p2
   * @return the new place pMerge
   */
  public Place mergePlaces(Place p1, Place p2) {
    Place pMerged = addPlace(p1.getName()+"_"+p2.getName());
    setTokens(pMerged, p1.getTokens() + p2.getTokens());

    for (Transition t : getPreSet(p1)) {
      addArc(t, pMerged);
    }
    for (Transition t : getPreSet(p2)) {
      if (getPreSet(pMerged).contains(t)) continue;
      addArc(t, pMerged);
    }
    for (Transition t : getPostSet(p1)) {
      addArc(pMerged, t);
    }
    for (Transition t : getPostSet(p2)) {
      if (getPostSet(pMerged).contains(t)) continue;
      addArc(pMerged, t);
    }
    return pMerged;
  }
  
  /**
   * Remove transition t from the net. Remove also all adjacent arcs.
   * 
   * @param t
   */
  public void removeTransition(Transition t) {
    LinkedList<Arc> a_remove = new LinkedList<Arc>();
    a_remove.addAll(getIncoming(t));
    a_remove.addAll(getOutgoing(t));
    for (Arc a : a_remove) {
      removeArc(a);
    }
    transitions.remove(t);
  }
  
  /**
   * Remove place p from the net. Remove also all adjacent arcs and update
   * the markings where p is involved.
   * 
   * @param p
   */
  public void removePlace(Place p) {
    
    if (!places.contains(p)) {
      System.err.println("Error. Tried to remove place "+p.getName()+" from net, but this place is not contained in the net");
      return;
    }
    
    setTokens(p, 0);
    
    LinkedList<Arc> a_remove = new LinkedList<Arc>();
    a_remove.addAll(getIncoming(p));
    a_remove.addAll(getOutgoing(p));
    for (Arc a : a_remove) {
      removeArc(a);
    }
    places.remove(p);
  }
  
  /**
   * Remove arc a from the net. 
   * @param a
   */
  public void removeArc(Arc a) {
    arcs.remove(a);
  }

  /**
   * Check whether transitions t1 and t2 are parallel (except for the places in
   * the set ignore). Two transitions are parallel if their pre- and post-sets
   * are identical.
   * 
   * @param t1
   * @param t2
   * @param ignore
   * @return
   *    true iff transitions t1 and t2 are parallel except for the places in
   *    the set ignore
   */
  public boolean parallelTransitions(Transition t1, Transition t2, Collection<Place> ignore) {
    for (Place p : getPreSet(t1)) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!getPreSet(t2).contains(p)) return false;
    }
    for (Place p : getPostSet(t1)) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!getPostSet(t2).contains(p)) return false;
    }
    for (Place p : getPreSet(t2)) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!getPreSet(t1).contains(p)) return false;
    }
    for (Place p : getPostSet(t2)) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!getPostSet(t1).contains(p)) return false;
    }
    return true;
  }
  
  /**
   * Check whether transitions t1 and t2 are parallel. Two transitions are
   * parallel if their pre- and post-sets are identical.
   * 
   * @param t1
   * @param t2
   * @return
   *    true iff transitions t1 and t2 are parallel
   */
  public boolean parallelTransitions(Transition t1, Transition t2) {
    return parallelTransitions(t1, t2, null);
  }

  /**
   * structurally reduce the Petri net to remove all tau transitions
   * 
   * TODO: re-implement all branching bisimulation preserving structural reduction rules for branching processes
   */
  public void removeTauTransitions() {
    LinkedList<Transition> taus = new LinkedList<Transition>();
    for (Transition t : transitions) {
      if (t.isTau()) {
        taus.add(t);
      }
    }
    
    boolean netChanged = true;
    while (netChanged) {
      netChanged = false;
      
      for (Transition t : taus) {
        if (getPreSet(t).size() == 1 && getPostSet(t).size() == 1) {
          Place pIn = getPreSet(t).get(0);
          Place pOut = getPostSet(t).get(0);
          mergePlaces(pIn, pOut);
          removeTransition(t);
          taus.remove(t);
          netChanged = true;
          break;
        }
      }
    }
  }

  /**
   * hide any place of the net between two tau transitions
   * (i.e. set it its visibility to tau) 
   */
  public void makePlacesInvisible() {
    for (Place p : places) {
      if (getPreSet(p).size() != 1 || getPostSet(p).size() != 1) continue;
      Transition t1 = getPreSet(p).get(0);
      Transition t2 = getPostSet(p).get(0);
      if (t1.isTau() && t2.isTau())
        p.setTau(true);
    }
  }

  /**
   * Remove all isolated nodes (empty pre-set and empty post-set) from the net.
   */
  public void removeIsolatedNodes() {
    LinkedList<Transition> t_remove = new LinkedList<Transition>();
    for (Transition t : transitions) {
      if (getPreSet(t).size() == 0 && getPostSet(t).size() == 0) {
        t_remove.add(t);
      }
    }
    LinkedList<Place> p_remove = new LinkedList<Place>();
    for (Place p : places) {
      if (getPreSet(p).size() == 0 && getPostSet(p).size() == 0) {
        p_remove.add(p);
      }
    }
    for (Transition t : t_remove) removeTransition(t);
    for (Place p : p_remove) removePlace(p);
  }

  
  /* -----------------------------------------------------------------------
   * 
   *   Output
   * 
   * ----------------------------------------------------------------------- */
  
  /**
   * Write the Petri net in GraphViz dot format
   * 
   * @param net
   * @return
   */
  public String toDot() {
    StringBuilder b = new StringBuilder();
    b.append("digraph BP {\n");
    
    // standard style for nodes and edges
    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
    b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

    String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
    
    HashMap<Node, Integer> nodeIDs = new HashMap<Node, Integer>();
    int nodeID = 0;
    
    // first print all places
    b.append("\n\n");
    b.append("node [shape=circle];\n");
    for (Place p : places) {
      nodeID++;
      nodeIDs.put(p, nodeID);
      
      if (p.getTokens() > 0)
        b.append("  p"+nodeID+" ["+tokenFillString+"]\n");
      else
        b.append("  p"+nodeID+" []\n");
      
      String auxLabel = "";
        
      b.append("  p"+nodeID+"_l [shape=none];\n");
      b.append("  p"+nodeID+"_l -> p"+nodeID+" [headlabel=\""+toLoLA_ident(p.getName())+" "+auxLabel+"\"]\n");
    }

    // then print all events
    b.append("\n\n");
    b.append("node [shape=box];\n");
    for (Transition t : transitions) {
      nodeID++;
      nodeIDs.put(t, nodeID);

      b.append("  t"+nodeID+" []\n");
      
      String auxLabel = "";
      
      b.append("  t"+nodeID+"_l [shape=none];\n");
      b.append("  t"+nodeID+"_l -> t"+nodeID+" [headlabel=\""+toLoLA_ident(t.getName())+" "+auxLabel+"\"]\n");
    }
    
    /*
    b.append("\n\n");
    b.append(" subgraph cluster1\n");
    b.append(" {\n  ");
    for (CNode n : nodes) {
      if (n.isEvent) b.append("e"+n.localId+" e"+n.localId+"_l ");
      else b.append("c"+n.localId+" c"+n.localId+"_l ");
    }
    b.append("\n  label=\"\"\n");
    b.append(" }\n");
    */
    
    // finally, print all edges
    b.append("\n\n");
    b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
    for (Arc arc : arcs) {
      if (arc.getSource() instanceof Transition)
        b.append("  t"+nodeIDs.get(arc.getSource())+" -> p"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
      else
        b.append("  p"+nodeIDs.get(arc.getSource())+" -> t"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
    }
    b.append("}");
    return b.toString();
  }

  /**
   * Convert arbitrary identifier into LoLA-compatible format.
   * @param s
   * @return
   */
  public static String toLoLA_ident(String s) {
    if (s.lastIndexOf('_') >= 0)
      return s.substring(0,s.lastIndexOf('_'));
    else
      return s;
  }
}
