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
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSys;
import hub.top.uma.DNodeSet.DNodeSetElement;

import java.util.HashMap;

/**
 * Synthesize a (possibly labeled) Petri net by folding the branching process
 * along its cut-off equivalence relation.
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
  public static PetriNet foldToNet_labeled(DNodeBP bp) {
    DNodeSetElement showNodes = getAllNodes_notHotAnti(bp);
    return foldToNet_labeled(bp, showNodes);
  }
  
  /**
   * Fold the given branching process into a possibly labeled Petri net.
   * 
   * @param bp
   * @param nodesToShow generate Petri net only from the nodes in this set 
   *                    if <code>null</code>, then all nodes will be used
   * @return
   */
  public static PetriNet foldToNet_labeled(DNodeBP bp, DNodeSetElement fromNodes) {
    
    //bp.printEquivalenceRelation();
    
    try {
      bp.minimize();
      bp.relax();
    } catch (NullPointerException e) {
      
    }
    
    //bp.printEquivalenceRelation();
    
    DNodeSys dAS = bp.getSystem();
    
    // maps each DNode to its least equivalent DNode (to itself it is the least)
    HashMap<DNode, DNode> equiv = bp.getElementary_ccPair();
    // map that assigns each DNode to its synthesize net counter-part
    // built up during net construction
    HashMap<DNode, Node> d2n = new HashMap<DNode, Node>();
    
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
      
      if (!e.isCutOff || equiv.get(e) == e) {
        String label = "";
        if (e.isAnti && !e.isHot)
          label = "tau_"+e.globalId;   // hide cold anti-events
        else
          label = dAS.properNames[e.id];
        Transition t = net.addTransition(label);
        d2n.put(e, t);
      }
    }

    // now map each event that has an equivalent counterpart to
    // the transition that represents this counterpart and
    // generate the arcs to the corresponding places
    for (DNode e : fromNodes) {
      if (!e.isEvent) continue;  
      // events only
      
      Transition t = null;
      if (!e.isCutOff) {
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
        } else {
          
          // follow down the chain of equivalent events until
          // reaching the least event in that chain
          while (equiv.get(e2) != null && equiv.get(e2) != e2)
            e2 = equiv.get(e2);
          
          t = (Transition)d2n.get(e2);
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
  
}
