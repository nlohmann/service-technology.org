package hub.top.greta.oclets.canonical;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.OccurrenceNet;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Orientation;
import hub.top.adaptiveSystem.PreNet;

public class DNodeAS {

	public String				properNames[];		// translate IDs to Names
	public Map<String, Short>	nameToID;
	
	private int					nodeNum; 
	public DNode.SortedLinearList fireableEvents;	// set of all contributed oclet events
	public DNode.SortedLinearList preConEvents;		// set of all precondition events
	public Map<Node, DNode>		nodeEncoding;		// encoding of oclet Nodes as DNodes
	
	public Map<Oclet, DNodeSet>	ocletEncoding;
	public DNodeSet				apEncoding;
	
	public int					maxHistoryDepth;	// depth of the longest history of a transition 
	
	public DNodeAS(AdaptiveSystem as) {
		
		DNode.idGen = 0;	// reset IDs
		
		fireableEvents = new DNode.SortedLinearList();
		preConEvents = new DNode.SortedLinearList();
		
		buildTranslationTable(as);
		nodeEncoding = new HashMap<Node, DNode>(nodeNum);
		ocletEncoding = new HashMap<Oclet, DNodeSet>(as.getOclets().size());
		
		maxHistoryDepth = -1;
		
		DNode.translationTable = this;		// all happens wrt. to this class
		
		for (Oclet o : as.getOclets())
			ocletEncoding.put(o, buildDNodeRepresentation(o));
		apEncoding = buildDNodeRepresentation(as.getAdaptiveProcess());
	}
	
	private void buildTranslationTable(AdaptiveSystem as) {
		short currentNameID = 0;
		nameToID = new HashMap<String, Short>();
		
		nodeNum = 0;
		
		// collect all names and assign each new name a new ID
		for (Node n : as.getAdaptiveProcess().getNodes()) {
			if (nameToID.get(n.getName()) == null)
				nameToID.put(n.getName(), currentNameID++);
			nodeNum++;
		}
		for (Oclet o : as.getOclets()) {
			for (Node n : o.getPreNet().getNodes()) {
				if (nameToID.get(n.getName()) == null)
					nameToID.put(n.getName(), currentNameID++);
				nodeNum++;
			}
			for (Node n : o.getDoNet().getNodes()) {
				if (nameToID.get(n.getName()) == null)
					nameToID.put(n.getName(), currentNameID++);
				nodeNum++;
			}
		}
		
		// build the translation table from IDs to names
		properNames = new String[currentNameID];
		for (Entry<String,Short> line : nameToID.entrySet()) {
			properNames[line.getValue()] = line.getKey();
		}
	}
	
	private Collection<DNode> translateToDNodes(OccurrenceNet o, boolean isAnti) {
		LinkedList<DNode> maxNodes = new LinkedList<DNode>();
		LinkedList<Node> searchQueue = new LinkedList<Node>();
		
		Map<Node, Integer> depth = new HashMap<Node, Integer>();
		
		// fill the queue with all maximal nodes of the oclet
		for (Node n : o.getNodes()) {
			if (n instanceof Event) {
				Event e = (Event)n;
				if (e.getPostConditions().isEmpty()) {
					searchQueue.add(n);
					
					// we compute the length of the longest path that ends in an
					// event, if this event has no predecessor, the corresponding
					// path has length 0
					depth.put(n, 0);
				}
			}			
			if (n instanceof Condition) {
				Condition b = (Condition)n;
				if (b.getPostEvents().isEmpty()) {
					searchQueue.add(n);
					
					// we compute the length of the longest path that ends in an
					// event, a path that ends in the pre-event of this condition
					// has one edge lass than a path that ends in this condition
					depth.put(n, -1);	
				}
			}
		}

		// then do a breath-first search on the structure of the oclet
		// and add each visited node to the buildStack, the same node
		// can be added multiple times
		LinkedList<Node> buildStack = new LinkedList<Node>();
		while (!searchQueue.isEmpty()) {
			Node n = searchQueue.removeFirst();
			
			int n_depth = depth.get(n);
			
			if (n instanceof Event) {
				Event e = (Event)n;
				for (Node post : e.getPreConditions()) {
					searchQueue.addLast(post);
					
					Integer d = depth.get(post);
					if (d == null || d <= n_depth) {
						depth.put(post, n_depth+1);	// increment depth of node
						// and remember maximum depth of a node
						maxHistoryDepth = Math.max(n_depth+1, maxHistoryDepth);
					}
				}
			} else {
				Condition b = (Condition)n;
				for (Node post : b.getPreEvents()) {
					searchQueue.addLast(post);
					
					Integer d = depth.get(post);
					if (d == null || d <= n_depth) {
						depth.put(post, n_depth+1);	// increment depth
						// and remember maximum depth of a node
						maxHistoryDepth = Math.max(n_depth+1, maxHistoryDepth);
					}
				}
			}
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
			
			LinkedList<Node> preNodes = new LinkedList<Node>();
			int postSize;
			if (n instanceof Event) {
				Event e = (Event)n;
				preNodes.addAll(e.getPreConditions());
				postSize = e.getPostConditions().size();
			} else {
				Condition b = (Condition)n;
				preNodes.addAll(b.getPreEvents());
				postSize = b.getPostEvents().size();
			}
			
			// save the predecessors of node n in an array
			DNode pre[] = new DNode[preNodes.size()];
			for (int i=0; i<pre.length; i++) {
				pre[i] = nodeEncoding.get(preNodes.get(i));
			}
			
			// create new DNode d for Node n
			DNode d = new DNode(nameToID.get(n.getName()), pre);
			if (n instanceof Event) {
				d.isEvent = true;
				// remember all events of a DoNet: these will be fired if enabled
				if (n.eContainer() instanceof DoNet)
					fireableEvents = fireableEvents.add(d);
				else if (n.eContainer() instanceof PreNet)
					preConEvents = preConEvents.add(d);
			}
			d.isAnti = isAnti;
			
			// initialize post-set of new DNode d
			if (postSize > 0) {
				d.post = new DNode[postSize];
				for (int i=0; i<postSize; i++)
					d.post[i] = null;
			} else {
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
		}
		return maxNodes;
	}
	
	private DNodeSet buildDNodeRepresentation(Oclet o) {
		
		Collection<DNode> maxNodesOfO = translateToDNodes(o.getDoNet(), o.getOrientation() == Orientation.ANTI);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : maxNodesOfO)
			ds.add(d);
		return ds;
	}
	
	private DNodeSet buildDNodeRepresentation(AdaptiveProcess ap) {
		
		Collection<DNode> maxNodesOfAP = translateToDNodes(ap, false);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : maxNodesOfAP)
			ds.add(d);
		return ds;
	}
}
