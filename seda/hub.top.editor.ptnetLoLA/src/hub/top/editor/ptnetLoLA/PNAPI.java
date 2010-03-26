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

package hub.top.editor.ptnetLoLA;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

/**
 * 
 * API class providing basic functionality for manipulating Petri nets
 * 
 * @author Dirk Fahland
 */
public class PNAPI {

  /**
   * Merge the transitions t1 and t2 in the net. Adds pre- and post-places of
   * t2 to the pre- or post-set of t1 and removes t2 from the net.
   * 
   * @param net
   * @param t1
   * @param t2
   */
  public static void mergeTransitions(PtNet net, Transition t1, Transition t2) {
    for (Place p : t2.getPreSet()) {
      if (!t1.getPreSet().contains(p)) {
        ArcToTransition a = PtnetLoLAFactory.eINSTANCE.createArcToTransition();
        a.setSource(p);
        a.setTarget(t1);
        net.getArcs().add(a);
      }
    }
    for (Place p : t2.getPostSet()) {
      if (!t1.getPostSet().contains(p)) {
        ArcToPlace a = PtnetLoLAFactory.eINSTANCE.createArcToPlace();
        a.setSource(t1);
        a.setTarget(p);
        net.getArcs().add(a);
      }
    }
    removeTransition(net, t2);
  }
  
  /**
   * Merge the places p1 and p2 in the net. Creates a new place pMerge; the
   * pre-and post-transitions of pMerge are the pre- and post-transitions of
   * p1 and p2, respectively.
   * 
   * The original places p1 and p2 are not removed from the net. Use
   * {@link #removePlace(PtNet, Place)} to remove either once this is appropriate.
   * 
   * @param net
   * @param p1
   * @param p2
   * @return the new place pMerge
   */
  public static Place mergePlaces(PtNet net, Place p1, Place p2) {
    Place pMerged = PtnetLoLAFactory.eINSTANCE.createPlace();
    pMerged.setName(p1.getName()+"_"+p2.getName());
    pMerged.setToken(p1.getToken() + p2.getToken());
    net.getPlaces().add(pMerged);

    for (Arc inArc : p1.getIncoming()) {
      ArcToPlace inArc_new = PtnetLoLAFactory.eINSTANCE.createArcToPlace();
      inArc_new.setSource(inArc.getSource());
      inArc_new.setTarget(pMerged);
      net.getArcs().add(inArc_new);
    }
    for (Arc inArc : p2.getIncoming()) {
      if (pMerged.getPreSet().contains(inArc.getSource())) continue;
      
      ArcToPlace inArc_new = PtnetLoLAFactory.eINSTANCE.createArcToPlace();
      inArc_new.setSource(inArc.getSource());
      inArc_new.setTarget(pMerged);
      net.getArcs().add(inArc_new);
    }
    for (Arc outArc : p1.getOutgoing()) {
      ArcToTransition outArc_new = PtnetLoLAFactory.eINSTANCE.createArcToTransition();
      outArc_new.setSource(pMerged);
      outArc_new.setTarget(outArc.getTarget());
      net.getArcs().add(outArc_new);
    }
    for (Arc outArc : p2.getOutgoing()) {
      if (pMerged.getPreSet().contains(outArc.getTarget())) continue;
      
      ArcToTransition outArc_new = PtnetLoLAFactory.eINSTANCE.createArcToTransition();
      outArc_new.setSource(pMerged);
      outArc_new.setTarget(outArc.getTarget());
      net.getArcs().add(outArc_new);
    }
    
    return pMerged;
  }

  /**
   * Remove transition t from the net. Remove also all adjacent arcs.
   * 
   * @param net
   * @param t
   */
  public static void removeTransition(PtNet net, Transition t) {
    for (Arc a : t.getIncoming()) {
      a.setSource(null);
    }
    for (Arc a : t.getOutgoing()) {
      a.setTarget(null);
    }
    net.getArcs().removeAll(t.getIncoming());
    net.getArcs().removeAll(t.getOutgoing());
    net.getTransitions().remove(t);
  }
  
  /**
   * Remove place p from the net. Remove also all adjacent arcs and update
   * the markings where p is involved.
   * 
   * @param net
   * @param p
   */
  public static void removePlace(PtNet net, Place p) {
    
    if (p.eContainer() != net) {
      System.err.println("Error. Tried to remove place "+p.getName()+" from net, but this place is not contained in the net");
      return;
    }
    
    p.setToken(0);
    p.setFinalMarking(0);
    for (Arc a : p.getIncoming()) {
      a.setSource(null);
    }
    for (Arc a : p.getOutgoing()) {
      a.setTarget(null);
    }
    net.getArcs().removeAll(p.getIncoming());
    net.getArcs().removeAll(p.getOutgoing());
    net.getPlaces().remove(p);
  }
  
  /**
   * Remove arc a from the net. 
   * @param net
   * @param a
   */
  public static void removeArc(PtNet net, Arc a) {
    a.setSource(null);
    a.setTarget(null);
    net.getArcs().remove(a);
  }

  /**
   * Check whether transitions t1 and t2 are parallel (except for the places in
   * the set ignore). Two transitions are parallel if their pre- and post-sets
   * are identical.
   * 
   * @param t1
   * @param t2
   * @param ignore
   * @return
   *    true iff transitions t1 and t2 are parallel except for the places in
   *    the set ignore
   */
  public static boolean parallelTransitions(Transition t1, Transition t2, Collection<Place> ignore) {
    for (Place p : t1.getPreSet()) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!t2.getPreSet().contains(p)) return false;
    }
    for (Place p : t1.getPostSet()) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!t2.getPostSet().contains(p)) return false;
    }
    for (Place p : t2.getPreSet()) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!t1.getPreSet().contains(p)) return false;
    }
    for (Place p : t2.getPostSet()) {
      if (ignore != null && ignore.contains(p)) continue;
      if (!t1.getPostSet().contains(p)) return false;
    }
    return true;
  }
  
  /**
   * Check whether transitions t1 and t2 are parallel. Two transitions are
   * parallel if their pre- and post-sets are identical.
   * 
   * @param t1
   * @param t2
   * @return
   *    true iff transitions t1 and t2 are parallel
   */
  public static boolean parallelTransitions(Transition t1, Transition t2) {
    return parallelTransitions(t1, t2, null);
  }

  /**
   * write the Petri net in GraphViz dot format
   * 
   * @param net
   * @return
   */
  public static String toDot(PtNet net) {
    StringBuilder b = new StringBuilder();
    b.append("digraph BP {\n");
    
    // standard style for nodes and edges
    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
    b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

    String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
    
    HashMap<Node, Integer> nodeIDs = new HashMap<Node, Integer>();
    int nodeID = 0;
    
    // first print all places
    b.append("\n\n");
    b.append("node [shape=circle];\n");
    for (Place p : net.getPlaces()) {
      nodeID++;
      nodeIDs.put(p, nodeID);
      
      if (p.getToken() > 0)
        b.append("  p"+nodeID+" ["+tokenFillString+"]\n");
      else
        b.append("  p"+nodeID+" []\n");
      
      String auxLabel = "";
        
      b.append("  p"+nodeID+"_l [shape=none];\n");
      b.append("  p"+nodeID+"_l -> p"+nodeID+" [headlabel=\""+toLoLA_ident(p.getName())+" "+auxLabel+"\"]\n");
    }

    // then print all events
    b.append("\n\n");
    b.append("node [shape=box];\n");
    for (Transition t : net.getTransitions()) {
      nodeID++;
      nodeIDs.put(t, nodeID);

      b.append("  t"+nodeID+" []\n");
      
      String auxLabel = "";
      
      b.append("  t"+nodeID+"_l [shape=none];\n");
      b.append("  t"+nodeID+"_l -> t"+nodeID+" [headlabel=\""+toLoLA_ident(t.getName())+" "+auxLabel+"\"]\n");
    }
    
    /*
    b.append("\n\n");
    b.append(" subgraph cluster1\n");
    b.append(" {\n  ");
    for (CNode n : nodes) {
      if (n.isEvent) b.append("e"+n.localId+" e"+n.localId+"_l ");
      else b.append("c"+n.localId+" c"+n.localId+"_l ");
    }
    b.append("\n  label=\"\"\n");
    b.append(" }\n");
    */
    
    // finally, print all edges
    b.append("\n\n");
    b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
    for (Arc arc : net.getArcs()) {
      if (arc instanceof ArcToPlace)
        b.append("  t"+nodeIDs.get(arc.getSource())+" -> p"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
      else
        b.append("  p"+nodeIDs.get(arc.getSource())+" -> t"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
    }
    b.append("}");
    return b.toString();
  }
  
  /**
   * write the Petri net in GraphViz dot format
   * 
   * @param net
   * @return
   */
  public static String toDot(PtNet net, Map<String, Integer> cluster) {
    StringBuilder b = new StringBuilder();
    b.append("digraph BP {\n");
    b.append("compound=true;\n");
    b.append("ratio=\"1.333\";\n");
    
    // standard style for nodes and edges
    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
    b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

    String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
    
    HashMap<Node, Integer> nodeIDs = new HashMap<Node, Integer>();
    int nodeID = 0;

    // get maximum cluster number
    HashSet<Integer> clusterValues = new HashSet<Integer>(cluster.values());
    int maxValue = Integer.MIN_VALUE;
    for (int i : clusterValues) {
      if (maxValue < i) maxValue = i+1;
    }
    
    // assign places to clusters
    HashMap<Node, Integer> nodeCluster = new HashMap<Node, Integer>();
    for (Place p : net.getPlaces()) {
      boolean found = false;
      for (String s : cluster.keySet()) {
        if (p.getName().startsWith(s)) {
          nodeCluster.put(p, cluster.get(s));
          found = true;
          break;
        }
      }
      // the "unassigned" cluster
      if (!found) nodeCluster.put(p, maxValue);
    }
    for (Transition t : net.getTransitions()) {
      boolean found = false;
      for (String s : cluster.keySet()) {
        if (t.getName().startsWith(s)) {
          nodeCluster.put(t, cluster.get(s));
          found = true;
          break;
        }
      }
      // the "unassigned" cluster
      if (!found) nodeCluster.put(t, maxValue);
    }
    
    
    for (Integer i : clusterValues) {
      
      if (i == 0) continue;
      
      // set color of nodes and clusters
      String nodeColor = "white";
      String graphColor = "";
      String invisColor = "";
      if (i == 1) {
        nodeColor = "fillcolor=\"#BCA61B\"";
        graphColor = "fillcolor=\"#EDE6C2\" style=filled";
        //invisColor = "color=\"#EDE6C2\" labelfontcolor=\"#000000\";";
      }
      if (i == 2) {
        nodeColor = "fillcolor=\"#00572C\"";
        graphColor = "fillcolor=\"#CCDDD5\"; style=filled;";
        //invisColor = "color=\"#CCDDD5\" fontcolor=\"#000000\";";
      }
      
      b.append("\n\n");
      b.append(" subgraph cluster"+i+"\n");
      b.append(" {\n  ");
      b.append("  "+graphColor);
    
      // first print all places
      b.append("\n\n");
      b.append("  node [shape=circle];\n");
      for (Place p : net.getPlaces()) {
        // not in the i-th cluster
        if (nodeCluster.get(p) != i) continue;
        
        nodeID++;
        nodeIDs.put(p, nodeID);
        
        if (p.getToken() > 0)
          b.append("  p"+nodeID+" ["+tokenFillString+"]\n");
        else
          b.append("  p"+nodeID+" ["+nodeColor+"]\n");
        
        String auxLabel = "";
          
        b.append("  p"+nodeID+"_l [shape=none height=.001 width=.001 style=invisible];\n");
        b.append("  p"+nodeID+"_l -> p"+nodeID+" [headlabel=\""+toLoLA_ident(p.getName())+" "+auxLabel+"\" weight=1.0 "+invisColor+" contrainst=false]\n");
      }
  
      // then print all events
      b.append("\n\n");
      b.append("  node [shape=box];\n");
      for (Transition t : net.getTransitions()) {
        // not in the i-th cluster
        if (nodeCluster.get(t) != i) continue;
        
        nodeID++;
        nodeIDs.put(t, nodeID);
  
        b.append("  t"+nodeID+" ["+nodeColor+"]\n");
        
        String auxLabel = "";
        
        b.append("  t"+nodeID+"_l [shape=none height=.001 width=.001 style=invisible];\n");
        b.append("  t"+nodeID+"_l -> t"+nodeID+" [headlabel=\""+toLoLA_ident(t.getName())+" "+auxLabel+"\" weight=1.0 "+invisColor+" contrainst=false]\n");
      }
      
      b.append("\n  label=\"\"\n");
      b.append(" }\n");
    }
    
    //// ------- print all remaining nodes outside of the cluster -------
    b.append("\n\n");
    b.append(" subgraph clusterX\n");
    b.append(" {\n  ");
    
    // first print all places
    b.append("\n\n");
    b.append("  node [shape=circle];\n");
    for (Place p : net.getPlaces()) {
      // not in the i-th cluster
      if (nodeCluster.get(p) != maxValue && nodeCluster.get(p) != 0) continue;
      
      // set color of nodes
      String nodeColor;
      if (nodeCluster.get(p) == 0) {
        nodeColor = "fillcolor=\"#00376C\"";
      } else {
        nodeColor = "";
      }

      nodeID++;
      nodeIDs.put(p, nodeID);
      
      if (p.getToken() > 0)
        b.append("  p"+nodeID+" ["+tokenFillString+"]\n");
      else
        b.append("  p"+nodeID+" ["+nodeColor+"]\n");
      
      String auxLabel = "";
        
      b.append("  p"+nodeID+"_l [shape=none];\n");
      b.append("  p"+nodeID+"_l -> p"+nodeID+" [headlabel=\""+toLoLA_ident(p.getName())+" "+auxLabel+"\" weight=1.0]\n");
    }
    // then print all events
    b.append("\n\n");
    b.append("  node [shape=box];\n");
    for (Transition t : net.getTransitions()) {
      // not in the i-th cluster
      if (nodeCluster.get(t) != maxValue && nodeCluster.get(t) != 0) continue;
      
      // set color of nodes
      String nodeColor;
      if (nodeCluster.get(t) == 0) {
        nodeColor = "fillcolor=\"#00376C\"";
      } else {
        nodeColor = "";
      }
      
      nodeID++;
      nodeIDs.put(t, nodeID);

      b.append("  t"+nodeID+" ["+nodeColor+"]\n");
      
      String auxLabel = "";
      
      b.append("  t"+nodeID+"_l [shape=none];\n");
      b.append("  t"+nodeID+"_l -> t"+nodeID+" [headlabel=\""+toLoLA_ident(t.getName())+" "+auxLabel+"\" weight=1.0]\n");
    }
    
    b.append("\n  label=\"\"\n");
    b.append(" }\n");
    
    /*
    b.append("\n\n");
    b.append(" subgraph cluster1\n");
    b.append(" {\n  ");
    for (CNode n : nodes) {
      if (n.isEvent) b.append("e"+n.localId+" e"+n.localId+"_l ");
      else b.append("c"+n.localId+" c"+n.localId+"_l ");
    }
    b.append("\n  label=\"\"\n");
    b.append(" }\n");
    */
    
    // finally, print all edges
    b.append("\n\n");
    b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
    for (Arc arc : net.getArcs()) {
      
      int weight = 10000;
      if (nodeCluster.get(arc.getSource()).equals(nodeCluster.get(arc.getTarget())))
          weight = 1;
      else
          weight = 10000;
      
      if (arc instanceof ArcToPlace)
        b.append("  t"+nodeIDs.get(arc.getSource())+" -> p"+nodeIDs.get(arc.getTarget())+" [weight="+weight+".0]\n");
      else
        b.append("  p"+nodeIDs.get(arc.getSource())+" -> t"+nodeIDs.get(arc.getTarget())+" [weight="+weight+".0]\n");
    }
    b.append("}");
    return b.toString();
  }
  
  public static String toLoLA_ident(String s) {
    return s.substring(0,s.lastIndexOf('_'));
  }
}
