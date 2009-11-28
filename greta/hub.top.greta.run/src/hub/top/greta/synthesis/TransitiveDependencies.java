/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.greta.synthesis;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import hub.top.adaptiveSystem.Oclet;
import hub.top.greta.oclets.canonical.DNode;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.DNodeSet;
import hub.top.greta.oclets.canonical.DNodeSys;

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
  
  int eventNum;
  DNode[] events;
  DNodeSys system;
  

  public TransitiveDependencies(DNodeSys sys) {
    system = sys;
    
    eventNum = sys.fireableEvents.length() + sys.preConEvents.length();
    events = new DNode[eventNum];
    
    int i=0;
    for (DNode e : sys.preConEvents) events[i++] = e;
    for (DNode e : sys.fireableEvents) events[i++] = e;
    
    computeTransitiveDependencies();
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
    
    directSucc = new boolean[eventNum][eventNum];
    transSucc = new boolean[eventNum][eventNum];

    // extract the direct successor relations between transitions from the oclets
    for (int i=0; i<eventNum; i++) {
      DNode e = events[i];
      if (e.post == null) continue;
      for (DNode b : e.post) {
        if (b.post == null) continue;
        for (DNode e2 : b.post) {
          int j = eventIndex(e2);
          directSucc[i][j] = true;
          transSucc[i][j] = true;
        }
      }
    }
    
    // then compute the transitive closure of the successor relations
    int added = 0;
    do {
      added = 0;
      for (int i=0; i<transSucc.length; i++) {
        for (int j=0; j<transSucc.length; j++) {
          if (i==j) continue;
          for (int k=0; k<transSucc.length; k++) {
            if (j == k) continue;
            if (transSucc[i][j] && transSucc[j][k]) {
              if (!transSucc[i][k]) {
                transSucc[i][k] = true;
                added++;
              }
            }
          }
        }
      }
    } while (added > 0);
    /*
    for (int i=0;i<eventNum; i++) {
      for (int j=0;j<eventNum; j++) {
        if (transSucc[i][j] && !directSucc[i][j])
          System.out.println(events[i]+" ==>* "+events[j]);
        else if (directSucc[i][j])
          System.out.println(events[i]+" -->  "+events[j]);
        else if (transSucc[i][j])
          System.out.println(events[i]+" -->* "+events[j]);
      } 
    }
    */
  }
  
  /**
   * @param e
   * @return
   *    the index of event e in {@link #events}, i.e. <code>events[eventIndex(e)] = e</code>
   */
  private int eventIndex(DNode e) {
    for (int j = 0; j<eventNum; j++) if (events[j] == e) return j;
    
    assert false; // this should never happen
    return eventNum;
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
        DNode t = system.getTransitionForID(e.causedBy[i]);
        extend(e, t, e, eventIndex(t));
      }
    }

    Short tauID = system.nameToID.get(DNodeSys.NAME_TAU);
    
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
