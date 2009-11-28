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

import hub.top.editor.ptnetLoLA.ArcToPlace;
import hub.top.editor.ptnetLoLA.ArcToTransition;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.greta.oclets.canonical.DNode;
import hub.top.greta.oclets.canonical.DNodeBP;

/**
 * Output a branching process as a Petri net model {@link PtNet} for
 * viewing and manipulating in a graphical editor.
 * 
 * @author Dirk Fahland
 */
public class DNode2PtNet {

  public static PtNet process(DNodeBP bp) {
    
    PtnetLoLAFactory fact = PtnetLoLAPackage.eINSTANCE.getPtnetLoLAFactory();
    PtNet net = fact.createPtNet();
    HashSet<DNode> allNodes = bp.getBranchingProcess().getAllNodes();
    
    HashMap<Integer, Node> nodeMap = new HashMap<Integer, Node>();
    
    // first print all conditions
    for (DNode n : allNodes) {
      if (n.isEvent)
        continue;

      // if (!option_printAnti && n.isAnti) continue;

      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
        
      Place p = fact.createPlace();
      p.setName(name);
      net.getPlaces().add(p);
      nodeMap.put(n.globalId, p);
      
      if (bp.getBranchingProcess().initialConditions.contains(n))
        p.setToken(1);
    }

    for (DNode n : allNodes) {
      if (!n.isEvent)
        continue;
      
      // if (!option_printAnti && n.isAnti) continue;
      
      String name = n.toString();
      if (n.isAnti) name = "NOT "+name;
      else if (n.isCutOff) name = "CUT("+name+")";
      
      Transition t = fact.createTransition();
      t.setName(name);
      net.getTransitions().add(t);
      nodeMap.put(n.globalId, t);
    }
    
    for (DNode n : allNodes) {
      if (n.isEvent) {
        for (DNode pre : n.pre) {
          ArcToTransition a = fact.createArcToTransition();
          a.setSource((Place)nodeMap.get(pre.globalId));
          a.setTarget((Transition)nodeMap.get(n.globalId));
          net.getArcs().add(a);
        }
      } else {
        for (DNode pre : n.pre) {
          ArcToPlace a = fact.createArcToPlace();
          a.setSource((Transition)nodeMap.get(pre.globalId));
          a.setTarget((Place)nodeMap.get(n.globalId));
          net.getArcs().add(a);
        }
      }
    }
    return net;
  }
  
}
