package hub.top.greta.oclets.canonical;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

public class DNodeSet {
	
	public class DNodeSetElement extends java.util.LinkedList<DNode>  {

	}

	public DNodeSetElement maxNodes = null;
	public DNodeSetElement initialConditions = null;
	
	public DNodeSetElement allConditions = null;
	public DNodeSetElement allEvents = null;
	
	public DNode[] initialCut = null;
	
	public DNodeSet (int num) {
		maxNodes = new DNodeSetElement();	// initialize hash-table size
		allConditions = new DNodeSetElement();
		allEvents = new DNodeSetElement();
	}

	/**
	 * Set the initial conditions of this DNodeSet, this are not
	 * its minimal nodes but the conditions that are maximal before
	 * the first event is fired in this set 
	 */
	public void setInitialConditions () {
		initialConditions = new DNodeSetElement();	// initialize hash-table size
		initialConditions.addAll(maxNodes);
		initialCut = new DNode[initialConditions.size()];
		int i=0;
		for (DNode b : initialConditions)
			initialCut[i++] = b;
		
		DNode.sortIDs(initialCut);
	}
	
	public void add(DNode newNode) {
		
		assert newNode != null : "Error! Trying to insert null node into DNodeSet";
		
		DNodeSetElement toRemove = new DNodeSetElement();
		for (DNode d : maxNodes) {
			if (newNode.preContains_identity(d)) {
				toRemove.add(d);
			}
		}
		maxNodes.removeAll(toRemove);
		maxNodes.add(newNode);
		
		if (newNode.isEvent)
			allEvents.add(newNode);
		else
			allConditions.add(newNode);
	}
	
	public HashSet<DNode> getAllNodes() {
		LinkedList<DNode> queue = new LinkedList<DNode>(maxNodes);
		HashSet<DNode> allNodes = new HashSet<DNode>();
		while (!queue.isEmpty()) {
			DNode n = queue.removeFirst();
			if (n == null) continue;
			allNodes.add(n);
			queue.addAll(Arrays.asList(n.pre));
		}
		return allNodes;
	}
	
	public DNodeSetElement getAllConditions() {
		/*
		LinkedList<DNode> queue = new LinkedList<DNode>(maxNodes);
		HashSet<DNode> allNodes = new HashSet<DNode>();
		while (!queue.isEmpty()) {
			DNode n = queue.removeFirst();
			if (n == null) continue;
			if (!n.isEvent)	// only add conditions
				allNodes.add(n);
			queue.addAll(Arrays.asList(n.pre));
		}
		*/
		return allConditions;
	}
	
	public DNodeSetElement getAllEvents() {
		return allEvents;
	}
	
	
	public DNode[] getPrimeCut(DNode event, DNode.SortedLinearList predecessors) {
		// TODO: compute the cut that is induced by the prime configuration of the event
		
		//DNode.SortedLinearList postConditions = new DNode.SortedLinearList();
		HashSet<DNode> postConditions = new HashSet<DNode>();
		HashSet<DNode> queued = new HashSet<DNode>();
		
		HashSet<DNode> consumedConditions = new HashSet<DNode>();
		LinkedList<DNode> queue = new LinkedList<DNode>();
		queue.add(event);
		queued.add(event);

		while (!queue.isEmpty()) {
			
			DNode first = queue.removeFirst();
			
			// add all post-conditions to the prime-cut that are not consumed by successors
			for (DNode post : first.post) {
				if (!consumedConditions.contains(post))
					postConditions.add(post);
			}
			
			for (DNode preCondition : first.pre)
			{
				// all pre-conditions of this node are consumed
				consumedConditions.add(preCondition);
				
				// add all predecessor events to the queue
				for (DNode preEvent : preCondition.pre) {
					if (!queued.contains(preEvent)) {
						queue.add(preEvent);
						queued.add(preEvent);
						// remember all predecessors of this event
						if (predecessors != null) {
							predecessors = predecessors.add(preEvent);
						}
					}
				}
			}
		}

		// as we are doing simple breadth-first search, we may have added
		// some conditions to the post-conditions that turned out later to
		// be consumed by other events, remove these from the postConditions
		postConditions.addAll(initialConditions);
		postConditions.removeAll(consumedConditions);
		
		DNode[] primeCut = new DNode[postConditions.size()];
		postConditions.toArray(primeCut);
		DNode.sortIDs(primeCut);
		
		return primeCut;
	}
	
	/**
	 * Fire ocletEvent in this set at the given location. This
	 * appends a new event with the same id as ocletEvent to
	 * this set, and corresponding post-conditions 
	 * 
	 * @param ocletEvent
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode ocletEvent, DNode[] fireLocation) {

		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvent.id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// remember the oclet event that caused this node
		newEvent.causedBy = new int[1];
		newEvent.causedBy[0] = ocletEvent.localId;
		
		// set post-node of conditions
		for (DNode preNode : fireLocation)	
			preNode.addPostNode(newEvent);
		
		// instantiate the post-conditions of the oclet event
		DNode postConditions[] = new DNode[ocletEvent.post.length];
		for (int i=0; i<postConditions.length; i++) {
			postConditions[i] = new DNode(ocletEvent.post[i].id, 1);
			allConditions.add(postConditions[i]);

			// predecessor is the new event
			postConditions[i].pre[0] = newEvent;

			assert postConditions[i] != null : "Error, created null post-condition!";
		}
		
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		// add post-set of newEvent to the maxNodes
		maxNodes.addAll(Arrays.asList(postConditions));
		return postConditions;
	}
	
	/**
	 * Synchronously fire a set of ocletEvents in this set at
	 * the given location. This appends ONE new event with the
	 * same id as all ocletEvents to this set, and corresponding
	 * post-conditions for all the given ocletEvents 
	 * 
	 * @param ocletEvent
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode[] ocletEvents, DNode[] fireLocation) {
		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvents[0].id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// set post-node of conditions
		for (DNode preNode : fireLocation)
			preNode.addPostNode(newEvent);
		
		newEvent.causedBy = new int[ocletEvents.length];
		
		// create a sorted list of all post-conditions of all events
		DNode.SortedLinearList list = new DNode.SortedLinearList();
		for (int i=0; i<ocletEvents.length; i++) {
			
			// remember the oclet events that caused this node
			newEvent.causedBy[i] = ocletEvents[i].localId;
			
			for (int j=0; j<ocletEvents[i].post.length; j++)
				list = list.add(ocletEvents[i].post[j]);
		}
		
		// remove duplicate post-conditions (these are merged)
		list.removeDuplicateIDs();
		
		// instantiate the remaining post-conditions of the new oclet event
		DNode postConditions[] = new DNode[list.length()];
		int i = 0;
		for (DNode post : list) {
			postConditions[i] = new DNode(post.id, 1);
			allConditions.add(postConditions[i]);

			// predecessor is the new event
			postConditions[i].pre[0] = newEvent;
			
			assert postConditions[i] != null : "Error, created null post-condition!";
			
			i++;
		}
		
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		// add post-set of newEvent to the maxNodes
		maxNodes.addAll(Arrays.asList(postConditions));
		return postConditions;
	}
	
	/*
	public void fire(LinkedList<DNode> ocletEvents, DNode[] fireLocation) {
		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvents.get(0).id, fireLocation);
		newEvent.isEvent = true;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		
		// collect all post conditions of all events
		DNode[] mergedPostConditions = new DNode[0];
		for (DNode ocletEvent : ocletEvents)
			mergedPostConditions = DNode.merge(null, mergedPostConditions, ocletEvent.post);
		
		// instantiate one condition for each ID and add it
		// as post-condition of newEvent to the maxNodes
		int lastID = -1;
		for (int i=0; i<mergedPostConditions.length; i++) {
			if (mergedPostConditions[i].id == lastID)
				continue;
			DNode postCondition = new DNode(mergedPostConditions[i].id, 1);
			// predecessor is the new event
			postCondition.pre[0] = newEvent;
			maxNodes.add(postCondition);
		}
	}
	*/
	
	/*
	public boolean contains(DNode n) {
		
		if (maxNodes[n.id] == null)
			return false;
		else {
			return contents[n.id].contains(n);
		}
	}
	*/
	
	public static boolean option_printAnti = true;
	
	public String toDot () {
		StringBuilder b = new StringBuilder();
		b.append("digraph BP {\n");
		
		b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
		b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
		b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

		String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
		String cutOffFillString = "fillcolor=grey";
		String antiFillString = "fillcolor=red";
		
		//HashSet<CNode> allNodes = nodes;
		
		HashSet<DNode> allNodes = new HashSet<DNode>();
		LinkedList<DNode> queue = new LinkedList<DNode>();
		queue.addAll(maxNodes);
		while (queue.size() > 0) {
			DNode d = queue.removeFirst();
			if (d == null) continue;
			allNodes.add(d);
			queue.addAll(Arrays.asList(d.pre));
		}
		
		b.append("\n\n");
		b.append("node [shape=circle];\n");
		for (DNode n : allNodes) {
			if (n.isEvent)
				continue;
			
			if (!option_printAnti && n.isAnti)
				continue;
			/* - print current marking
			if (cutNodes.contains(n))
				b.append("  c"+n.localId+" ["+tokenFillString+"]\n");
			else
			*/
			if (n.isAnti)
				b.append("  c"+n.localId+" ["+antiFillString+"]\n");
			else
				b.append("  c"+n.localId+" []\n");
				
			b.append("  c"+n.localId+"_l [shape=none];\n");
			b.append("  c"+n.localId+"_l -> c"+n.localId+" [headlabel=\""+n+"\"]\n");
		}
		
		b.append("\n\n");
		b.append("node [shape=box];\n");
		for (DNode n : allNodes) {
			if (!n.isEvent)
				continue;
			
			if (!option_printAnti && n.isAnti)
				continue;
			
			
			if (n.isAnti)
				b.append("  e"+n.localId+" ["+antiFillString+"]\n");
			else if (n.isCutOff)
				b.append("  e"+n.localId+" ["+cutOffFillString+"]\n");
			else
				b.append("  e"+n.localId+" []\n");
			b.append("  e"+n.localId+"_l [shape=none];\n");
			b.append("  e"+n.localId+"_l -> e"+n.localId+" [headlabel=\""+n+"\"]\n");
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
		
		b.append("\n\n");
		b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
		for (DNode n : allNodes) {
			String prefix = n.isEvent ? "e" : "c";
			for (int i=0; i<n.pre.length; i++) {
				if (n.pre[i] == null) continue;
				if (!option_printAnti && n.isAnti) continue;
				
				if (n.pre[i].isEvent)
					b.append("  e"+n.pre[i].localId+" -> "+prefix+n.localId+" [weight=10000.0]\n");
				else
					b.append("  c"+n.pre[i].localId+" -> "+prefix+n.localId+" [weight=10000.0]\n");
			}
		}
		b.append("}");
		return b.toString();
	}
}
