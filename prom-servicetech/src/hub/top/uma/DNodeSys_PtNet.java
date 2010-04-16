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
import java.util.Iterator;
import java.util.Map;

import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.PetrinetEdge;
import org.processmining.models.graphbased.directed.petrinet.PetrinetNode;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.semantics.petrinet.Marking;

/**
 * A {@link DNodeSys} representation of a Petri net for constructing a McMillan
 * prefix of the Petri net. This class is used in {@link DNodeBP}.
 * 
 * @author Dirk Fahland
 */
public class DNodeSys_PtNet extends DNodeSys {

	private int						      nodeNum;       // counter number of nodes 
	private Map<PetrinetNode, DNode>		nodeEncoding;  // encoding of net Nodes as DNodes
	
	/**
	 * All nodes corresponding to nodes of the original Petri net without successor.
	 */
	public HashSet<Short>			  terminalNodes;
	
	/**
	 * Construct {@link DNodeSys} from a Petri net. Every transition becomes
	 * a {@link DNodeSys#fireableEvents}, places are stored as corresponding
	 * pre- and post-conditions.
	 * 
	 * @param net
	 * @throws InvalidModelException
	 */
	public DNodeSys_PtNet(Petrinet net, Marking initMarking) throws InvalidModelException {
		// initialize the standard data structures
		super();
		
		terminalNodes = new HashSet<Short>();
		
		// create a name table initialize the translation tables
		buildNameTable(net);
		nodeEncoding = new HashMap<PetrinetNode, DNode>(nodeNum);
		
		// then translate the net
		buildDNodeRepresentation(net);
		
		// and set the initial state
		initialRun = buildInitialState(net, initMarking);
		
		finalize_setPreConditions();
		finalize_initialRun();
	}
	
	/**
	 * Translate full strings of the Petri net to ids and fill
	 * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
	 * 
	 * @param net
	 */
	private void buildNameTable(Petrinet net) {
		short currentNameID = 0;
		nameToID = new HashMap<String, Short>();
		
		nodeNum = 0;
		
		// collect all names and assign each new name a new ID
		for (PetrinetNode n : net.getTransitions()) {
			if (nameToID.get(n.getLabel()) == null)
				nameToID.put(n.getLabel(), currentNameID++);
			nodeNum++;
		}
		for (PetrinetNode n : net.getPlaces()) {
			if (nameToID.get(n.getLabel()) == null)
				nameToID.put(n.getLabel(), currentNameID++);
			nodeNum++;
			
			if (net.getOutEdges(n).size() == 0)
				// remember ID of this terminal node
				terminalNodes.add(nameToID.get(n.getLabel()));
		}
		
		// build the translation table from IDs to names
		finalize_setProperNames();
	}
	
	/**
	 * Extend post-set of <code>other</code> by a new node.
	 * 
	 * HV: Never used locally, hence commented out.
	 * 
	 * @param other
	 * @param toAdd
	 */
//	private void addToPostNodes(DNode other, DNode toAdd) {
//		int j=0;
//		for (; j<other.post.length; j++) {
//			if (other.post[j] == null) {
//				other.post[j] = toAdd;
//				break;
//			}
//		}
//		// we've added all successors of pre[i]: sort the nodes by their IDs
//		if (j == other.post.length-1) {
//			DNode.sortIDs(other.post);
//		}
//	}

  /**
   * Extend pre-set of <code>other</code> by a new node.
   * 
   * HV: Never used locally, hence commented out.
   * 
   * @param other
   * @param toAdd
   */
//	private void addToPreNodes(DNode other, DNode toAdd) {
//		int j=0;
//		for (; j<other.pre.length; j++) {
//			if (other.pre[j] == null) {
//				other.pre[j] = toAdd;
//				break;
//			}
//		}
//		// we've added all successors of pre[i]: sort the nodes by their IDs
//		if (j == other.pre.length-1) {
//			DNode.sortIDs(other.pre);
//		}
//	}
	
	/**
	 * The actual translation algorithm for transforming a PetriNet into
	 * the {@link DNode} representation.
	 * 
	 * @param net
	 * @throws InvalidModelException
	 */
	private void buildDNodeRepresentation(Petrinet net) throws InvalidModelException  {
		/*
		// first translate all places without predecessors
		for (Node n : net.getPlaces()) {
			DNode d = new DNode(nameToID.get(n.getName()), null);
			
			// prepare predecessors
			if (n.getIncoming().size() > 0) {
				d.pre = new DNode[n.getIncoming().size()];
				for (int i=0; i<n.getIncoming().size(); i++)
					d.pre[i] = null;
			}
			
			// prepare successors
			if (n.getOutgoing().size() > 0) {
				d.post = new DNode[n.getOutgoing().size()];
				for (int i=0; i<n.getOutgoing().size(); i++)
					d.post[i] = null;
			}
			
			nodeEncoding.put(n, d);		// store new pair
		}
		
		// then translate all transitions setting predecessors and successors
		for (Node n : net.getTransitions()) {
			
			// collect all predecessors
			LinkedList<Node> preNodes = new LinkedList<Node>();
			for (Arc a : n.getIncoming()) {
				preNodes.add(a.getSource());
			}
			
			// save the DNode predecessors of node n in an array
			DNode pre[] = new DNode[preNodes.size()];
			for (int i=0; i<pre.length; i++) {
				pre[i] = nodeEncoding.get(preNodes.get(i));
			}
			
			// create new DNode d for Node n
			DNode d = new DNode(nameToID.get(n.getName()), pre);
			d.isEvent = true;
			fireableEvents = fireableEvents.add(d);
			
			// make DNode d a post-node of each of its predecessors
			for (int i=0; i<pre.length; i++) {
				addToPostNodes(pre[i], d);
			}
			
			// prepare successors of DNode d,
			// and make DNode d a pre-node of each of its successors
			if (n.getOutgoing().size() > 0) {
				d.post = new DNode[n.getOutgoing().size()];

				int num = 0;
				for (Arc a : n.getOutgoing()) {
					d.post[num] = nodeEncoding.get(a.getTarget());
					// make DNode d a predecessor of each successor of d
					addToPreNodes(d.post[num], d);
					num++;
				}
				// sort all successors of DNode d
				DNode.sortIDs(d.post);
			}
			
			nodeEncoding.put(n, d);		// store new pair
		}
		*/
		
		// then translate all transitions setting predecessors and successors
		for (PetrinetNode n : net.getTransitions()) {
			
			
			
			// create new DNode copies of all predecessors
			DNode pre[] = null;
			if (net.getInEdges(n).size() > 0) {
				pre = new DNode[net.getInEdges(n).size()];
				int i = 0;
				for (PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode> a : net.getInEdges(n)) {
					pre[i++] = new DNode(nameToID.get(a.getSource().getLabel()), null);
				}
				DNode.sortIDs(pre);
			} else {
			  throw new InvalidModelException(InvalidModelException.EMPTY_PRESET,
			      n, "Node "+n.getLabel()+" has an empty pre-set.");
			}
			
			// create new DNode d for Node n
			DNode d = new DNode(nameToID.get(n.getLabel()), pre);
			d.isEvent = true;
			fireableEvents = fireableEvents.add(d);
			
			// create pre-/post-set for successor/predecessor nodes of d
			DNode[] dPostPre = new DNode[1]; dPostPre[0] = d;
			
			// make DNode d a post-node of each of its predecessors
			for (int i=0; i<pre.length; i++) {
				pre[i].post = dPostPre; 
			}
			
			// prepare successors of DNode d,
			// and make DNode d a pre-node of each of its successors
			if (net.getOutEdges(n).size() > 0) {
				d.post = new DNode[net.getOutEdges(n).size()];

				int i = 0;
				for (PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode> a : net.getOutEdges(n)) {
					d.post[i++] = new DNode(nameToID.get(a.getTarget().getLabel()), dPostPre);
				}
				// sort all successors of DNode d
				DNode.sortIDs(d.post);
			} else {
			  // has no successors, but initialize post set
			  d.post = new DNode[0];
			}
			
			nodeEncoding.put(n, d);		// store new pair
		}
	}
	
	/**
	 * Translate initial marking of the Petri net into an initial run of this system.
	 * 
	 * @param net
	 * @return
	 *     the {@link DNodeSet} representing the initial run of the system
	 */
	private DNodeSet buildInitialState(Petrinet net, Marking initMarking) {

		DNodeSet ds = new DNodeSet(properNames.length);
		
		Iterator<Place> markedPlaces = initMarking.iterator();

		// create a new DNode for every token on every place in the net
		while (markedPlaces.hasNext()) {
			
			Place p = markedPlaces.next();
			int tokens = initMarking.occurrences(p);
			for (int i=0; i<tokens; i++)
				ds.add(new DNode(nameToID.get(p.getLabel()), new DNode[0]));
		}
		
		return ds;
	}
	
	/**
	 * @param d
	 * @return <code>true</code> iff the given node 'd' represents a Petri net place
	 * without successor, i.e. a structurally dead place.
	 * 
	 * @see hub.top.greta.oclets.canonical.DNodeSys#isTerminal(hub.top.greta.oclets.canonical.DNode)
	 */
	@Override
	public boolean isTerminal(DNode d) {
      return terminalNodes.contains(d.id);
	}
}
