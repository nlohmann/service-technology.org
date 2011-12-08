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
import java.util.LinkedList;
import java.util.Map.Entry;

import hub.top.adaptiveSystem.AdaptiveSystemFactory;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.OccurrenceNet;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Orientation;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.Temp;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;

/**
 * Output a branching process as a Petri net model {@link PtNet} for
 * viewing and manipulating in a graphical editor.
 * 
 * @author Dirk Fahland
 */
public class DNode2Oclet {

  public static Oclet toCounterExampleOclet_normal(DNodeBP bp, DNode[] maxNodes) {
    
    AdaptiveSystemFactory fact = AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystemFactory();
    
    Oclet o = fact.createOclet();
    PreNet preNet = fact.createPreNet(); preNet.setName("pre");
    DoNet doNet = fact.createDoNet();    doNet.setName("do");
    o.setPreNet(preNet);
    o.setDoNet(doNet);
    
    o.setName("counter-example");

    HashMap<DNode, Node> nodeMap = new HashMap<DNode, Node>();
    
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (int i=0;i<maxNodes.length;i++)
      queue.add(maxNodes[i]);
    
    // first print all conditions
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      
      String name = bp.getSystem().properNames[d.id];

      Node n = null;
      // DNode d has not been translated yet, create it and add it to the oclet
      if (d.isEvent) {
        Event e = fact.createEvent();
        e.setName(name);
        
        n = e;
      } else {
        Condition b = fact.createCondition();
        b.setName(name);
        
        n = b;
      }
      nodeMap.put(d, n);
      
      if (bp.getBranchingProcess().initialConditions.contains(d)) {
        // add node to the preNet
        n.setTemp(Temp.WITHOUT);
        if (n instanceof Condition) ((Condition)n).setToken(1);
        preNet.getNodes().add(n);
        
      } else {
        // add node to the doNet
        n.setTemp(Temp.HOT);
        doNet.getNodes().add(n);
      }
      
      if (d.pre != null)
        for (DNode pre: d.pre) {
          if (!nodeMap.containsKey(pre))
            queue.addLast(pre);
        }
    }
    
    for (Entry<DNode, Node> d2n: nodeMap.entrySet()) {
      
      DNode d = d2n.getKey();
      Node n = d2n.getValue();
      
      if (d.post != null) {
        for (DNode post : d.post) {
          if (nodeMap.get(post) != null)
            addArcToNet(n, nodeMap.get(post), (OccurrenceNet)n.eContainer(), fact);
        }
      }
    }
    
    return o;
  }
  
  public static Oclet toCounterExampleOclet_anti(DNodeBP bp, DNode[] maxNodes, DNode antiEvent) {
    
    AdaptiveSystemFactory fact = AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystemFactory();
    
    Oclet o = fact.createOclet();
    PreNet preNet = fact.createPreNet(); preNet.setName("pre");
    DoNet doNet = fact.createDoNet();    doNet.setName("do");
    o.setPreNet(preNet);
    o.setDoNet(doNet);
    
    o.setName("counter-example");
    o.setOrientation(Orientation.ANTI);

    HashMap<DNode, Node> nodeMap = new HashMap<DNode, Node>();
    
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (int i=0;i<maxNodes.length;i++)
      queue.add(maxNodes[i]);
    
    // first print all conditions
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      
      if (nodeMap.containsKey(d)) continue;
      
      String name = bp.getSystem().properNames[d.id];

      Node n = null;
      // DNode d has not been translated yet, create it and add it to the oclet
      if (d.isEvent) {
        Event e = fact.createEvent();
        e.setName(name);
        
        n = e;
      } else {
        Condition b = fact.createCondition();
        b.setName(name);
        
        n = b;
      }
      nodeMap.put(d, n);
      
      boolean inPreNet = true;
      if (d == antiEvent)
        inPreNet = false;
      else if (!d.isEvent && d.pre != null && d.pre.length > 0 && d.pre[0] == antiEvent)
        inPreNet = false;
      
      if (inPreNet) {
        // add node to the preNet
        n.setTemp(Temp.WITHOUT);
        if (n instanceof Condition) ((Condition)n).setToken(1);
        preNet.getNodes().add(n);
        
      } else {
        // add node to the doNet
        n.setTemp(Temp.HOT);
        doNet.getNodes().add(n);
      }
      
      if (d.pre != null)
        for (DNode pre: d.pre) {
          if (!nodeMap.containsKey(pre))
            queue.addLast(pre);
        }
    }
    
    for (Entry<DNode, Node> d2n: nodeMap.entrySet()) {
      
      DNode d = d2n.getKey();
      Node n = d2n.getValue();
      
      if (d.post != null) {
        for (DNode post : d.post) {
          if (nodeMap.get(post) != null)
            addArcToNet(n, nodeMap.get(post), (OccurrenceNet)n.eContainer(), fact);
        }
      }
    }
    
    return o;
  }
  
  private static void addArcToNet(Node src, Node tgt, OccurrenceNet net, AdaptiveSystemFactory fact) {
    if (src instanceof Condition) {
      ArcToEvent arc = fact.createArcToEvent();
      arc.setSource((Condition)src);
      arc.setDestination((Event)tgt);
      net.getArcs().add(arc);
    }
    if (src instanceof Event) {
      ArcToCondition  arc = fact.createArcToCondition();
      arc.setSource((Event)src);
      arc.setDestination((Condition)tgt);
      net.getArcs().add(arc);
    }
  }
  
}
