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

import hub.top.editor.ptnetLoLA.ArcToPlace;
import hub.top.editor.ptnetLoLA.ArcToTransition;
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

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Synthesize an AdaptiveSystem into an unlabeled Petri net with the
 * same behavior (if this is possible).
 * 
 * TODO: in case of non-equivalent synthesis: show error message to UI
 * TODO: in case of non-equivalent synthesis: refine equivalence relation on places
 * 
 * @author Dirk Fahland
 */
public class NetSynthesisLocal {

  private DNodeBP                 bp;
  private DNodeSet                bps;
  private DNodeSys                system;
  private TransitiveDependencies  dependencies;
  
  public NetSynthesisLocal(DNodeBP bp) {
    this.bp = bp;
    this.bps = bp.getBranchingProcess();
    this.system = bp.getSystem();
    this.dependencies = new TransitiveDependencies(system);
  }

  /**
   * TODO: document and refactor into readable methods
   * 
   * @param bp
   * @return
   */
  public PtNet foldToPetriNet()
  {
    // extend branching process with transitive dependencies
    dependencies.extendBranchingProcess(bp);
    
    Map<Short, Set<DNode>> eq = getBasicEquivalence();
    
    HashSet<DNode> impliedConditions_preserve = new HashSet<DNode>();
    
    // identify all critical pairs of equivalent conditions and check whether
    // they are distinguishable by other conditions of the branching process
    
    for (Short id : eq.keySet()) {
      // get all conditions of the equivalence class 'id'
      DNode[] b_equiv = new DNode[eq.get(id).size()];
      b_equiv = eq.get(id).toArray(b_equiv);
      
      // two equivalent conditions are critical if their post-sets are not isomorphic
      // compare any two conditions for non-isomorphic post-sets
      for (int i=0; i<b_equiv.length-1; i++) {
        // a cut-off condition always has isomorphic post-sets wrt.
        // its equivalent conditions by construction
        if (b_equiv[i].isCutOff) continue;
        
        for (int j=i+1; j<b_equiv.length; j++) {
          // a cut-off condition always has isomorphic post-sets wrt.
          // its equivalent conditions by construction
          if (b_equiv[j].isCutOff) continue;
          
          // The two conditions b_equiv[i] and b_equiv[j] may have
          // non-isomorphic post-sets. Get all post-events that are not included
          // in both post-sets
          List<DNode> diff = equivalent(b_equiv[i].post, b_equiv[j].post);
          if (diff == null || diff.size() == 0) continue;
          //System.out.println("("+b_equiv[i]+","+b_equiv[j]+") is critical "+diff);
          
          // now check for each post-event 'd' that causes b_equiv[i] and b_equiv[j]
          // to be critical whether 'd' depends on a condition 'pre_d' by which we can
          // distinguish whether 'd' consumes from b_equiv[i] or from b_equiv[j].
          
          // this condition 'pre_d' is concurrent to, say, b_equiv[i] and must not
          // have an isomorphic counterpart at b_equiv[j], i.e. no post-event of
          // b_equiv[j] may depend on a condition with the same ID as 'pre_d'.
          // Correspondingly if 'pre:d' is concurrent to b_equiv[j].
          
          // compute pre_i = pre(post(i)) and pre_j = pre(post(j))
          HashSet<DNode> pre_i = new HashSet<DNode>();
          for (DNode post_i : b_equiv[i].post) {
            pre_i.addAll(Arrays.asList(post_i.pre));
          }
          HashSet<DNode> pre_j = new HashSet<DNode>();
          for (DNode post_j : b_equiv[j].post) {
            pre_j.addAll(Arrays.asList(post_j.pre));
          }
          
          // now check whether 'd' has a predecessor 'd_pre' that distinguishes i and j
          for (DNode d : diff) {
            //System.out.println("mismatch on "+d);

            boolean canDistinguish = false;
            for (DNode pre_d : d.pre) {
              
              // check whether 'pre_d' occurs in pre(post(i))
              boolean found_in_pre_i = false;
              for (DNode d_pre_i : pre_i) {
                if (eq.get(d_pre_i.id) != null && eq.get(d_pre_i.id).contains(pre_d)) {
                  found_in_pre_i = true; break;
                }
              }
              //if (!found_in_pre_i) System.out.println("distinguishable by "+pre_d);
              
              // check whether 'pre_d' occurs in pre(post(j))
              boolean found_in_pre_j = false;
              for (DNode d_pre_j : pre_j) {
                if (eq.get(d_pre_j.id) != null && eq.get(d_pre_j.id).contains(pre_d)) {
                  found_in_pre_j = true; break;
                }
              }
              //if (!found_in_pre_j) System.out.println("distinguishable by "+pre_d);
              
              if (!found_in_pre_i || !found_in_pre_j) {
                // 'pre_d' occurs only in one of pre(post(i)) or pre(post(j)),
                // thus 'pre_d' distinguishes the critical equivalent conditions i and j
                
                // if 'pre_d' is an implied condition added as transitive dependency,
                // then we have to include 'pre_d' in the synthesis
                if (dependencies.impliedConditions.contains(pre_d)) {
                  impliedConditions_preserve.add(pre_d);
                }
                canDistinguish = true;
              }
            }
            if (!canDistinguish) {
              System.out.println("Error. Cannot synthesize an equivalent Petri net:");
              System.out.println("The occurrence of "+d+" is introduces new behavior");
              System.out.println("at ("+b_equiv[i]+","+b_equiv[j]+")");
            }
          } // check for predecessors of 'd'
        }
      } // for each pair of conditions: check critical
    } // for all equivalence classes
    
    return generateNet(eq, impliedConditions_preserve);
  }

  /**
   * Generate the basic equivalence relation on all nodes of the branching process.
   * This equivalence relation puts all standard conditions of the branching process
   * with the same ID into one equivalence class. All other nodes remain singleton.
   * 
   * Especially, each node that was added as a transitive dependency by
   * {@link TransitiveDependencies#extendBranchingProcess(DNodeBP)} remains
   * singleton. 
   * 
   * @return
   */
  private Map<Short, Set<DNode>> getBasicEquivalence() {
    Map<Short, Set<DNode>> eq = new HashMap<Short, Set<DNode>>();
    for (DNode b : bps.allConditions) {
      Set<DNode> b_equiv = eq.get(b.id);
      if (b_equiv == null) {
        b_equiv = new HashSet<DNode>();
        eq.put(b.id, b_equiv);
      }
      b_equiv.add(b);
    }
    return eq;
  }
  
  private PtNet generateNet(Map<Short, Set<DNode>> eq, Set<DNode> impliedConditions_preserve)
  {
    HashMap<DNode, Node> transMap = new HashMap<DNode, Node>(); 
    PtnetLoLAFactory f = PtnetLoLAPackage.eINSTANCE.getPtnetLoLAFactory();
    PtNet net = f.createPtNet();
    for (Short id : eq.keySet()) {
      Place p = f.createPlace();
      p.setName(system.properNames[id]);
      for (DNode d : eq.get(id)) {
        transMap.put(d, p);
        if (bps.initialConditions.contains(d)) {
          p.setToken(p.getToken()+1);
        }
      }
      net.getPlaces().add(p);
    }

    HashSet<Place> addedPlaces = new HashSet<Place>();
    int tauId = 0;
    for (DNode d : impliedConditions_preserve) {
      Place p = f.createPlace();
      p.setName("p"+tauId);
      transMap.put(d, p);
      net.getPlaces().add(p);
      addedPlaces.add(p);
    }

    for (DNode d : bps.allEvents) {
      if (d.isAnti) continue;
      
      Transition t = f.createTransition();
      t.setName(system.properNames[d.id]);
      net.getTransitions().add(t);
      
      for (DNode pre : d.pre) {
        Place p = (Place)transMap.get(pre);
        if (p == null) continue;  // shall not be synthesized: no arc
        
        ArcToTransition a = f.createArcToTransition();
        a.setSource(p); a.setTarget(t);
        net.getArcs().add(a);
      }
      
      for (DNode post : d.post) {
        Place p = (Place)transMap.get(post);
        if (p == null) continue;  // shall not be synthesized: no arc
        
        ArcToPlace a = f.createArcToPlace();
        a.setSource(t); a.setTarget(p);
        net.getArcs().add(a);
      }
    }
    
    Transition[] trans = net.getTransitions().toArray(new Transition[net.getTransitions().size()]);
    for (int i=0; i<trans.length-1; i++) {
      if (trans[i] == null) continue;
      
      for (int j=i+1; j<trans.length; j++) {
        if (trans[j] == null) continue;
        
        if (trans[i].getName().equals(trans[j].getName())) {
          if (PNAPI.parallelTransitions(trans[i], trans[j], addedPlaces)) {
            PNAPI.mergeTransitions(net, trans[i], trans[j]);
            trans[j] = null;
          }
        }
      }
    }
    
    return net;
  }
  
  /**
   * Check whether the two sets of {@link DNode}s represent the same set of
   * transitions/places of the system, i.e. if their entry carry pairwise
   * the same labels. All {@link DNode}s that occur only in one of the sets
   * are returned.
   * 
   * @param set1
   * @param set2
   * @return a list of all {@link DNode}s occuring only in one of the sets
   */
  private List<DNode> equivalent(DNode[] set1, DNode[] set2) {
    // assume: set1 and set2 are ordered by their IDs
    if (set1 == null && set2 == null) return null;
    if (set1 == null) return Arrays.asList(set2);
    if (set2 == null) return Arrays.asList(set1);
    
    List<DNode> diff = new LinkedList<DNode>();
    
    int i1 = 0, i2 = 0;
    while (i1 < set1.length && i2 < set2.length) {
      if (set1[i1].id == set2[i2].id) {
        i1++;
        i2++;
      } else {
        if (set1[i1].id < set2[i2].id) {
          diff.add(set1[i1]);
          i1++;
        } else {
          diff.add(set2[i2]);
          i2++;
        }
      }
    }
    while (i1 < set1.length) diff.add(set1[i1++]);
    while (i2 < set2.length) diff.add(set2[i2++]);
    return diff;
  }
}
