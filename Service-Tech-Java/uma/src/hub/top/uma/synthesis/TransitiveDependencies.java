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

package hub.top.uma.synthesis;

import java.util.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.Uma;

/**
 * Compute the transitive dependencies between events of a branching process
 * if these dependencies are justified by corresponding transitions of oclets.
 * Also allows to extend a given branching process by conditions that
 * materialize these dependencies. These additional conditions support
 * the synthesis of a Petri net from an set of {@link Oclet}s, i.e. see
 * {@link SynthesizeNetAction}.
 * 
 * @author Dirk Fahland
 *
 */
public class TransitiveDependencies {
  
  HashMap<DNode, Integer> nodeIndex;
  int nodeNum;
  DNode[] nodes;
  DNodeBP build;
  DNodeSet bp;

  public TransitiveDependencies(DNodeBP build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    nodeNum = bp.getAllNodes().size();
    nodes = new DNode[nodeNum];
    nodeIndex = new HashMap<DNode, Integer>();
    
    int i=0;
    for (DNode d : bp.getAllNodes()) {
      nodes[i] = d;
      nodeIndex.put(d, i);
      i++;
    }
    
    //computeTransitiveDependencies();
  }
  
  /**
   * <code>events[j]</code> is a direct successor of <code>events[i]</code>
   * iff <code>directSucc[i][j] == true</code>
   */
  boolean directSucc[][];
  
  /**
   * <code>events[j]</code> is a transitive successor of <code>events[i]</code>
   * iff <code>transSucc[i][j] == true</code>
   */
  boolean transSucc[][];

  /**
   * Compute the transitive dependencies between all transitions in all
   * oclets of the {@link #system}.
   */
  private void computeTransitiveDependencies() {
    
    directSucc = new boolean[nodeNum][nodeNum];
    transSucc = new boolean[nodeNum][nodeNum];

    // extract the direct successor relations between nodes
    for (int i=0; i<nodeNum; i++) {
      DNode c = nodes[i];
      if (c.post == null) continue;
      for (DNode d : c.post) {
        int j = nodeIndex.get(d);
        directSucc[i][j] = true;
        transSucc[i][j] = true;
      }
    }
    
    /*
     For each j from 1 to n
      For each i from 1 to n
         If T(i,j)=1, then form the Boolean or of row i and row j 
                           and replace row i by it.
         Go on to the next i-value.
      Once you have processed each i-value, go on to the next j-value.

     */
    for (int j=0; j<transSucc.length; j++) {
      for (int i=0; i<transSucc.length; i++) {
        if (i != j && transSucc[i][j]) {
          for (int k = 0; k<transSucc.length; k++) {
            transSucc[i][k] = transSucc[i][k] || transSucc[j][k];
          }
        }
      }
    }
    
    /*
    for (int i=0;i<nodeNum; i++) {
      for (int j=0;j<nodeNum; j++) {
        if (transSucc[i][j] && !directSucc[i][j])
          System.out.println(nodes[i]+" ==>* "+nodes[j]);
        else if (directSucc[i][j])
          System.out.println(nodes[i]+" -->  "+nodes[j]);
        else if (transSucc[i][j])
          System.out.println(nodes[i]+" -->* "+nodes[j]);
      } 
    }
    */
    
  }
  
  /**
   * Computing version of transitive dependencies. Determines whether
   * x depends on y by breadth-first searching in the {@link DNodeSet}.
   * 
   * @param x
   * @param y
   * @return
   */
  private boolean dependsOn_compute(DNode x, DNode y) {
    
    if (x == y) return true;
    if (x.pre == null) return false;
    
    HashSet<DNode> queueContents = new HashSet<DNode>();
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (DNode d : x.pre) {
      queue.addLast(d);
      queueContents.add(d);
    }
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      queueContents.remove(d);
      if (d == y) return true;
      if (d.pre == null) continue;
      for (DNode d2 : d.pre) {
        if (!queueContents.contains(d2)) {
          queue.addLast(d2);
          queueContents.add(d2);
        }
      }
    }
    return false;
  }
  
  /**
   * Cached version of transitive dependencies. Looks up whether x depends on y
   * on a boolean matrix.
   * 
   * @param x
   * @param y
   * @return
   */
  private boolean dependsOn_cached(DNode x, DNode y) {
    return transSucc[nodeIndex.get(y)][nodeIndex.get(x)];
  }
  
  /**
   * @return The set of all implied conditions, i.e. for each condition b
   * in this set exists a path that connects its pre-set with its post-set.
   * Important: there may be two conditions b1 and b2 that are implied because
   * b1 establishes the path for b2 and vice versa. To determine a maximal set
   * of implied conditions that can be removed, use {@link #getImpliedConditions_solution()}.
   */
  public HashSet<DNode> getAllImpliedConditions() {
    HashSet<DNode> implied = new HashSet<DNode>();
    
    // check for each condition 'b' of the DNodeSet
    for (int i=0; i<nodes.length; i++) {
      DNode b = nodes[i];
      if (b.isEvent || b.pre == null || b.pre.length == 0
          || b.post == null || b.post.length == 0) continue;
      
      // compute for all pre-events e and all post-events f of 'b'
      boolean each_f_dependsOn_e_without_b = true;
      DNode e = b.pre[0];
      for (DNode f : b.post) {
        // whether there exists an alternative path from 'e' to 'f'
        // we find this path by looking for a path from 'e' to a
        // pre-condition 'c' of 'f' that is not 'b'
        boolean f_dependsOn_e_without_b = false;
        for (DNode c : f.pre) {
          
          if (c == b) continue;
          // find the path from 'e' to 'c'
          if ( dependsOn_compute(c, e) ) {
            // found one, 'b' is implied
            f_dependsOn_e_without_b = true;
            break;
          }
        }
        // didn't find any, 'b' is not implied
        if (!f_dependsOn_e_without_b)
          each_f_dependsOn_e_without_b = false;
      }
      
      if (each_f_dependsOn_e_without_b)
        implied.add(b);
    }
    
    return implied;
  }
  
  /**
   * @return A set of implied conditions s.t. after removing
   * all conditions of this set from the {@link DNodeSet}, the
   * resulting {@link DNodeSet} still has the same causal relations
   * as the original set.
   */
  public HashSet<DNode> getImpliedConditions_solution() {
    HashSet<DNode> implied = new HashSet<DNode>();
    
    int maxID = build.getSystem().currentNameID;
    
    // find solution per ID, i.e. first consider all conditions with ID 0,
    // then with ID 1, etc. this ensures that globally, conditions with the lowest
    // ID are classified first as implied, which ensures that after removing
    // implied places, the resulting net has a maximally consistent labeling
    // wrt. pre- and post-places
    //for (int id = 0; id < maxID; id++)
    {
      //Uma.out.print(id+"/"+maxID+": ");
      
      // check for each condition 'b' of the DNodeSet
      for (int i=0; i<nodes.length; i++) {
        //if (nodes[i].id != id) continue;
        
        DNode b = nodes[i];
        if (b.isEvent || b.pre == null || b.pre.length == 0
            || b.post == null || b.post.length == 0) continue;
        
        if (i % 100 == 0) Uma.out.print(i+" ");
        if (i % 1000 == 0) Uma.out.print("\n");
        
        // compute for all pre-events e and all post-events f of 'b'
        boolean each_f_dependsOn_e_without_b = true;
        DNode e = b.pre[0];
        for (DNode f : b.post) {
          // whether there exists an alternative path from 'e' to 'f'
          // we find this path by looking for a path from 'e' to a
          // pre-condition 'c' of 'f' that is not 'b'
          boolean f_dependsOn_e_without_b = false;
          for (DNode c : f.pre) {
            
            if (c == b) continue;
            // find the path from 'e' to 'c'
            // but skip all conditions 'c' that already have been identified as implied
            // and all conditions 'cPrime' that are equivalent to 'c' (and would be
            // folded to the same place as 'c'
            boolean cEquivIsImplied = false;
            for (DNode cPrime : build.futureEquivalence().get(build.equivalentNode().get(c))) {
              if (implied.contains(cPrime)) {
                cEquivIsImplied = true;
                break;
              }
            }
            
            if (cEquivIsImplied) continue;
            
            if ( dependsOn_compute(c, e) ) {
              // found one, 'b' is implied
              f_dependsOn_e_without_b = true;
              break;
            }
          }
          // didn't find any, 'b' is not implied
          if (!f_dependsOn_e_without_b)
            each_f_dependsOn_e_without_b = false;
        }
        
        if (each_f_dependsOn_e_without_b)
          implied.add(b);
      }
      //Uma.out.print("\n");
    }
    
    return implied;
  }
  
  /**
   * @param e
   * @return
   *    the index of event e in {@link #nodes}, i.e. <code>events[eventIndex(e)] = e</code>
   */
  private int eventIndex(DNode e) {
    for (int j = 0; j<nodeNum; j++) if (nodes[j] == e) return j;
    
    assert false; // this should never happen
    return nodeNum;
  }
  
  LinkedList<DNode> transDep_preEvent;
  LinkedList<DNode> transDep_postEvent;
  
  /**
   * Extend the branching process with a transitive dependency from event e
   * to event eStart if there is a transitive dependency between their
   * corresponding oclet transitions t and tStart (tStart = events[tStartIndex]).
   * 
   * Then descend recursively along all predecessors e' of e (and corresponding
   * predecessors t' of t) and extend the branching process with depenencies
   * from e' to eStart.
   * 
   * The transitive dependencies are stored in two linked list {@link #transDep_preEvent}
   * and {@link #transDep_postEvent}. There is a transitive dependency between
   * <code>transDep_preEvent[i]</code> and <code>transDep_postEvent[i]</code>
   * for each <code>i</code>. 
   * 
   * @param e
   * @param t
   * @param eStart
   * @param tStartIndex
   */
  public void extend(DNode e, DNode t, DNode eStart, int tStartIndex) {
    if (e.id != t.id) return;
  
    if (e.isEvent) {
      // System.out.print(e+" -?-> "+eStart);
      int tIndex = eventIndex(t);
      if (transSucc[tIndex][tStartIndex] && !directSucc[tIndex][tStartIndex]) {
        // transition t is a transitive (and no direct) predecessor of tStart
        // store the source and the target of the new dependency in the lists
        transDep_preEvent.addLast(e);
        transDep_postEvent.addLast(eStart);
      }
    }
    
    // descend recursively along the predecessor of e and t likewise
    if (t.pre == null || t.pre.length == 0) return;
    
    for (int i=0,j=0; i<e.pre.length; i++) {
      // find the j-th predecessor of t that corresponds to the i-th predecessor of e
      while (t.pre[j].id < e.pre[i].id && j<t.pre.length) j++;
      if (j == t.pre.length) break;
      extend(e.pre[i], t.pre[j], eStart, tStartIndex);
    }
  }
  
  /**
   * All conditions that represent a transitive dependency between two events
   * because of an oclet history. This field is written in {@link #extendBranchingProcess(DNodeBP)}.
   */
  public LinkedList<DNode> impliedConditions;
  
  /**
   * Extend the branching process bp by conditions between events. A transitive
   * dependency between two transitions of an oclet denotes a history-based
   * enabling condition for firing that later transition. By adding a condition
   * between the corresponding events of the branching process, we materialize
   * this dependency.
   * 
   * Each added condition is only added to the pre/postset of the events, the
   * new condition is NOT added to the set of conditions
   * ({@link DNodeSet#allConditions}) of the branching process. This is exploitet
   * for instance in {@link NetSynthesisLocal#foldToPetriNet().
   * 
   * @param bp
   */
  public void extendBranchingProcess(DNodeBP bp)
  {
    DNodeSet bps = bp.getBranchingProcess();
    transDep_preEvent = new LinkedList<DNode>();
    transDep_postEvent = new LinkedList<DNode>();
    
    // compute the pairs of events of the branching process between which
    // a new condition shall materialize a transitive dependency according
    // to the transitive dependencies of oclet transitions
    for (DNode e : bps.allEvents) {
      // backwards search for each oclet transition t that contributed event e
      // for predecessor events of e on which e transitively depends according to t
      for (int i=0; i<e.causedBy.length; i++) {
        DNode t = build.getSystem().getTransitionForID(e.causedBy[i]);
        extend(e, t, e, eventIndex(t));
      }
    }

    Short tauID = build.getSystem().nameToID.get(DNodeSys.NAME_TAU);
    
    impliedConditions = new LinkedList<DNode>();

    // extend the branching process with a new condition for each transitive
    // dependency stored in transDep_preEvent and transDep_postEvent, i.e.
    // add a new condition as pre- or post-condition of the respective event
    Iterator<DNode> preIt = transDep_preEvent.iterator();
    Iterator<DNode> postIt = transDep_postEvent.iterator();
    while (preIt.hasNext()) {
      DNode preEvent = preIt.next();
      DNode postEvent = postIt.next();
      
      DNode b = new DNode(tauID, 1);
      b.pre[0] = preEvent;
      preEvent.addPostNode(b);
      
      postEvent.addPreNode(b);
      b.addPostNode(postEvent);
      
      b.isAnti = true;  // mark as special
      // System.out.println(preEvent+" --> "+postEvent+ " by "+b);
      // and remember that this is a new condition
      impliedConditions.add(b);
    }
  }
  
  public static void printEquivalenceRelation(HashMap<Short, HashSet<DNode>> eq) {
    System.out.println("equivalence relation:");
    for (Short id : eq.keySet()) {
      System.out.print("[");
      for (DNode b : eq.get(id))
        System.out.print(b+" ");
      System.out.println("]");
    }
  }
}
