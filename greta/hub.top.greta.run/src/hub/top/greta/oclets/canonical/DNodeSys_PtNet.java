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
 * 		Dirk Fahland
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

package hub.top.greta.oclets.canonical;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.RefMarkedPlace;

/**
 * A {@link DNodeSys} representation of a Petri net for constructing a McMillan
 * prefix of the Petri net. This class is used in {@link DNodeBP}.
 * 
 * @author Dirk Fahland
 */
public class DNodeSys_PtNet extends DNodeSys {

	private int						      nodeNum;       // counter number of nodes 
	private Map<Node, DNode>		nodeEncoding;  // encoding of net Nodes as DNodes
	
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
	public DNodeSys_PtNet(PtNet net) throws InvalidModelException {
		// initialize the standard data structures
		super();
		
		terminalNodes = new HashSet<Short>();
		
		// create a name table initialize the translation tables
		buildNameTable(net);
		nodeEncoding = new HashMap<Node, DNode>(nodeNum);
		
		// then translate the net
		buildDNodeRepresentation(net);
		
		// and set the initial state
		initialRun = buildInitialState(net);
		
		finalize_setPreConditions();
		finalize_initialRun();
	}
	
	/**
	 * Translate full strings of the Petri net to ids and fill
	 * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
	 * 
	 * @param net
	 */
	private void buildNameTable(PtNet net) {
		short currentNameID = 0;
		nameToID = new HashMap<String, Short>();
		
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
			
			if (n.getOutgoing().size() == 0)
				// remember ID of this terminal node
				terminalNodes.add(nameToID.get(n.getName()));
		}
		
		// build the translation table from IDs to names
		finalize_setProperNames();
	}
	
	/**
	 * Extend post-set of <code>other</code> by a new node.
	 * 
	 * @param other
	 * @param toAdd
	 */
	private void addToPostNodes(DNode other, DNode toAdd) {
		int j=0;
		for (; j<other.post.length; j++) {
			if (other.post[j] == null) {
				other.post[j] = toAdd;
				break;
			}
		}
		// we've added all successors of pre[i]: sort the nodes by their IDs
		if (j == other.post.length-1) {
			DNode.sortIDs(other.post);
		}
	}

  /**
   * Extend pre-set of <code>other</code> by a new node.
   * 
   * @param other
   * @param toAdd
   */
	private void addToPreNodes(DNode other, DNode toAdd) {
		int j=0;
		for (; j<other.pre.length; j++) {
			if (other.pre[j] == null) {
				other.pre[j] = toAdd;
				break;
			}
		}
		// we've added all successors of pre[i]: sort the nodes by their IDs
		if (j == other.pre.length-1) {
			DNode.sortIDs(other.pre);
		}
	}
	
	/**
	 * The actual translation algorithm for transforming a PetriNet into
	 * the {@link DNode} representation.
	 * 
	 * @param net
	 * @throws InvalidModelException
	 */
	private void buildDNodeRepresentation(PtNet net) throws InvalidModelException  {
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
		for (Node n : net.getTransitions()) {
			
			// create new DNode copies of all predecessors
			DNode pre[] = null;
			if (n.getIncoming().size() > 0) {
				pre = new DNode[n.getIncoming().size()];
				int i = 0;
				for (Arc a : n.getIncoming()) {
					pre[i++] = new DNode(nameToID.get(a.getSource().getName()), null);
				}
				DNode.sortIDs(pre);
			} else {
			  throw new InvalidModelException(InvalidModelException.EMPTY_PRESET,
			      n, "Node "+n.getName()+" has an empty pre-set.");
			}
			
			// create new DNode d for Node n
			DNode d = new DNode(nameToID.get(n.getName()), pre);
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
			if (n.getOutgoing().size() > 0) {
				d.post = new DNode[n.getOutgoing().size()];

				int i = 0;
				for (Arc a : n.getOutgoing()) {
					d.post[i++] = new DNode(nameToID.get(a.getTarget().getName()), dPostPre);
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
	private DNodeSet buildInitialState(PtNet net) {

		DNodeSet ds = new DNodeSet(properNames.length);

		// create a new DNode for every token on every place in the net
		for (RefMarkedPlace p : net.getInitialMarking().getPlaces()) {
			for (int i=0; i<p.getToken(); i++)
				ds.add(new DNode(nameToID.get(p.getPlace().getName()), new DNode[0]));
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
