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

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PNAPI;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.greta.oclets.canonical.DNode;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.DNodeSet;
import hub.top.greta.oclets.canonical.DNodeSys;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;

/**
 * Synthesize a (possibly labeled) Petri net from an AdaptiveSystem
 * by folding the branching process along its cut-off equivalence
 * relation.
 * 
 * @author Dirk Fahland
 */
public class NetSynthesis {

  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param bp
   * @return
   */
  public static PtNet foldToNet_labeled(DNodeBP bp) {
    
    bp.printEquivalenceRelation();
    
    try {
      bp.minimize();
      bp.relax();
    } catch (NullPointerException e) {
      
    }
    
    bp.printEquivalenceRelation();
    
    DNodeSys dAS = bp.getSystem();
    DNodeSet dBP = bp.getBranchingProcess();
    
    // maps each DNode to its least equivalent DNode (to itself it is the least)
    HashMap<DNode, DNode> equiv = bp.getElementary_ccPair();
    // map that assigns each DNode to its synthesize net counter-part
    // built up during net construction
    HashMap<DNode, Node> d2n = new HashMap<DNode, Node>();
    
    PtnetLoLAFactory fact = PtnetLoLAPackage.eINSTANCE.getPtnetLoLAFactory();
    PtNet net = fact.createPtNet();
    
    for (DNode b: dBP.getAllConditions()) {
      if (equiv.get(b) == null || equiv.get(b) == b) {
        Place p = fact.createPlace();
        p.setName(dAS.properNames[b.id]+"_"+b.globalId);
        net.getPlaces().add(p);
        
        if (b.pre == null || b.pre.length == 0)
          p.setToken(1);
        
        d2n.put(b, p);
      }
    }
    
    // now map each condition that has an equivalent counterpart to
    // the place that represents this counterpart
    for (DNode b: dBP.getAllConditions()) {
      if (equiv.get(b) != null && equiv.get(b) != b && !b.isAnti) {
        d2n.put(b, d2n.get(equiv.get(b)));
      }
    }

    for (DNode e : dBP.getAllEvents()) {
      
      if (!e.isCutOff || equiv.get(e) == e) {
        Transition t = fact.createTransition();
        t.setName(dAS.properNames[e.id]+"_"+e.globalId);
        net.getTransitions().add(t);
        d2n.put(e, t);
      }
    }
    
    for (DNode e : dBP.getAllEvents()) {
      
      if (e.isAnti)
        continue;
      
      Transition t = null;
      if (!e.isCutOff)
        t = (Transition) d2n.get(e);
      else {
        DNode e2 = equiv.get(e);
        if (e2 == null) {
          t = fact.createTransition();
          t.setName(dAS.properNames[e.id]+"_"+e.globalId);
          net.getTransitions().add(t);
          d2n.put(e, t);
        } else {
          t = (Transition)d2n.get(e2);
        }
      }
      
      for (DNode b : e.pre) {
        Place p = (Place)d2n.get(b);
        if (t.getPreSet().contains(p))
          continue;
        
        Arc a = fact.createArcToTransition();
        a.setSource(p);
        a.setTarget(t);
        
        net.getArcs().add(a);
      }
      
      for (DNode b : e.post) {
        Place p = (Place)d2n.get(b);
        if (t.getPostSet().contains(p))
          continue;

        Arc a = fact.createArcToPlace();
        a.setSource(t);
        a.setTarget(p);
        
        net.getArcs().add(a);
      }
    }
    
    Transition[] trans = net.getTransitions().toArray(new Transition[net.getTransitions().size()]);
    for (int i=0; i<trans.length-1; i++) {
      if (trans[i] == null) continue;
      
      for (int j=i+1; j<trans.length; j++) {
        if (trans[j] == null) continue;
        
        String prefix_i = trans[i].getName().substring(0,trans[i].getName().lastIndexOf('_'));
        String prefix_j = trans[j].getName().substring(0,trans[j].getName().lastIndexOf('_'));
        
        if (prefix_i.equals(prefix_j)) {
          if (PNAPI.parallelTransitions(trans[i], trans[j], null)) {
            PNAPI.mergeTransitions(net, trans[i], trans[j]);
            trans[j] = null;
          }
        }
      }
    }

    return net;
  }
  
  /**
   * @param e1
   * @param e2
   * @return
   *    true iff the events e1 and e2 of a {@link DNodeBP} are in direct
   *    conflict
   */
  public static boolean areInConflict(DNode e1, DNode e2) {
    DNode[] preCons = DNode.merge(null, e1.pre, e2.pre);
    // merge preconditions of e1 and e2 and sort them by their IDs
    for (int i=0; i<preCons.length-1; i++) {
      for (int j=i+1; j<preCons.length; j++) {
        if (preCons[i] == preCons[j]) return true;
        // the preconditions are sorted by their IDs,
        // no need to check for duplicates beyond j if the ID changes
        if (preCons[i].id != preCons[j].id) break;
      }
    }
    return false;
  }
  
  /**
   * @param eventSet
   * @return
   *  the number of maximal sub-sets of concurrent events contained in eventSet
   */
  public static int countConfigurations(DNode[] eventSet) {
    
    int configCount = 0;
    
    boolean[] start = new boolean[eventSet.length];
    for (int i=0; i<start.length; i++) start[i] = true;
    
    LinkedList<boolean[]> seen = new LinkedList<boolean[]>();
    LinkedList<boolean[]> queue = new LinkedList<boolean[]>();
    queue.add(start);
    
    while (!queue.isEmpty()) {
      boolean[] selection = queue.remove();
      
      // check whether selection describes a configuration that is sub-set
      // of a configuration we have already seen
      boolean isImplied = false;
      for (boolean[] seenSel : seen) {
        isImplied = true;
        for (int i=0; i<start.length; i++) {
          if (selection[i] && !seenSel[i]) {
            isImplied = false; break; 
          }
        }
        if (isImplied) break;
      }
      // yes, the skip this selection
      if (isImplied) continue;
      
      // print for convenience
      System.out.print("checking [");
      for (int i=0; i<eventSet.length; i++)
        if (selection[i]) System.out.print("1");
        else System.out.print("0");
      System.out.print("] ");
      
      System.out.print("[");
      for (int i=0; i<eventSet.length; i++)
        if (selection[i]) System.out.print(eventSet[i]+", ");
      System.out.print("] ");
      
      // check whether current selection describes a configuration
      // (a conflict-free set of events) by pairwise comparing events for conflict
      boolean isConfig = true;
      for (int i=0; i<eventSet.length-1; i++) {
        if (!selection[i]) continue;
        for (int j=i+1;j<eventSet.length; j++) {
          if (!selection[j]) continue;
          isConfig &= !areInConflict(eventSet[i], eventSet[j]);
          if (!isConfig) break;
        }
        if (!isConfig) break;
      }
      if (isConfig) {
        System.out.println(" is config");
        // remember this selection (i.e. truncate checking all sub-sets of events)
        seen.add(selection);
        // found one configuration
        configCount++;        
      } else {
        System.out.println(" NOT config");
        
        // not a configuration, generate all sub-sets of this set of events
        // that contain one event less

        for (int i_disable=0; i_disable<selection.length; i_disable++) {
          // copy the current bit-vector describing the current selection
          boolean[] nextSelection = new boolean[selection.length];
          for (int j=0; j<nextSelection.length; j++) {
            nextSelection[j] = selection[j];
          }
          // the remove the i-th event from the selection
          if (nextSelection[i_disable]) {
            // the i-th even is still part of the selection
            // remove it
            nextSelection[i_disable] = false;
            // and add the reduced selection to the queue
            queue.addLast(nextSelection);
          }
        } // end: generate sub-sets
      } // end: no config found 
    } // end: while queue not empty
    
    return configCount;
  }
  
  /**
   * Derive input and output logic of the actions that occur in a {@link DNodeBP}
   * in terms of AND-split/AND-join, XOR-split/XOR-join.
   * 
   * The algorithm computes "local token flows" between events by aggregating
   * predecessor/successor relations of actions in the branching process. The
   * ratio between aggregated token flows between action and occurrences of actions
   * tells whether an action always requires all its predecessors/successors
   * (AND-logic) to occur or only one of them (XOR-logic) or mix (OR-logic).
   * 
   * FIXME: the local computations yield inconclusive results
   *  
   * @param bp
   */
  public static void getEventTypes(DNodeBP bp) {
    DNodeSys sys = bp.getSystem();
    short maxEvent = -1;
    for (Short i : sys.nameToID.values()) {
      if (maxEvent < i) maxEvent = i;
    }
    maxEvent++;
    
    Map<DNode, Short> depth = bp.getBranchingProcess().getDepthMap();
    
    short flowCount[][] = new short[maxEvent][maxEvent];
    short eventCount[] = new short[maxEvent];
    short eventCountConfig[] = new short[maxEvent];
    
    // aggregate the flows between occurrences of same actions
    // 1. build the direct successor relation between all events in the BP
    // 2. count all edges between events with the same labels (flowCount)
    // 3. count occurrences of each event (eventCount, eventCountConfig)
    for (DNode e : bp.getBranchingProcess().getAllEvents()) {
      // do not compute successors of cut-off events or anti-events
      if (e.isCutOff || e.isAnti) continue;
      
      HashSet<DNode> ePostEvents = new HashSet<DNode>();
      for (DNode e2 : bp.getBranchingProcess().getAllEvents()) {
        if (e == e2) continue;
        // but allow cut-off events here to build a complete mapping
        // of all successors, do not consider anti-events
        if (e2.isAnti) continue;
        
        if (depth.get(e2) - depth.get(e) == 2) {
          // event e2 could be a direct causal successor of event e
          
          // see if e is a pre-event of one of e2's pre-conditions
          boolean isPred = false;
          for (DNode b : e2.pre) {
            for (int i=0; i<b.pre.length; i++) {
              // e2 is a predecessor of e
              if (b.pre[i] == e) { isPred = true; break; }
            }
            if (isPred) break;
          }
          
          if (isPred) {
            // e is a pre-event of one of e2's pre-conditions and
            // a direct pre-event according to the depth-metric
            ePostEvents.add(e2);
            // this edge counts as direct edge between the actions e.id and e2.id
            flowCount[e.id][e2.id]++;     
            System.out.println(e+"->"+e2);
          }
        }
      }
      
      // approximate how in how many different distributed runs event 'e' can
      // participate by computing all possible configurations (maximal co-sets)
      // of its successor events
      DNode[] eventSet = ePostEvents.toArray(new DNode[ePostEvents.size()]);
      eventCountConfig[e.id] += countConfigurations(eventSet);
      
      eventCount[e.id] += 1;
    }
    // all token flows have been computed
    
    // now derive the logic from the token flows
    for (int i=0; i<maxEvent; i++) {
      if (eventCount[i] > 0) {
        // for each event that occurs
        
        // input logic
        int inTokenSum = 0; // sum of tokens from direct predecessors to event i
        int inTokenAll = 0;
        boolean inTokenEqual = true;
        
        System.out.print("in: ");
        for (int j=0;j<maxEvent;j++) {
          if (flowCount[j][i] > 0) {
            System.out.print(bp.getSystem().properNames[j]+":"+flowCount[j][i]+" | ");
            
            inTokenSum++;
            if (inTokenAll == 0) inTokenAll = flowCount[j][i];
            // do all predecessors provide the same number of tokens to event i?
            else inTokenEqual &= (inTokenAll == flowCount[j][i]);
          }
        }
        if (inTokenEqual && inTokenAll == eventCount[i]) System.out.print(" AND-JOIN");
        else if (inTokenSum == eventCount[i]) System.out.print(" XOR-JOIN");
        else if (inTokenSum > 0) System.out.print(" OR-JOIN");
        System.out.println();
        
        System.out.println(bp.getSystem().properNames[i]+":"+eventCount[i]+"/"+eventCountConfig[i]);

        int outTokenSum = 0; // sum of tokens to direct predecessors from event i
        int outTokenAll = 0;
        boolean outTokenEqual = true;

        System.out.print("out: ");
        for (int j=0;j<maxEvent;j++) {
          if (flowCount[i][j] > 0) {
            System.out.print(bp.getSystem().properNames[j]+":"+flowCount[i][j]+" | ");
            
            outTokenSum++;
            if (outTokenAll == 0) outTokenAll = flowCount[i][j];
            // do all predecessors require the same number of tokens to event i?
            else outTokenEqual &= (outTokenAll == flowCount[i][j]);
          }
        }
        if (outTokenEqual && outTokenAll == eventCount[i]) System.out.print(" AND-SPLIT");
        else if (outTokenSum == eventCount[i]) System.out.print(" XOR-SPLIT");
        else if (outTokenSum > 0) System.out.print(" OR-SPLIT");

        System.out.println();
        System.out.println();

      }
    }

  }
}
