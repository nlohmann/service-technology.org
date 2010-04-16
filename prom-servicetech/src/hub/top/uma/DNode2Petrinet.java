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

package hub.top.uma;

import java.util.HashMap;
import java.util.HashSet;

import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.PetrinetNode;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.models.graphbased.directed.petrinet.impl.PetrinetFactory;
import org.processmining.models.semantics.petrinet.Marking;


/**
 * Output a branching process as a Petri net model {@link Petrinet} for
 * viewing and manipulating in a graphical editor.
 * 
 * @author Dirk Fahland
 * @email dirk.fahland@service-technology.org
 * @version Mar 23, 2010
 */
public class DNode2Petrinet {
	
  /**
   * @param bp
   * @return
   * 	an array [{@link Petrinet} net, {@link Marking} initMarking] representing
   *    the branching process bp as a Petri net and its initial marking
   */
  public static Petrinet process(DNodeBP bp) {
    
    Petrinet net = PetrinetFactory.newPetrinet("Complete prefix of unfolding");

    HashSet<DNode> allNodes = bp.getBranchingProcess().getAllNodes();
    
    HashMap<Integer, PetrinetNode> nodeMap = new HashMap<Integer, PetrinetNode>();
    
    // first print all conditions
    for (DNode n : allNodes) {
      if (n.isEvent)
        continue;

      // if (!option_printAnti && n.isAnti) continue;

      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
        
      Place p = net.addPlace(name); 
      nodeMap.put(n.globalId, p);
      
      //if (bp.getBranchingProcess().initialConditions.contains(n))
      //  initMarking.add(p);
    }

    for (DNode n : allNodes) {
      if (!n.isEvent)
        continue;
      
      // if (!option_printAnti && n.isAnti) continue;
      
      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
      
      Transition t = net.addTransition(name);
      nodeMap.put(n.globalId, t);
    }
    
    for (DNode n : allNodes) {
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
