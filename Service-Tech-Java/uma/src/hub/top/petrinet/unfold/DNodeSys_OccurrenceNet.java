/*****************************************************************************\
 * Copyright (c) 2008-2010. All rights reserved. Dirk Fahland. AGPL3.0
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

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

public class DNodeSys_OccurrenceNet extends DNodeSys {
  
  private int                 nodeNum;       // counter number of nodes 
  private Map<DNode, Node>    nodeOrigin;    // inverse of #nodeEncoding
  private Map<Node, DNode>    nodeEncoding;  // encoding of net Nodes as DNodes
  
  private Set<Place>          implicitPlaces;// implicit places of the net

  /**
   * Construct {@link DNodeSys} from a Petri net. Every transition becomes
   * a {@link DNodeSys#fireableEvents}, places are stored as corresponding
   * pre- and post-conditions.
   * 
   * @param net
   * @throws InvalidModelException
   */
  public DNodeSys_OccurrenceNet(PetriNet net, Set<Place> implicitPlaces) throws InvalidModelException {
    // initialize the standard data structures
    super();
    
    // create a name table initialize the translation tables
    buildNameTable(net);
    nodeEncoding = new HashMap<Node, DNode>(nodeNum);
    nodeOrigin = new HashMap<DNode, Node>(nodeNum);
    
    this.implicitPlaces = implicitPlaces; 
    
    // and set the initial state
    initialRun = buildDNodeRepresentation(net);
    
    finalize_setPreConditions();
    finalize_initialRun();
    finalize_generateIndexes();
  }
  
  /**
   * Translate full strings of the Petri net to ids and fill
   * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
   * 
   * @param net
   */
  private void buildNameTable(PetriNet net) {
    
    nodeNum = 0;
    
    // collect all names and assign each new name a new ID
    for (Node n : net.getTransitions()) {
      if (nameToID.get(n.getName()) == null)
        nameToID.put(n.getName(), currentNameID++);
      nodeNum++;
    }
    for (Node n : net.getPlaces()) {
      if (nameToID.get(n.getName()) == null)
        nameToID.put(n.getName(), currentNameID++);
      nodeNum++;
    }
    
    // build the translation table from IDs to names
    finalize_setProperNames();
  }
  
  /**
   * The actual translation algorithm for transforming an acylic Petri net
   * into a {@link DNodeSet}. The translations preserves the structure
   * of the net.
   * 
   * @param o
   * @param isAnti
   * @param beginAtMarked
   * @return
   */
  private LinkedList<DNode> translateToDNodes(PetriNet net) {
    LinkedList<DNode> allNodes = new LinkedList<DNode>();
    
    LinkedList<Node> buildQueue = new LinkedList<Node>();
    
    for (Node n : net.getPlaces()) {
      if (n.getIncoming().size() == 0) buildQueue.add(n);
    }
    for (Node n : net.getTransitions()) {
      if (n.getIncoming().size() == 0) buildQueue.add(n);
    }
    
    nextnode: while (!buildQueue.isEmpty()) {
      Node n = buildQueue.removeFirst();
      
      if (nodeEncoding.containsKey(n))  // node was already translated, skip
        continue;
      
      // save the predecessors of node n in an array
      DNode pre[] = new DNode[n.getPreSet().size()];
      for (int i=0; i<pre.length; i++) {
        pre[i] = nodeEncoding.get(n.getPreSet().get(i));
        if (pre[i] == null) {
          continue nextnode;
        }
      }
      
      // count all successors that will be translated to a DNode
      int postSize = n.getOutgoing().size();
      
      // create new DNode d for Node n
      DNode d = new DNode(nameToID.get(n.getName()), pre);
      if (n instanceof Transition) {
        d.isEvent = true;
      } else if (n instanceof Place) {
        d.isImplied = implicitPlaces.contains((Place)n);
      }
      
      // initialize post-set of new DNode d
      if (postSize > 0) {
        d.post = new DNode[postSize];
        for (int i=0; i<postSize; i++)
          d.post[i] = null;
      } else {
        d.post = new DNode[0];
      }
      allNodes.add(d);  // no predecessor: a maximal node
      
      // make DNode d a post-node of each of its predecessors
      for (int i=0; i<pre.length; i++) {
        int j=0;
        for (; j<pre[i].post.length; j++) {
          if (pre[i].post[j] == null) {
            pre[i].post[j] = d;
            break;
          }
        }
        // we've added all successors of pre[i]: sort the nodes by their IDs
        if (j == pre[i].post.length-1) {
          DNode.sortIDs(pre[i].post);
        }
      }
      nodeEncoding.put(n, d);   // store new pair
      nodeOrigin.put(d, n);     // and its reverse mapping
      
      for (Node m : n.getPostSet()) {
        if (buildQueue.contains(m)) buildQueue.remove(m);
        buildQueue.addLast(m);
      }
    }
    return allNodes;
  }
  
  /**
   * Translate an occurrence net into a {@link DNodeSet} of its {@link DNode}s.
   * 
   * @param o
   * @return
   */
  private DNodeSet buildDNodeRepresentation(PetriNet net) {
    
    Collection<DNode> maxNodesOfNet = translateToDNodes(net);
    
    // store the constructed DNodes in a DNodeSet, this represents the oclet
    DNodeSet ds = new DNodeSet(properNames.length);
    for (DNode d : maxNodesOfNet)
      ds.add(d);
    
    return ds;
  }
}
