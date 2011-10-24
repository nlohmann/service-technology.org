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

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Options;
import hub.top.uma.DNodeSet.DNodeSetElement;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * Synthesize a (possibly labeled) Petri net by folding the branching process
 * along its cut-off equivalence relation.
 * 
 * @author Dirk Fahland
 */
public class NetSynthesis {
  
  private DNodeRefold bp;
  
  /**
   *  map that assigns each DNode to its synthesize net counter-part
   *  built up during net construction
   */
  public HashMap<DNode, Node> d2n;
  
  /**
   * reverse of {@link #d2n}, mapping each node to the canonical
   * representative of the original folding equivalence class of
   * the net
   */
  public HashMap<Node, DNode> n2d;

  public NetSynthesis(DNodeRefold bp) {
    this.bp = bp;
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * Assumes the folding equivalence has not been computed yet and
   * computes it on the fly.
   * 
   * @return
   */
  public PetriNet foldToNet_labeled() {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return this.foldToNet_labeled(showNodes, true);
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param computeFoldingEquivalence set to <code>true</code> if the
   * folding equivalence still has to be computed
   * 
   * @return
   */
  public PetriNet foldToNet_labeled(boolean computeFoldingEquivalence) {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return this.foldToNet_labeled(showNodes, computeFoldingEquivalence);
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param nodesToShow generate Petri net only from the nodes in this set 
   *                    if <code>null</code>, then all nodes will be used
   * @param computeFoldingEquivalence set to <code>true</code> if the
   *                    folding equivalence still has to be computed
   * @return
   */
  public PetriNet foldToNet_labeled(DNodeSetElement fromNodes, boolean computeFoldingEquivalence) {
    
    DNodeSys dAS = bp.getSystem();

    if (computeFoldingEquivalence) {
      // we still have to compute the folding equivalence for the synthesis
      bp.equivalentNode();
      bp.extendFutureEquivalence_maximal();
      while (bp.extendFutureEquivalence_backwards());
      //bp.debug_printFoldingEquivalence();
      
      bp.relaxFutureEquivalence();
      
      //bp.debug_printFoldingEquivalence();
    }
    
    // maps each DNode to its least equivalent DNode (to itself it is the least)
    Map<DNode, DNode> equiv = bp.equivalentNode();
    // map that assigns each DNode to its synthesize net counter-part
    // built up during net construction
    d2n = new HashMap<DNode, Node>();
    n2d = new HashMap<Node, DNode>();
    
    PetriNet net = new PetriNet(); 
    
    // First generate all conditions, beginning with conditions
    // that have no equivalent counterpart
    for (DNode b: fromNodes) {
      if (b.isEvent) continue;  
      // conditions only
      
      if (equiv.get(b) == null || equiv.get(b) == b)
      {
        Place p = net.addPlace(dAS.properNames[b.id]);
        
        if (b.pre == null || b.pre.length == 0) {
          p.setTokens(1);
        }
        d2n.put(b, p);
        n2d.put(p, b);
      }
    }
    
    // now map each condition that has an equivalent counterpart to
    // the place that represents this counterpart
    for (DNode b: fromNodes) {
      if (b.isEvent) continue;  
      // conditions only

      if (equiv.get(b) != null && equiv.get(b) != b && !b.isAnti) {

        // follow down the chain of equivalent conditions until
        // reaching the least condition in that chain
        DNode bOther = equiv.get(b);
        while (equiv.get(bOther) != null && equiv.get(bOther) != bOther)
          bOther =  equiv.get(bOther);
        
        if (b.pre == null || b.pre.length == 0) {
          ((Place)d2n.get(bOther)).setTokens(1);
        }
        
        d2n.put(b, d2n.get(bOther));
      }
    }
    
    // Then generate all events, again beginning with events that
    // have no equivalent counter part
    for (DNode e : fromNodes) {
      if (!e.isEvent) continue;  
      // events only
      
      if (equiv.get(e) == e) {
        String label = "";
        if (e.isAnti && !e.isHot)
          label = "tau_"+e.globalId;   // hide cold anti-events
        else
          label = dAS.properNames[e.id];
        Transition t = net.addTransition(label);
        d2n.put(e, t);
        n2d.put(t, e);
        //System.out.println(e + " --first--> "+t);
      }
    }

    // now map each event that has an equivalent counterpart to
    // the transition that represents this counterpart and
    // generate the arcs to the corresponding places
    for (DNode e : fromNodes) {
      if (!e.isEvent) continue;  
      // events only
      
      Transition t = null;
      if (equiv.get(e) == e) {
        // get transition if already generated
        t = (Transition) d2n.get(e);
      } else {
        // and map to existing transition if available
        DNode e2 = equiv.get(e);
        if (e2 == null) {
          String label = "";
          if (e.isAnti && !e.isHot)
            label = "tau_"+e.globalId;   // hide cold anti-events
          else
            label = dAS.properNames[e.id];
          t = net.addTransition(label);
          d2n.put(e, t);
          n2d.put(t, e);
          
          //System.out.println(e + " --second--> "+t);
        } else {
          
          // follow down the chain of equivalent events until
          // reaching the least event in that chain
          while (equiv.get(e2) != null && equiv.get(e2) != e2)
            e2 = equiv.get(e2);
          
          t = (Transition)d2n.get(e2);
          d2n.put(e, t);
          //System.out.println(e + " --recall--> "+t);
        }
      }
      
      // create arcs
      for (DNode b : e.pre) {
        Place p = (Place)d2n.get(b);
        if (p == null) {
          System.out.println("transition "+t+" has no pre-place for "+b);
          continue;
        }
        if (t.getPreSet().contains(p))
          continue;
        net.addArc(p, t);
      }
      for (DNode b : e.post) {
        Place p = (Place)d2n.get(b);
        if (p == null) {
          System.out.println("transition "+t+" has no post-place for "+b);
          continue;
        }
        if (t.getPostSet().contains(p))
          continue;
        net.addArc(t, p);
      }
    }
    
    // because of the folding, we may generate multiple parallel transitions
    // having the same label, same pre-set and same post-set, identify these
    // and merge them
    net.removeParallelTransitions();
    net.removeIsolatedNodes();
    
    for (Transition s1 : net.getTransitions()) {
      for (Transition s2 : net.getTransitions()) {
        if (net.nondeterministicTransitions(s1,s2)) {
          System.out.println("non-deterministic transitions "+s1+" ("+n2d.get(s1)+") and "+s2+" ("+n2d.get(s2)+")");
          System.out.print("  "+s1.getPreSet()+": ");
            for (Place p : s1.getPreSet()) System.out.print(n2d.get(p)+", ");
            System.out.print("\n");
          System.out.print("  "+s2.getPreSet()+": ");
            for (Place p : s2.getPreSet()) System.out.print(n2d.get(p)+", ");
            System.out.print("\n");

        }
      }
    }
    
    /*
    for (Transition s1 : net.getTransitions()) {
      System.out.print(s1+" -->");
      for (DNode e: fromNodes) {
        if (!e.isEvent) continue;
        DNode e2 = e;
        while (equiv.get(e) != null && equiv.get(e) != e)
          e = equiv.get(e);
        if (n2d.get(s1) == e) {
          System.out.print(e2+", ");
        }
      }
      System.out.println();
    }
    */
    
    return net;
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param bp
   * @param nodesToShow generate Petri net only from the nodes in this set 
   *                    if <code>null</code>, then all nodes will be used
   * @param computeFoldingEquivalence set to <code>true</code> if the
   *                    folding equivalence still has to be computed
   * @return
   */
  public static PetriNet foldToNet_labeled(DNodeRefold bp, DNodeSetElement fromNodes, boolean computeFoldingEquivalence) {
   NetSynthesis synth = new NetSynthesis(bp);
   return synth.foldToNet_labeled(fromNodes, computeFoldingEquivalence);
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param bp
   * @param computeFoldingEquivalence set to <code>true</code> if the
   *                    folding equivalence still has to be computed
   * @return
   */
  public static PetriNet foldToNet_labeled(DNodeRefold bp, boolean computeFoldingEquivalence) {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return foldToNet_labeled(bp, showNodes, computeFoldingEquivalence);
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * Assumes the folding equivalence has not been computed yet and
   * computes it on the fly.
   * 
   * @param bp
   * @return
   */
  public static PetriNet foldToNet_labeled(DNodeRefold bp) {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return foldToNet_labeled(bp, showNodes, true);
  }
  
  public static PetriNet foldToNet_byLabel(DNodeBP bp) {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return foldToNet_byLabel(bp, showNodes);
  }

  
  public static PetriNet foldToNet_byLabel(DNodeBP bp, DNodeSetElement fromNodes) {

    DNodeSys dAS = bp.getSystem();
    
    HashMap<Short, Node> d2n = new HashMap<Short, Node>();
    
    PetriNet net = new PetriNet(); 
    
    // first generate all places
    for (DNode b: fromNodes) {
      if (b.isEvent || b.isAnti) continue;  
      // conditions only
      
      if (!d2n.containsKey(b.id)) {
        Place p = net.addPlace(dAS.properNames[b.id]);
        
        if (b.pre == null || b.pre.length == 0) {
          p.setTokens(1);
        }
        d2n.put(b.id, p);
      }
    }
    
    // then generate all transitions
    for (DNode e : fromNodes) {
      if (!e.isEvent) continue;  
      // events only
      
      if (!d2n.containsKey(e.id)) {
        Transition t = net.addTransition(dAS.properNames[e.id]);
        d2n.put(e.id, t);
      }
      
      Transition t = (Transition)d2n.get(e.id);
      // create arcs
      for (DNode b : e.pre) {
        Place p = (Place)d2n.get(b.id);
        if (p == null) {
          System.out.println("transition "+t+" has no pre-place for "+b);
          continue;
        }
        if (t.getPreSet().contains(p))
          continue;
        net.addArc(p, t);
      }
      for (DNode b : e.post) {
        Place p = (Place)d2n.get(b.id);
        if (p == null) {
          System.out.println("transition "+t+" has no post-place for "+b);
          continue;
        }
        if (t.getPostSet().contains(p))
          continue;
        net.addArc(t, p);
      }
    }
    
    return net;
  }
  
  /**
   * @param bp
   * @return all nodes of <code>bp</code> that are not {@link DNode#isHot} and {@link DNode#isAnti}
   */
  public static DNodeSetElement getAllNodes_notHotAnti(DNodeBP bp) {

    DNodeSetElement allNodes = new DNodeSetElement();
    
    for (DNode n : bp.getBranchingProcess().getAllNodes()) {
      if (n.isHot && n.isAnti) continue;
      allNodes.add(n);
    }
    
    return allNodes;
  }
  
  /**
   * Output a branching process as a Petri net model {@link Petrinet} for
   * viewing and manipulating in a graphical editor.
   * 
   * @param bp
   * @param nodesToShow generate Petri net only from the nodes in this set 
   *                    if <code>null</code>, then the complete branching
   *                    process will be returned
   * @param annotate    annotate nodes names with additional information, e.g.
   *                    node is a cut-off event etc.
   * @return
   *  a Petrinet representing the branching process bp
   */
  public static PetriNet convertToPetriNet(DNodeBP bp, DNodeSetElement nodesToShow, boolean annotate) {
    PetriNet net = new PetriNet();

    if (nodesToShow == null)
      nodesToShow = bp.getBranchingProcess().getAllNodes();
    
    HashMap<Integer, Node> nodeMap = new HashMap<Integer, Node>();
    
    // first print all conditions
    for (DNode n : nodesToShow) {
      if (n.isEvent)
        continue;

      String name;
      if (annotate) {
        name = n.toString();
        if (n.isAnti) name = "NOT "+name;
        else if (n.isCutOff) name = "CUT("+name+")";
      } else {
        name = bp.getSystem().properNames[n.id];
      }
        
      Place p = net.addPlace(name); 
      nodeMap.put(n.globalId, p);
      
      if (bp.getBranchingProcess().initialConditions.contains(n))
        p.setTokens(1);
    }

    for (DNode n : nodesToShow) {
      if (!n.isEvent)
        continue;
      
      String name;
      if (annotate) {
        name = n.toString();
        if (n.isAnti) name = "NOT "+name;
        else if (n.isCutOff) name = "CUT("+name+")";
      } else {
        name = bp.getSystem().properNames[n.id];
      }
      
      Transition t = net.addTransition(name);
      nodeMap.put(n.globalId, t);
    }
    
    System.out.println(nodeMap);
    
    for (DNode n : nodesToShow) {
      if (n.isEvent) {
        for (DNode pre : n.pre) {
          net.addArc(
              (Place)nodeMap.get(pre.globalId),
              (Transition)nodeMap.get(n.globalId)
             );
        }
      } else {
        for (DNode pre : n.pre) {
            net.addArc(
                (Transition)nodeMap.get(pre.globalId),
                (Place)nodeMap.get(n.globalId)
               );
        }
      }
    }
    return net;
  }
  
  /**
   * Record of results of {@link NetSynthesis#doesImplement(PetriNet, DNodeBP)} storing
   * the result and diagnostic information.
   *  
   * @author fahland
   */
  public static class Diagnostic_Implements {
    /**
     * result produced by {@link NetSynthesis#compareBehavior(DNodeBP, DNodeBP)}
     */
    public int       result;
    /**
     * behavior of the original (oclet) system
     */
    public DNodeBP   dbp_oclets;
    /**
     * behavior of the second (Petri net) system to check for implementation
     */
    public DNodeBP   dbp_net;
  }
  
  /**
   * Check whether the Petri net 'net2' exhibits the behavior described by
   * 'dbp' (the branching process of an oclet system). 
   * 
   * @param net2
   * @param dbp
   * @return
   * @throws InvalidModelException
   */
  public static Diagnostic_Implements doesImplement(PetriNet net2, DNodeBP dbp) throws InvalidModelException {
    net2.turnIntoUnlabeledNet();
    DNodeSys_PetriNet sys2 = new DNodeSys_PetriNet(net2);
    
    Options o = new Options(sys2);
    o.configure_buildOnly();
    o.configure_PetriNet();
    
    DNodeBP dbp2 = new DNodeBP(sys2, o);
    
    while ((dbp2.step() > 0));

    dbp2.futureEquivalence();

    int result = hub.top.uma.synthesis.NetSynthesis.compareBehavior(dbp, dbp2);
    switch (result) {
      case hub.top.uma.synthesis.NetSynthesis.COMPARE_EQUAL:
        System.out.println("both systems have equal behavior");
        break;
      case hub.top.uma.synthesis.NetSynthesis.COMPARE_LESS:
        System.out.println("the oclet system has less behavior than the Petri net system");
        break;
      case hub.top.uma.synthesis.NetSynthesis.COMPARE_MORE:
        System.out.println("the oclet system has more behavior than the Petri net system");
        break;
      case hub.top.uma.synthesis.NetSynthesis.COMPARE_INCONCLUSIVE:
        System.out.println("both systems have different behavior");
        break;
      case hub.top.uma.synthesis.NetSynthesis.COMPARE_NOT_INITIAL:
        System.out.println("the initial states don't match");
        break;
    }
    
    Diagnostic_Implements diag = new Diagnostic_Implements();
    diag.result = result;
    diag.dbp_oclets = dbp;
    diag.dbp_net = dbp2;
    return diag;
  }
  
  public static final int COMPARE_EQUAL = 0;
  public static final int COMPARE_LESS = -1;
  public static final int COMPARE_MORE = 1;
  public static final int COMPARE_INCONCLUSIVE = 2;
  public static final int COMPARE_NOT_INITIAL = 3;
  
  /**
   * Compares the behavior of two systems by their branching processes. Requires
   * that both systems have computed the {@link DNodeBP#foldingEquivalenceClass}es. 
   * 
   * @param dbp
   * @param dbp2
   * @return <code>0</code> iff both systems have the same behavior. See other
   * <code>COMPARE_</code> values for other possible results.
   */
  public static int compareBehavior(DNodeBP dbp, DNodeBP dbp2) {
    DNodeSys sys = dbp.getSystem();
    DNodeSys sys2 = dbp2.getSystem();
    DNodeSet bp = dbp.getBranchingProcess();
    DNodeSet bp2 = dbp2.getBranchingProcess();
    
    HashMap<DNode, DNode> matchMap = new HashMap<DNode, DNode>();
    HashSet<DNode> toMatch = new HashSet<DNode>(bp.getAllNodes());
    HashSet<DNode> toMatch2 = new HashSet<DNode>();
    
    // compare initial states of both systems
    int matchCount = 0;
    for (DNode d2 : bp2.initialConditions) {
      boolean match = false;  // whether 'd2' has a matching initial node 'd' of sys
      
      // we cannot compare nodes of 'sys' and 'sys2' by their IDs, but only by their
      // full label strings, additionally, the 'sys2' has split labels of the form
      // 'XXXXXXX_id', to compare labels of 'd2' and a node 'd', cut off the last part '_id'
      String d2_label = sys2.properNames[d2.id].substring(0, sys2.properNames[d2.id].lastIndexOf('_'));
      for (DNode d : bp.initialConditions) {
        if (d2_label.equals(sys.properNames[d.id])) {
          //System.out.println(d2 + " matches "+d.globalId);
          matchMap.put(d2, d);  // remember the matching 'd2' -> 'd'
          toMatch.remove(d);
          if (d2.post != null)  // and add all successors of 'd2' to extend the matching in the next step
            for (DNode d2post : d2.post)
              toMatch2.add(d2post);
          
          match = true;
          break;
        }
      }
      if (!match) {
        //System.out.println(d2.toString(sys2) + " has no match");
      } else {
        matchCount++;
      }
    }
    
    if (matchCount != bp.initialConditions.size()) {
      System.out.println("initial nodes do not match");
      return COMPARE_NOT_INITIAL;
    } else {
      System.out.println("initial states match");
      // compare the remainder of the behavior by a breadth-first search
      
      while (!toMatch2.isEmpty()) {
        // pick a node 'd2' of 'sys2' that has not been matched and whose predecessors are all matched
        DNode d2 = null;
        for (DNode d2prime : toMatch2) {
          boolean allPreMatched = true;
          for (DNode d2pre : d2prime.pre)
            if (!matchMap.containsKey(d2pre)) allPreMatched = false;
          if (allPreMatched) {
            d2 = d2prime;
            break;
          }
        }
        if (d2 == null) {
          //System.out.println("nothing to match. abort");
          break;
        }

        // and find all nodes 'd' of 'sys' that match 'd2'
        
        // we cannot compare nodes of 'sys' and 'sys2' by their IDs, but only by their
        // full label strings, additionally, the 'sys2' has split labels of the form
        // 'XXXXXXX_id', to compare labels of 'd2' and a node 'd', cut off the last part '_id'
        String d2_label = sys2.properNames[d2.id].substring(0, sys2.properNames[d2.id].lastIndexOf('_'));

        // we find matching nodes 'd' by considering the predecessors 'd2Pre' of 'd2',
        // and exploring the successors all nodes of 'sys' that match 'd2Pre'
        boolean match = false;
        DNode[] d2Pre = d2.pre;
        // the nodes of 'sys' that match 'd2Pre'
        HashSet<DNode>[] dPre = new HashSet[d2Pre.length];
        // all successors of 'dPre' that may match 'd2'
        HashSet<DNode> dCand = new HashSet<DNode>();
        
        // identify the 'b' nodes of 'sys' that match 'b2', for each predecessor 'b2' of 'd2'   
        for (int i=0;i<d2Pre.length;i++) {
          // a matching node 'b' that matches 'b2' = d2Pre[i]
          DNode b = matchMap.get(d2Pre[i]);
          // and all nodes of 'sys' that are equivalent to 'b'
          Set<DNode> dPre_i = dbp.futureEquivalence().get(dbp.equivalentNode().get(b)); 
          
          // and collect all successors of 'b' (and its equivalent nodes,
          // these successors are possible nodes 'd' of 'sys' to match 'd2'
          for (DNode bPrime : dPre_i) {
            if (bPrime.post == null) continue;
            for (DNode d : bPrime.post) {
              // successor 'd' is of the same type as 'd2' and not an anti-node
              if (d2_label.equals(sys.properNames[d.id])
                  && d2.pre.length == d.pre.length
                  && !d.isAnti)
              {
                dCand.add(d);
              }
            }
          }
          
          // We sort all nodes of 'sys', that match the predecessors of 'd2' by their
          // IDs in 'sys'. This way, we can quickly compare, whether a candidate node
          // 'd' in 'dCand' has only predecessors 'b' that match a predecessor 'b2' of 'd2'
          insertSetSorted(dPre, 0, i, dPre_i);
        }
       
        // now check for each candidate node 'd', whether its pre-set is equivalent
        // to the matching nodes of the pre-set of 'd2'
        HashSet<DNode> dCandMatch = new HashSet<DNode>();
        for (DNode d : dCand) {
          boolean allPreEquivalent = true;
          for (int i=0;i<d2Pre.length;i++) {
            if (!dPre[i].contains(d.pre[i])) {
              // this predecessor 'i' of 'd' does not match the corresponding predecessor of 'd2'
              // and 'i' is not equivalent to a matching predecessor of 'd2', so 'd' cannot
              // match 'd2'
              allPreEquivalent = false;
              break;
            }
          }
          if (allPreEquivalent) {
            // all predecessors of 'd' either match all predecessors of 'd2', or are
            // equivalent to matching nodes, 'd' matches 'd2'
            dCandMatch.add(d);
          }
        }
        // 'd2' matches with each node in 'dCandMatch'
        
        if (dCandMatch.isEmpty()) {
          match = false;
        } else {
          match = true;

          // remember the matching, represent 'd2' -> 'dCandMatch' by one node 'd' explicitly
          DNode d = dCandMatch.iterator().next();
          matchMap.put(d2, d);
          //System.out.println(d2.toString(sys2) + " matches "+d.toString(sys));
          
          // and remember that each node in 'dCandMatch' was matched 
          toMatch.removeAll(dCandMatch);
          toMatch2.remove(d2);
          // explore matching for all successors of 'd2'
          if (d2.post != null)
            for (DNode d2post : d2.post) {
              // skip anti-nodes in matching
              if (d2post.isAnti && d2post.isHot) continue;
              toMatch2.add(d2post);
            }

        }
        
        if (!match) {
          //System.out.println("------------------> "+d2.toString(sys2) + " has no match");
          toMatch2.remove(d2);
          continue;
        }
      }
    }
    // finished matching all nodes, evaluate the result
    
    if (toMatch.isEmpty()) {
      System.out.println("all original nodes matched");
      return COMPARE_EQUAL;
    }
    
    // some nodes may be unmatched, but these might be anti-nodes, which
    // don't have to be matched
    boolean allUnmatchedAnti = true;
    for (DNode d : toMatch) {
      if (!d.isAnti) {
        allUnmatchedAnti = false;
        break;
      }
    }
    
    if (allUnmatchedAnti) {
      System.out.println("only anti-nodes unmatched");
      return COMPARE_EQUAL;
    }
    
    // there are some unmatched nodes, determine all unmatched nodes
        
    int unmatched1 = 0;
    int unmatched2 = 0;

    System.out.println("unmatched nodes oclet system: ");
    for (DNode d : bp.getAllNodes()) {
      if (d.isAnti) {
        // we only check matching of positive nodes
        continue;
      }
      
      if (!matchMap.containsValue(d)) {
        if (!d.isEvent) continue;
        
        // 'd' was not matched, so check whether an equivalent node was matched
        boolean matched = false;
        for (DNode dPrime : dbp.futureEquivalence().get(dbp.equivalentNode().get(d))) {
          if (matchMap.containsValue(dPrime)) {
            matched = true;
            break;
          }
        }
        if (matched) continue;
        
        System.out.println(d.toString(sys));
        d.isImplied = true;
        unmatched1++;
      }
    }

    System.out.println("\n\nunmatched nodes Petri net system: ");
    for (DNode d2 : bp2.getAllNodes()) {
      if (d2.isAnti && d2.isHot) {
        // we only check matching of positive nodes
        continue;
      }
      if (!matchMap.containsKey(d2)) {
        if (!d2.isEvent) continue;
        System.out.println(d2.toString(sys2));
        d2.isImplied = true;
        unmatched2++;
      }
    }
    
    // and evaluate the result
    if (unmatched1 > 0 && unmatched2 == 0) return COMPARE_MORE;
    if (unmatched1 == 0 && unmatched2 > 0) return COMPARE_LESS;
    return COMPARE_INCONCLUSIVE;
  }
  
  /**
   * Insert the set 'add' of nodes (all having the same {@link DNode#id} into 'sets'
   * s.t. the 'sets' remain ordered by their {@link DNode#id}s. We use a naive quicksort
   * implementation to insert 'add' into 'sets'. 'fillStart' and 'fillEnd' determine
   * the part of the array 'sets' where 'add' may be inserted.
   * 
   * @param sets
   * @param fillStart
   * @param fillEnd
   * @param add
   */
  private static void insertSetSorted(Set<DNode>[] sets, int fillStart, int fillEnd, Set<DNode> add) {
    if (fillStart == fillEnd) {
      sets[fillStart] = add;
      return;
    }
    
    int median = (fillEnd+fillStart)/2;
    short medianVal = sets[median].iterator().next().id;
    
    if (add.iterator().next().id < medianVal) {
      // add before the median, shift everything behind the median one slot
      // to gain space up to the median for inserting 'add'
      for (int j=fillEnd;j>median;j--) {
        sets[j] = sets[j-1];
      }
      insertSetSorted(sets,fillStart,median,add);
    } else {
      // add after the median
      insertSetSorted(sets,median+1,fillEnd,add);
    }
  }
  
}
