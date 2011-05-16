package hub.top.mia;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;

public class StateTree {
	private SafeMarking root;
	
	public StateTree() {
		root = new SafeMarking();
		root.setDepth(0);
	}
	
	/**
	 * build state tree
	 */
	public void buildTree(ArrayList<SafeMarking> global_states) {
		// compute all intersections
		ArrayList<SafeMarking> clusters = new ArrayList<SafeMarking>();
		for (int i = 0; i<global_states.size(); i++) {
			for (int j = 0; j<i; j++) {
				SafeMarking inner = global_states.get(i).intersect(global_states.get(j));
				
				// check if inner is already
				SafeMarking new_inner = null; 
				for (SafeMarking cluster : clusters) {
					if (cluster.equals(inner)) {
						new_inner = cluster;
					}
				}
				
				// if inner is already in the clusters set, add leafs new global states
				if (new_inner != null) {
					new_inner.addLeaf(global_states.get(i));
					new_inner.addLeaf(global_states.get(j));
				} else {
					// add new inner to the cluster set
					inner.addLeaf(global_states.get(i));
					inner.addLeaf(global_states.get(j));
					clusters.add(inner);
				}		
			}
		}
		
		// sort clusters in descending order
		SafeMarking[] arrayClusters = new SafeMarking[clusters.size()];
		MarkingComparator comp = new MarkingComparator();
		clusters.toArray(arrayClusters);
		Arrays.sort(arrayClusters, comp);
		
		/*
		System.out.println("-----------");
		for (SafeMarking cluster : arrayClusters) {
			System.out.println(cluster);
		}*/
		
		for (SafeMarking cluster : arrayClusters) {
			// iterate over all leafs
			HashSet<SafeMarking> leafs = cluster.getLeafs();
			for (SafeMarking leaf : leafs) {
				HashSet<SafeMarking> fathers = leaf.getFathers();
				
				if (fathers.isEmpty()) {
					leaf.addFather(cluster);
				} else {
					// compute intersection between all fathers of all leafs 
					SafeMarking temp = leaf;
					for (SafeMarking father : fathers) {
						if (cluster.contains(father) && comp.compare(father, temp) > 0) {
							temp = father;
						}
					}
					
					// add link between smallest father and cluster
					if (!temp.equals(leaf)) {
						cluster.addChild(temp);
						temp.addFather(cluster);
					}
				}
			}	
		}
		
		// add clusters with no fathers as children of root
		for (SafeMarking cluster : arrayClusters) {
			if (cluster.getFathers().isEmpty()) {
				root.addChild(cluster);
				cluster.addFather(root);
			}
		}
	}
	
	/**
	 * breadth first search traversal of the state tree
	 */
	public void traverseTree() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);
		
		do {
			SafeMarking current = queue.poll();
			
			// output current node
			System.out.println(current + " d:" + current.getDepth());
			
			// push children in the queue
			HashSet<SafeMarking> children = current.getChildren();
			for (SafeMarking child : children) {
				child.setDepth(current.getDepth() + 1);
				queue.add(child);
			}
			
		} while (!queue.isEmpty());
	}
}
