/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved. 
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

package hub.top.uma;

import hub.top.uma.DNode;
import hub.top.uma.DNodeCutGenerator;
import hub.top.uma.DNodeSys_AdaptiveSystem;

import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedList;

public class DNodeBP_Scenario extends DNodeBP {

  public DNodeBP_Scenario(DNodeSys system) {
    super(system);
  }
  
  public LinkedList<DNode> getImpliedScenarios() {
    LinkedList<DNode> implied = new LinkedList<DNode>();
    for (DNode e : bp.getAllEvents()) {
      if (e.isCutOff) continue;
      
      HashSet<Short> postIDs = new HashSet<Short>();
      // collect the IDs of all post-events of e
      if (e.post != null) {
        for (DNode b : e.post) {
          if (b.post != null) {
            for (DNode e2 : b.post) {
              postIDs.add(e2.id);
            }
          }
        }
      }
      
      // check for each transition t that occurs by event e
      // each post-transition t2 of t occurs as post-event of e (by postIDs)
      for (int i=0; i<e.causedBy.length; i++) {
        DNode t = dNodeAS.getTransitionForID(e.causedBy[i]);
        // whether all successors of t are also successors of e
        if (t.post == null || t.post.length == 0) continue;
        for (DNode tPost : t.post) {
          if (tPost.post == null || tPost.post.length == 0) continue;
          DNode t2 = tPost.post[0]; // successor transition of t
          if (!postIDs.contains(t2.id)) {
            // t2 does not occur as post-event of e, e is implied behavior
            implied.add(e);
            e.isImplied = true;
            break;
          }
        }
        if (e.isImplied) break;
      }
    }
    return implied;
  }
  
  
  public LinkedList<DNode> getImpliedScenarios_global() {
    LinkedList<DNode> impliedEvents = new LinkedList<DNode>();
    
    if (dNodeAS instanceof DNodeSys_AdaptiveSystem) {
      DNodeSys_AdaptiveSystem os = (DNodeSys_AdaptiveSystem)dNodeAS;
      LinkedList<DNode[]> histories = os.getHistories();
      
      int eventNum = 0;
      
      for (DNode e : bp.getAllEvents()) {
        eventNum++;
        //if (e.isCutOff) continue;
        
        for (int iCaused=0; iCaused<e.causedBy.length; iCaused++) {
          DNode t = dNodeAS.getTransitionForID(e.causedBy[iCaused]);
          // event e is an occurrence of a transition that has a its
          // preconditions in the history of an oclet
          DNode[] hist = null;
          HashSet<Short> preSetInHist = new HashSet<Short>();
          // check whether transition 't' has its preplaces in a global history of
          // an oclet (FIXME: can be improved, every 't' has exactly on such history or null)
          for (int iHist=0; iHist < histories.size() && hist == null; iHist++) {

            // check if all pre-places of t occur in the global history 'iHist'
            boolean all_pInHist = true;
            for (DNode p : t.pre) {
              boolean p_isInHist = false;
              for (DNode pInHist : histories.get(iHist)){
                if (pInHist == p) { p_isInHist = true; break; }
              }
              if (!p_isInHist) { all_pInHist = false; break; }
            }
            if (!all_pInHist) continue;
            
            // 'iHist' is the global history of transition 't'
            hist = histories.get(iHist);
            // remember the pre-places which alrady occur as pre-conditions of event 'e'
            for (DNode p : t.pre) {
              preSetInHist.add(p.id);
            }
            break;
          }
          if (hist == null) // e not contributed by a minimal transition
            continue;       // of an oclet
          
          // all nodes concurrent to the pre-set of the event 'e'
          HashSet<DNode> uncoveredNodes = new HashSet<DNode>();
          for (DNode b : e.pre) {
            uncoveredNodes.addAll(co.get(b));
          }
          
          // all nodes covered by the histories and their predecessors
          // and successors
          HashSet<DNode> coveredNodes = new HashSet<DNode>();
          
          // check if these preconditions extend to an occurrence of the
          // entire history
          LinkedList<DNode> possibleMatches = new LinkedList<DNode>();          
          for (DNode b : e.pre) {
            if (preSetInHist.contains(b.id)) {
              // already found the matching condition for the place with id == b.id
              possibleMatches.add(b);
            }
          }
            
          // all nodes to search for cuts
          for (DNode max : bp.getAllConditions()) {
            // the pre-conditions of event e are fixed for this cut, ignore
            // all other conditions with the same ID (assuming safe cuts here)
            if (preSetInHist.contains(max.id)) continue;
            // only conditions which are concurrent to the pre-conditions of 'e'
            // can be part of a cut
            if (!co.get(e.pre[0]).contains(max)) continue;
            // add those conditions of the branching process that occur also
            // in the history
            for (int iCut=0; iCut<hist.length; iCut++) {
              if (max.id == hist[iCut].id)
              {
                possibleMatches.add(max);
              }
            }
          }
          // possible matches contains all conditions that may extend to
          // a cut which ends with 'hist'
          
          // TODO: adapt algorithmic solution from DNodeBP#findEnablingCuts() 
          Collections.sort(possibleMatches, DNode.compare);
          
          boolean histNotFound = false;
          // get the cuts
          DNodeCutGenerator cgen = new DNodeCutGenerator(hist, possibleMatches, co);
          while (cgen.hasNext())
          {
            DNode[] cutNodes = cgen.next();

            // see if each precondition of event e is
            // in max BP (current maximal conditions of the current BP)
            int iHist=0; 
            for (; iHist<hist.length; iCaused++) {
              boolean found = false;
              // find a matching condition for hist[iHist] in cutNodes
              for (DNode b : cutNodes)
              {
                if (b.id == hist[iHist].id) {
                  if (b.endsWith(hist[iHist])) {
                    found = true;
                    break;
                  }
                }
              }
              iHist++;
              if (!found) { // no match, cancel
                //System.out.println("no match for "+ e.pre[i]);
                break;
              }
            } // finish searching for matching preconditions in max BP
            if ( iHist == hist.length ) {
              //histFound = true;
              //break;
              
              HashSet<DNode> intersectCoNodes = null;
              // all nodes of the history are covered by it
              for (DNode b : cutNodes) {
                coveredNodes.add(b);
                // construct the intersection of all conditions that are concurrent to
                // the cutNodes, the resulting conditions all occur in a cut that involves
                // the global history of 'e'
                if (intersectCoNodes == null) intersectCoNodes = new HashSet<DNode>(co.get(b));
                else intersectCoNodes.retainAll(co.get(b));
              }
              coveredNodes.addAll(intersectCoNodes);
            }
          } // all occurreces of the history of the oclet of e
          
          // compute all concurrent nodes that are not covered by a history
          uncoveredNodes.removeAll(coveredNodes);

          // cover all forwards reachable nodes
          HashSet<DNode> processed = new HashSet<DNode>();
          LinkedList<DNode> toProcess = new LinkedList<DNode>();
          // to also cover all successors: mimic the firing of branching processes
          // into all uncovered nodes: uncovered nodes should be direct successors of
          // covered histories...
          for (DNode b : coveredNodes) {
            if (b.isEvent || b.post == null) continue;
            for (DNode bPost : b.post) {
              if (!coveredNodes.contains(bPost)) {
                toProcess.add(bPost);
              }
            }
            processed.add(b);
          }
          
          while (!toProcess.isEmpty()) {
            DNode eProcess = toProcess.removeFirst();
            
            boolean preProcessed = true;
            boolean reQueue = true;
            for (DNode b : eProcess.pre) {
              if (!processed.contains(b)) {
                preProcessed = false;
                // one predecessor of the event is not covered, so all successors are
                // also not covered, remove this event
                if (!coveredNodes.contains(b)) reQueue = false;
              }
            }
            if (!preProcessed && reQueue) {
              toProcess.addLast(eProcess);
              continue;
            }
            
            if (eProcess.post == null) continue;
            for (DNode b : eProcess.post) {
              // do not explore beyond unconvered nodes
              if (!uncoveredNodes.contains(b)) continue;
              
              coveredNodes.add(b);
              uncoveredNodes.remove(b);
              processed.add(b);
              if (b.post == null) continue;
              for (DNode eSucc : b.post) {
                toProcess.addLast(eSucc);
              }
            }
          }
          
          // cover all backwards reachable nodes
          LinkedList<DNode> queue = new LinkedList<DNode>();
          for (DNode b : coveredNodes) {
            queue.add(b);
          }

          while (!queue.isEmpty()) {
            DNode coveredNode = queue.removeFirst();
            if (coveredNode.pre == null) continue;
            for (DNode pre : coveredNode.pre) {
              if (coveredNodes.contains(pre)) continue;
              
              uncoveredNodes.remove(pre);
              coveredNodes.add(pre);
              queue.addLast(pre);
            }
          }
          // all predecessors of covered nodes eventually reach covered behavior, i.e.
          // a cut where the global history of 'e' occurs
          
          if (!uncoveredNodes.isEmpty())
            histNotFound = true;
          
          /*
          HashSet<DNode> toCheck = new HashSet<DNode>(uncoveredNodes);
          while (!toCheck.isEmpty())
          {
            DNode yetUncovered = toCheck.iterator().next();
            toCheck.remove(yetUncovered);
            // this node to check has been covered already, skip
            if (!uncoveredNodes.contains(yetUncovered))
              continue;
            
            // search all predecessor of 'yetUnconvered' whether it contains
            // a covered node,
            // - if the first covered node met on this path is a covered event,
            //   then 'yetUncovered' is covered,
            // - if the first covered node met on this path is a covered condition,
            //   then 'yetUncovered' is uncovered
            // - if no covered node is met, then the search reaches the minimal nodes of
            //   the prefix, if there is covered concurrent condition, then 'yetUncovered'
            //   is covered
            queue.add(yetUncovered);
            HashSet<DNode> visitedNodes = new HashSet<DNode>();
            while (!queue.isEmpty()) {
              DNode uncoveredPred = queue.removeFirst();
              // collect all nodes visited on the descent, these node are covered
              // iff 'yetUncovered' is covered
              visitedNodes.add(uncoveredPred);
              
              if (uncoveredPred.pre == null || uncoveredPred.pre.length == 0) {
                // an uncovered node without predecessor
                boolean coveredByCo = false;
                // it is covered if it has a concurrent node that is covered
                // because it cannot be in conflict to any of the covered cuts
                for (DNode coNode : co.get(uncoveredPred)) {
                  if (coveredNodes.contains(coNode)) {
                    coveredNodes.addAll(visitedNodes);
                    uncoveredNodes.removeAll(visitedNodes);
                    coveredByCo = true;
                    break;
                  }
                }
                if (!coveredByCo) {
                  histNotFound = true;
                  break;
                }
              } else {
                
                // an uncovered node with predecessor
                for (DNode pre : uncoveredPred.pre) {
                  if (coveredNodes.contains(pre)) {
                    if (pre.isEvent) {
                      // the predecessor if this node is a covered event
                      // so this condition is concurrent to a condition
                      // which occurs in a cut where the global history of
                      // 'e' occurs, we're done
                      coveredNodes.addAll(visitedNodes);
                      uncoveredNodes.removeAll(visitedNodes);
                      break; // the inner loop for predecessors of 'uncovered'
                    
                    } else if (!coveredNodes.contains(uncoveredPred)) {
                      // the predecessor of this node is a covered condition
                      // but this node is uncovered or leads to an uncovered
                      // node, so this node (and its successors) do not reach
                      // a cut where the global history of 'e' occurs
                      histNotFound = true;
                      break;
                    }
                  } else {
                    // visit this predecessor, because we don't know
                    // whether it is covered or not
                    if (!queue.contains(pre))
                      queue.addLast(pre);
                  }
                } // all predecessors of 'uncovered' node
              }
            }
          }
          */
          if (histNotFound) {
            System.out.println("event "+e+" is implied");
            System.out.println("uncovered conditions: "+uncoveredNodes);
            System.out.println("covered nodes: "+coveredNodes);
            e.isImplied = true;
            impliedEvents.add(e);
          }
        }
      } // check event 'e' for being implied
    }
    return impliedEvents;
  }


}
