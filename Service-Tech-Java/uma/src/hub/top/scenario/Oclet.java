/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
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

package hub.top.scenario;

import java.util.HashSet;
import java.util.LinkedList;

import hub.top.petrinet.Node;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

public class Oclet extends hub.top.petrinet.PetriNet {

  private HashSet<Node> history;
  
  private HashSet<Node> hotNodes;
  
  /**
   * whether this oclet describes an anti-oclet
   */
  public boolean isAnti;
  
  /**
   * the name of the oclet
   */
  public String name;
  
  /**
   * @param anti
   */
  public Oclet(String name, boolean anti) {
    super();
    history = new HashSet<Node>();
    isAnti = anti;
    hotNodes = new HashSet<Node>();
    this.name = name;
  }
  
  /**
   * Create an oclet with the given nodes
   * @param anti
   * 
   * @param conditions_hist node IDs of the conditions in the history
   * @param events_hist     node IDs of the events in the history
   * @param conditions_con  node IDs of the conditions in the contribution
   * @param events_con      node IDs of the events in the contribution
   * @param arcs            arcs modeled as pairs of node IDs, i.e.,
   *  an entry <code>a</code> in <code>arcs</code> has length 2 and
   *  describes an arc from <code>a[0]</code> to <code>a[1]</code>.
   */
  public Oclet(String name, boolean anti, LinkedList<String> conditions_hist, LinkedList<String> events_hist, LinkedList<String> conditions_con, LinkedList<String> events_con, LinkedList<String[]> arcs) {
    this(name, anti);
    
    for (String b : conditions_hist) {
      addPlace(b, true);
    }
    for (String e : events_hist) {
      addTransition(e, true);
    }
    for (String b : conditions_con) {
      addPlace(b, false);
    }
    for (String e : events_con) {
      addTransition(e, false);
    }
    for (String[] a : arcs) {
      if (a.length != 2) continue;
      addArc(a[0],a[1]);
    }
  }

  /**
   * Create a new place in the contribution of this oclet.
   * 
   * @param name
   */
  @Override
  public Place addPlace(String name) {
    return addPlace(name, false);
  }
  
  /**
   * Create a new place in the contribution of this oclet.
   * 
   * @param name
   */
  @Override
  public Place addPlace_unique(String name) {
    // TODO Auto-generated method stub
    return super.addPlace_unique(name);
  }
  
  /**
   * Create a new place in the contribution or the history of this oclet.
   * 
   * @param name
   * @param history
   */
  public Place addPlace(String name, boolean history) {
    Place p = super.addPlace(name);
    if (history) this.history.add(p);
    return p;
  }
  
  /**
   * Create a new transition in the contribution of this oclet.
   * 
   * @param name
   */
  @Override
  public Transition addTransition_unique(String name) {
    return addTransition(name, false);
  }
  
  /**
   * Create a new transition in the contribution of this oclet.
   * 
   * @param name
   */
  @Override
  public Transition addTransition(String name) {
    return addTransition(name, false);
  }
  
  /**
   * Create a new transition in the contribution or the history of this oclet.
   * 
   * @param name
   * @param history
   */
  public Transition addTransition(String name, boolean history) {
    Transition t = super.addTransition(name);
    if (history) this.history.add(t);
    return t;
  }
  
  /**
   * @return <code>true</code> iff the oclet is an anti-oclet
   */
  public boolean isAntiOclet() {
    return isAnti;
  }
  
  /**
   * @param n
   * @return <code>true</code> iff node n is node of the history
   */
  public boolean isInHistory(Node n) {
    return history.contains(n);
  }
  
  /**
   * Set temperature of node to hot. 
   * @param n
   */
  public void makeHotNode(Node n) {
    if (!getPlaces().contains(n) && !getTransitions().contains(n)) return;
    hotNodes.add(n);
  }
  
  /**
   * Set temperature of node to hot. 
   * @param n
   */
  public void makeColdNode(Node n) {
    if (!getPlaces().contains(n) && !getTransitions().contains(n)) return;
    hotNodes.remove(n);
  }
  
  /**
   * @param n
   * @return <code>true</code> iff Node 'n' is hot
   */
  public boolean isHotNode(Node n) {
    return hotNodes.contains(n);
  }
  
  /**
   * @return the list of maximal conditions of this oclet's history
   */
  public LinkedList<Place> getMaxHistory() {
    LinkedList<Place> max = new LinkedList<Place>();
    for (Place p : getPlaces()) {
      if (isInHistory(p)) {
        boolean successorInHist = false;
        for (Transition t : p.getPostSet()) {
          if (isInHistory(t)) {
            successorInHist = true;
            break;
          }
        }
        if (!successorInHist)
          max.add(p);
      }
    }
    return max;
  }


  /**
   * @return <code>true</code> iff the net is a causal net: every condition
   * has at most pre-transition and at most one post-transition and the
   * net is acyclic 
   */
  public boolean isCausalNet() {
    // the empty net is a causal net
    if (getPlaces().size() == 0 && getTransitions().size() == 0)
      return true;
    
    // each condition has at most one pre-event and at most one post-event
    for (Place p : getPlaces()) {
      if (p.getIncoming().size() > 1 || p.getOutgoing().size() > 1)
        return false;
    }
    
    // an oclet is acyclic
    LinkedList<Node> dfsStack = new LinkedList<Node>();
    HashSet<Node> visited = new HashSet<Node>();
    for (Transition t : getTransitions()) {
      if (t.getIncoming().size() == 0) {
        dfsStack.add(t);
      }
    }
    for (Place p : getPlaces()) {
      if (p.getIncoming().size() == 0) {
        dfsStack.add(p);
      }
    }
    // if the net has no minimal nodes, then it contains a cycle
    if (dfsStack.isEmpty()) return false;
    

    while (!dfsStack.isEmpty()) {
      Node n = dfsStack.getFirst();
      
      boolean changedStack = false;
      for (Node m : n.getPostSet()) {
        // reaching node 'm' again on the depth-first stack
        if (dfsStack.contains(m)) return false;
        // otherwise, explore an unexplored successor of 'n'
        if (!visited.contains(m)) {
          dfsStack.addFirst(m);
          visited.add(m);
          changedStack = true;
          break;
        }
      }
      
      if (!changedStack) {
        // all successors of 'n' have been seen, pop the node
        dfsStack.removeFirst();
      }
    }
    return true;
  }

  public HashSet<Node> getNodes() {
    HashSet<Node> nodes = new HashSet<Node>();
    nodes.addAll(getPlaces());
    nodes.addAll(getTransitions());
    return nodes;
  }
  
  /**
   * @return <code>true</code> iff the history of the oclet is a
   * prefix of the entire underlying net
   */
  public boolean historyIsPrefix() {
    for (Node n : getNodes()) {
      if (isInHistory(n)) {
        for (Node m : n.getPreSet()) {
          if (!isInHistory(m)) return false;
        }
      }
    }
    return true;
  }

  /**
   * @return <code>true</code> iff the net satisfies all structural
   * constraints of an oclet: a labeled causal net where the history
   * is a prefix
   */
  public boolean isValidOclet() {

    if (!isCausalNet()) return false;
    if (!historyIsPrefix()) return false;
    
    // each event in the history of an oclet has also all post-conditions
    // in the history
    for (Transition t : getTransitions()) {
      if (isInHistory(t)) {
        for (Place p : t.getPostSet()) {
          if (!isInHistory(p)) {
            return false;
          }
        }
      }
    }
    return true;
  }
}
