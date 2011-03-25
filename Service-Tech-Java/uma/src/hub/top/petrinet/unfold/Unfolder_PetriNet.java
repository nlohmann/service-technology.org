/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. AGPL3.0
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

package hub.top.petrinet.unfold;

import java.util.HashMap;
import java.util.HashSet;

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSet;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Options;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.Uma;

public class Unfolder_PetriNet {
  
  // a special representation of the Petri net for the unfolder
  private DNodeSys_PetriNet sys;
  
  // the unfolding 
  private DNodeBP bp;

  /**
   * Initialize the unfolder to construct a finite complete prefix
   * of a safe Petri net.
   * 
   * @param net a safe Petri net
   */
  public Unfolder_PetriNet(PetriNet net) {
    try {
      sys = new DNodeSys_PetriNet(net);
      
      Options o = new Options(sys);
      // configure to unfold a Petri net
      o.configure_PetriNet();
      // stop construction of unfolding when reaching an unsafe marking
      o.configure_stopIfUnSafe();
      
      // initialize unfolder
      bp = new DNodeBP(sys, o);
      
    } catch (InvalidModelException e) {
      
      Uma.err.println("Error! Invalid model.");
      Uma.err.println(e);
      sys = null;
      bp = null;
    }
  }
  
  /**
   * compute the unfolding
   */
  public void computeUnfolding() {
    
    int total_steps = 0;
    int current_steps = 0;
    // extend unfolding until no more events can be added
    while ((current_steps = bp.step()) > 0) {
      total_steps += current_steps;
      Uma.out.print(total_steps+"... ");
    }
  }
  
  /**
   * Convert the unfolding into a Petri net and return this Petri net
   * @return
   */
  public PetriNet getUnfoldingAsPetriNet() {
    return convertToPetrinet(bp.getBranchingProcess());
  }

  /**
   * Convert the branching process into a Petri net and return this Petri net
   * @param bp
   * @return
   */
  public static PetriNet convertToPetrinet(DNodeSet bp) {
    PetriNet unfolding = new PetriNet();
    DNodeSetElement allNodes = bp.getAllNodes();
    
    HashMap<Integer, Node> nodeMap = new HashMap<Integer, Node>();
    
    // first print all conditions
    for (DNode n : allNodes) {
      if (n.isEvent)
        continue;

      // if (!option_printAnti && n.isAnti) continue;

      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
        
      Place p = unfolding.addPlace(name);
      nodeMap.put(n.globalId, p);
      
      if (bp.initialConditions.contains(n))
        p.setTokens(1);
    }

    for (DNode n : allNodes) {
      if (!n.isEvent)
        continue;
      
      // if (!option_printAnti && n.isAnti) continue;
      
      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
      
      Transition t = unfolding.addTransition(name);
      nodeMap.put(n.globalId, t);
    }
    
    for (DNode n : allNodes) {
      if (n.isEvent) {
        for (DNode pre : n.pre) {
          unfolding.addArc(
                (Place)nodeMap.get(pre.globalId),
                (Transition)nodeMap.get(n.globalId));
        }
      } else {
        for (DNode pre : n.pre) {
          unfolding.addArc(
                (Transition)nodeMap.get(pre.globalId),
                (Place)nodeMap.get(n.globalId));
        }
      }
    }
    return unfolding;
  }
  
  /**
   * @return the unfolding in GraphViz dot format
   */
  public String getUnfoldingAsDot() {
    return bp.getBranchingProcess().toDot(sys.properNames);
  }
}
