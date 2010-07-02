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

package hub.top.scenario;

import hub.top.petrinet.Node;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

/**
 * A {@link DNode} representation of an {@link AdaptiveSystem} consisting of
 * {@link Oclet}s used for constructing a McMillan prefix of this system. This
 * class is used in {@link DNodeBP}.
 * 
 * 
 * @author Dirk Fahland
 */
public class DNodeSys_OcletSpecification extends DNodeSys {

	private int						nodeNum;	// counter number of nodes 
	
	private Map<Node, DNode>     nodeEncoding;    // encoding of oclet Nodes as DNodes
	private Map<DNode, Node>     nodeOrigin;      // inverse of #nodeEncoding
	private Map<Oclet, DNodeSet> ocletEncoding;   // encoding of oclets to DNodeSets

	
	/**
	 * Construct DNode representation of an {@link AdaptiveSystem}. Every event
	 * of an {@link Oclet}s contribution ({@link Oclet#getDoNet()}) becomes a
	 * {@link DNodeSys#fireableEvents}, every event of an {@link Oclet}s
	 * precondition ({@link Oclet#getPreNet()}) becomes a {@link DNodeSys#preConEvents).
	 * Pre- and post-conditions of these events are stored accordingly.
	 * 
	 * @param as
	 */
	public DNodeSys_OcletSpecification(OcletSpecification as) {

		// initialize the standard data structures
		super();
		
		// create a name table initialize the translation tables
		buildNameTable(as);
		nodeEncoding = new HashMap<Node, DNode>(nodeNum);
		nodeOrigin = new HashMap<DNode, Node>(nodeNum);
		ocletEncoding = new HashMap<Oclet, DNodeSet>(as.getOclets().size());
		
		// then translate all oclets, first normal oclets, then anti-oclets
		for (Oclet o : as.getOclets())
		  if (!o.isAntiOclet())
		    ocletEncoding.put(o, buildDNodeRepresentation(o));
	  for (Oclet o : as.getOclets())
	    if (o.isAntiOclet())
	      ocletEncoding.put(o, buildDNodeRepresentation(o));
	  
		// and the adaptive process
		initialRun = buildDNodeRepresentation_initial(as.getInitialRun());
		
		finalize_setPreConditions();
		finalize_initialRun();
		finalize_generateIndexes();
	}

	/**
	 * Translate full strings of the Adaptive System to ids and fill
   * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
   * 
	 * @param as
	 */
	private void buildNameTable(OcletSpecification as) {

		nodeNum = 0;
		
		// collect all names and assign each new name a new ID
		for (Node n : as.getInitialRun().getNodes()) {
			if (nameToID.get(n.getName()) == null)
				nameToID.put(n.getName(), currentNameID++);
			nodeNum++;
		}
		for (Oclet o : as.getOclets()) {
			for (Node n : o.getNodes()) {
				if (nameToID.get(n.getName()) == null)
					nameToID.put(n.getName(), currentNameID++);
				nodeNum++;
			}
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
	private Collection<DNode> translateToDNodes(Oclet o, boolean isAnti, boolean beginAtMarked) {
		LinkedList<DNode> maxNodes = new LinkedList<DNode>();
		LinkedList<Node> searchQueue = new LinkedList<Node>();
		
		Map<Node, Integer> depth = new HashMap<Node, Integer>();
		
		// construct DNode representation only from marked conditions
		if (beginAtMarked) {
		  for (Place b : o.getPlaces()) {
        if (b.getTokens() == 1) {
          searchQueue.add(b);
          
          // we compute the length of the longest path that ends in an
          // event, a path that ends in the pre-event of this condition
          // has one edge lass than a path that ends in this condition
          depth.put(b, -1); 
		    }
		  }
		  
		// construct DNode representation from all nodes
		} else {
  		// fill the queue with all maximal nodes of the occurrence net
  		for (Node n : o.getNodes()) {
				if (n.getPostSet().isEmpty()) {
					searchQueue.add(n);
					
					// we compute the length of the longest path that ends in an
					// event, if this event has no predecessor, the corresponding
					// path has length 0
					depth.put(n, 0);
				}
  		}
		}

		// then do a breadth-first search on the structure of the oclet
		// and add each visited node to the buildStack, the same node
		// can be added multiple times
		LinkedList<Node> buildStack = new LinkedList<Node>();
		while (!searchQueue.isEmpty()) {
			Node n = searchQueue.removeFirst();
			
			int n_depth = depth.get(n);
			
			for (Node post : n.getPreSet()) {

	      Integer d = depth.get(post);
				searchQueue.addLast(post);

				if (d == null || d <= n_depth) {
					depth.put(post, n_depth+1);	// increment depth of node
					// and remember maximum depth of a node
					maxHistoryDepth = Math.max(n_depth+1, maxHistoryDepth);
				}
			}
			if (buildStack.contains(n)) buildStack.remove(n);
			buildStack.addFirst(n);
		}
		
		// the buildStack now contains the nodes of the Oclet such that
		// for each Node in the stack, its predecessors in the graph are
		// also predecessors in the stack
		
		// translate each node from the stack
		
		while (!buildStack.isEmpty()) {
			Node n = buildStack.removeFirst();
			
			if (nodeEncoding.containsKey(n))	// node was already translated, skip
				continue;
			
      // save the predecessors of node n in an array
      DNode pre[] = new DNode[n.getPreSet().size()];
      for (int i=0; i<pre.length; i++) {
        pre[i] = nodeEncoding.get(n.getPreSet().get(i));
      }
			
			// count all successors that will be translated to a DNode
			int postSize = 0; 
      for (Node postNode : n.getPostSet()) {
        if (depth.containsKey(postNode)) postSize++;
      }
			
			// create new DNode d for Node n
			DNode d = new DNode(nameToID.get(n.getName()), pre);
			if (n instanceof Transition) {
				d.isEvent = true;
				// remember all events of a DoNet: these will be fired if enabled
				if (!o.isInHistory(n))
					fireableEvents.add(d);
				else
					preConEvents.add(d);
			}
			d.isAnti = isAnti;
			d.isHot = o.isHotNode(n); 
			
			// initialize post-set of new DNode d
			if (postSize > 0) {
				d.post = new DNode[postSize];
				for (int i=0; i<postSize; i++)
					d.post[i] = null;
			} else {
			  d.post = new DNode[0];
				maxNodes.add(d);	// no predecessor: a maximal node
			}
			
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
			nodeEncoding.put(n, d);		// store new pair
			nodeOrigin.put(d, n);     // and its reverse mapping
		}
		return maxNodes;
	}

	/**
	 * Translate an oclet into a {@link DNodeSet} of its {@link DNode}s.
	 * 
	 * @param o
	 * @return
	 */
	private DNodeSet buildDNodeRepresentation(Oclet o) {
		
		Collection<DNode> maxNodesOfO = translateToDNodes(o, o.isAntiOclet(), false);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : maxNodesOfO)
			ds.add(d);
		
		return ds;
	}
	
	/**
	 * Translate the initial run of an {@link AdaptiveSystem} into a {@link DNodeSet}
	 * representation.
	 *  
	 * @param ap
	 * @return
	 */
	private DNodeSet buildDNodeRepresentation_initial(Oclet o) {
		
		Collection<DNode> maxNodesOfAP = translateToDNodes(o, false, true);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : maxNodesOfAP)
			ds.add(d);
		return ds;
	}

	/**
   * the maximal cut of each oclet's history in the system, required
   * for finding implied scenarios
   *
	 * @return the maximal cut of each oclet's history in the system
	 */
	public LinkedList<DNode[]> getHistories() {
	  LinkedList<DNode[]> histories = new LinkedList<DNode[]>();
	  // iterate over all oclets
	  for (Oclet o : ocletEncoding.keySet()) {
	    // and return the maximal conditions of the pre-nets = global history
	    Collection<Place> histMax = o.getMaxHistory();
	    DNode[] dConditions = new DNode[histMax.size()];
	    int i=0;
	    for (Node p : histMax) {
	      if (nodeEncoding.get(p) == null) System.err.println(p+" of "+o+"was not translated");
	      dConditions[i++] = nodeEncoding.get(p);
	    }
	    DNode.sortIDs(dConditions);
	    histories.add(dConditions);
	  }
	  return histories;
	}
	
	/**
	 * @param d
	 * @return the node that caused the definition of {@link DNode} 'd' in this system
	 */
	public Node getOriginalNode(DNode d) {
	  return nodeOrigin.get(d);
	}
	
	@Override
	public String getInfo() {
    
    int numPlaces = 0;
    int numTransition = 0;
    int numArcs = 0;
    
    for (Node n : nodeEncoding.keySet()) {
      if (n instanceof Transition)
        numTransition++;
      else
        numPlaces++;
      numArcs += n.getPreSet().size();
    }
	  return "|P| = "+numPlaces+" |T| = " + numTransition + " |F| = "+numArcs;
	}
}
