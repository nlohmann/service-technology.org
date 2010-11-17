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

package hub.top.petrinet.util;

import java.util.ArrayList;
import java.util.HashSet;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

/**
 * This class provides several structural reduction techniques for {@link PetriNet}s.
 * 
 * TODO: complete the routines, classify which routines preserve which properties
 * 
 * @author dfahland
 */
public class StructuralReduction {

  private PetriNet net;
  
  /**
   * Prepare structural reductions for the {@link PetriNet} net. Call
   * {@link #reduce()} to reduce the net.
   * 
   * @param net
   */
  public StructuralReduction(PetriNet net) {
    this.net = net;
  }
  
  /**
   * @param p1
   * @param p2
   * @return <code>true</code> iff p1 and p2 are parallel places, i.e. have
   * exactly the same pre-set, post-set, and initial marking.
   */
  public boolean parallelPlaces(Place p1, Place p2) {
    if (   p1.getPreSet().size() != p2.getPreSet().size()
        || p1.getPostSet().size() != p2.getPostSet().size())
    {
      return false;
    }
    
    for (Transition t : p1.getPreSet())
      if (!p2.getPreSet().contains(t)) return false;
    for (Transition t : p1.getPostSet())
      if (!p2.getPostSet().contains(t)) return false;
    return true;
  }
  
  /**
   * Find any two places that are parallel {@link #parallelPlaces(Place, Place)}
   * and remove one of them.
   * @return <code>true</code> iff a parallel place was removed
   */
  public boolean reduceParallelPlaces() {
    ArrayList<Place> ps = new ArrayList<Place>(net.getPlaces());
    for (int i=0;i<ps.size();i++) {
      for (int j=i+1;j<ps.size();j++) {
        Place p1 = ps.get(i);
        Place p2 = ps.get(j);
        if (parallelPlaces(p1, p2)) {
          net.removePlace(p1);
          return true;
        }
      }
    }
    return false;
  }
  
  /**
   * @param t1
   * @param t2
   * @return <code>true</code> iff t1 and t1 are sequential transitions
   * separated by the same set of places.
   */
  public boolean sequentialTransitions(Transition t1, Transition t2) {
    if (   t1.getPostSet().size() != t2.getPreSet().size() )
      return false;
    
    for (Place p : t1.getPostSet())
      if (!t2.getPreSet().contains(p)) return false;
    return true;
  }
  
  /**
   * Find any two transitions that are sequential {@link #sequentialTransitions(Transition, Transition)}
   * and remove one of them.
   * @return <code>true</code> iff a sequential transition was removed
   */
  public boolean reduceSequenceTransitions() {
    ArrayList<Transition> ts = new ArrayList<Transition>(net.getTransitions());
    for (int i=0;i<ts.size();i++) {
      for (int j=i+1;j<ts.size();j++) {
        Transition t1 = ts.get(i);
        Transition t2 = ts.get(j);
        if (sequentialTransitions(t1, t2) || sequentialTransitions(t2, t1)) {
          if (t1.tau) {
            net.removeTransition(t1);
          } else if (t2.tau) {
            net.removeTransition(t2);
          }
          return true;
        }
      }
    }
    return false;
  }
  
  /**
   * Find any two transitions that are parallel {@link PetriNet#parallelTransitions(Transition, Transition)}
   * and remove one of them.
   * @return <code>true</code> iff a parallel transition was removed
   */
  public boolean reduceParallelTransitions() {
    ArrayList<Transition> ts = new ArrayList<Transition>(net.getTransitions());
    for (int i=0;i<ts.size();i++) {
      for (int j=i+1;j<ts.size();j++) {
        Transition t1 = ts.get(i);
        Transition t2 = ts.get(j);
        if (net.parallelTransitions(t1, t2)) {
          if (t1.tau) {
            net.removeTransition(t1);
          } else if (t2.tau) {
            net.removeTransition(t2);
          }
          return true;
        }
      }
    }
    return false;
  }
  
  /**
   * Find and remove a tau-labeled transition that has one pre-place and
   * one post-place by merging the two places.
   * @return <code>true</code> if a tau-transition has been removed.
   */
  public boolean reduceTauTransition() {
    ArrayList<Transition> ts = new ArrayList<Transition>(net.getTransitions());
    for (int i=0;i<ts.size();i++) {
      Transition t = ts.get(i);
      if (!t.isTau()) continue;
      if (t.getPreSet().size() != 1) continue;
      if (t.getPostSet().size() != 1) continue;
      
      HashSet<Transition> prePost = new HashSet<Transition>();
      for (Place p : t.getPreSet()) {
        prePost.addAll(p.getPostSet());
      }
      HashSet<Transition> postPre = new HashSet<Transition>();
      for (Place p : t.getPostSet()) {
        postPre.addAll(p.getPreSet());
      }
      if (prePost.size() == 1 && postPre.size() == 1) {
        // t is the only post-transition of its pre-set
        // and the only pre-transition of its post-set
        net.contractTransition(t);
        return true;
      }

    }
    return false;
  }
  
  /**
   * Find and remove a tau-labeled transition that has multiple pre- and
   * post-places by replacing the tau-transition and its adjacent places
   * with the strong-concurrency pattern {@link PetriNet#replaceTransitionStrongCo(Transition)}.
   * 
   * @return <code>true</code> if a tau-transition has been removed.
   */
  public boolean reduceTauTransition2() {
    ArrayList<Transition> ts = new ArrayList<Transition>(net.getTransitions());
    for (int i=0;i<ts.size();i++) {
      Transition t = ts.get(i);
      if (!t.isTau()) continue;
      
      HashSet<Transition> prePost = new HashSet<Transition>();
      for (Place p : t.getPreSet()) {
        prePost.addAll(p.getPostSet());
      }
      HashSet<Transition> postPre = new HashSet<Transition>();
      for (Place p : t.getPostSet()) {
        postPre.addAll(p.getPreSet());
      }
      if (prePost.size() == 1 && postPre.size() == 1) {
        // t is the only post-transition of its pre-set
        // and the only pre-transition of its post-set
        net.replaceTransitionStrongCo(t);
        return true;
      }

    }
    return false;
  }
  
  /**
   * Find and remove a tau-labeled place by merging the place's pre- and
   * post-transition (must be 1 each). {@link PetriNet#contractPlace(Place)}.
   * 
   * @return  <code>true</code> if a tau-place has been removed.
   */
  public boolean reduceTauPlace() {
    ArrayList<Place> ps = new ArrayList<Place>(net.getPlaces());
    for (int i=0;i<ps.size();i++) {
      Place p = ps.get(i);
      if (p.getPreSet().size() != 1) continue;
      if (p.getPostSet().size() != 1) continue;
      if (p.getTokens() != 0) continue;
      
      //HashSet<Place> prePost = new HashSet<Place>();
      //for (Transition t : p.getPreSet()) {
      //  prePost.addAll(t.getPostSet());
      //}
      HashSet<Place> postPre = new HashSet<Place>();
      for (Transition t : p.getPostSet()) {
        postPre.addAll(t.getPreSet());
      }
      if (/*prePost.size() == 1 &&*/ postPre.size() == 1) {
        // t is the only post-transition of its pre-set
        // and the only pre-transition of its post-set
        Transition t1 = p.getPreSet().get(0);
        Transition t2 = p.getPostSet().get(0);
        
        if (!t1.isTau()) continue;
        if (!t2.isTau()) continue;
        
        net.contractPlace(p);
        
        return true;
      }

    }
    return false;
  }
  
  /**
   * Find and remove a tau-labeled place by merging the place's pre- and
   * post-transition (must be 1 each). {@link PetriNet#contractPlace(Place)}.
   * 
   * @return  <code>true</code> if a tau-place has been removed.
   */
  public boolean reduceTauPlace2() {
    ArrayList<Place> ps = new ArrayList<Place>(net.getPlaces());
    for (int i=0;i<ps.size();i++) {
      Place p = ps.get(i);
      
      if (p.getPreSet().size() != 1) continue;
      if (p.getPostSet().size() != 1) continue;
      if (p.getTokens() != 0) continue;
      
      Transition tPre = p.getPreSet().get(0);
      Transition tPost = p.getPostSet().get(0);
      
      if (!tPost.isTau()) continue;
      
      HashSet<Transition> prePre = new HashSet<Transition>();
      for (Place q : tPre.getPreSet()) {
        prePre.addAll(q.getPreSet());
      }
      HashSet<Transition> postPre = new HashSet<Transition>();
      for (Place q : tPost.getPreSet()) {
        if (q == p) continue;
        postPre.addAll(q.getPreSet());
      }
      
      if (prePre.containsAll(postPre)) {
        
        net.contractPlace(p);
        
        return true;
      }

    }
    return false;
  }
  
  /**
   * Apply all available reductions on the net until no more reductions
   * can be applied.
   */
  public void reduce() {
    while (true) {
      
      if (reduceParallelPlaces()) continue;
      if (reduceParallelTransitions()) continue;
      //if (reduceTauTransition()) continue;
      if (reduceTauTransition2()) continue;
      //if (reduceTauPlace()) continue;
      if (reduceTauPlace2()) continue;
      
      break;
    }
  }
  
  public static void main(String [] args) throws Exception {
    String rule = args[0];
    String fileName = args[1];

    PetriNet net = PetriNetIO.readNetFromFile(fileName);
    
    if (rule.equals("tau-transition-2")) {
      
      for (int i=2; i<args.length; i++) {
        net.findTransition(args[i]).setTau(true);
      }
      
      StructuralReduction red = new StructuralReduction(net);
      while (red.reduceTauTransition2());
      
    } else if (rule.equals("tau-place-2")) {

      for (int i=2; i<args.length; i++) {
        net.findTransition(args[i]).setTau(true);
      }
      
      StructuralReduction red = new StructuralReduction(net);
      while (red.reduceTauPlace2());
      
    }
    
    PetriNetIO.writeToFile(net, fileName, PetriNetIO.FORMAT_DOT, 0);
  }
}
