package hub.top.mia;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.BitSet;

public class StateTree {
	private SafeMarking root;
	private MarkingComparator comp;
	private SafeMarking successor;

	/** some constants **/
	public static final int INTERSECTION_EMPTY = 0;
	public static final int INTERSECTION_SINGLE = 1;
	public static final int INTERSECTION_MORE = 2;

	public StateTree() {
		root = new SafeMarking();
		root.setDepth(0);
		comp = new MarkingComparator();
	}

	

	/**
	 * find smallest ancestor that contains cluster
	 * TODO: change successor procedure, for each father, find ancestor that contains cluster
	 * @param current
	 * @param cluster
	 */
	public void findSuccessor(SafeMarking current, SafeMarking cluster) {
		HashSet<SafeMarking> fathers = current.getFathers();

		System.out.println("Current: " + current + " " + cluster);
		if (fathers.isEmpty()) {
			System.out.println("Fathers is empty! ");
			return;
		}

		for (SafeMarking father : fathers) {
			if (father.contains(cluster) && comp.compare(father, successor) > 0
					&& !cluster.equals(father)) {
				successor = father;
				System.out.println("Current: " + successor + " " + cluster);
				findSuccessor(father, cluster);
			}
		}
	}

	/**
	 * build state tree
	 */
	public void buildTree(ArrayList<SafeMarking> global_states) {
		// compute all intersections
		ArrayList<SafeMarking> clusters = new ArrayList<SafeMarking>();
		for (int i = 0; i < global_states.size(); i++) {
			for (int j = 0; j < i; j++) {
				SafeMarking inner = global_states.get(i).intersect(
						global_states.get(j));

				// check if inner is already
				SafeMarking new_inner = null;
				for (SafeMarking cluster : clusters) {
					if (cluster.equals(inner)) {
						new_inner = cluster;
					}
				}

				// if inner is already in the clusters set, add leafs new global
				// states
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

		//System.out.println("-----------");
		//for (SafeMarking cluster : arrayClusters) {
			//System.out.println(cluster);
		//}

		// iterate over all clusters in descending order
		for (SafeMarking cluster : arrayClusters) {

			System.out.println("Current cluster: " + cluster);

			HashSet<SafeMarking> leafs = cluster.getLeafs();
			HashSet<SafeMarking> successors = new HashSet<SafeMarking>();

			// iterate over all leafs
			for (SafeMarking leaf : leafs) {

				System.out.println("Leaf: " + leaf);
				// get fathers of leaf
				HashSet<SafeMarking> fathers = leaf.getFathers();

				if (fathers.isEmpty()) {
					//System.out.println("Leaf Link: " + leaf + " " + cluster);
					leaf.addFather(cluster);
				} else {
					// compute intersection between all fathers of all leafs
					successor = leaf;
					findSuccessor(leaf, cluster);

					System.out.println("After find successor: " + successor
							+ " " + cluster);

					if (!successor.equals(leaf)) {
						System.out.println("Inner Link: " + successor + " "
								+ cluster);
						cluster.addChild(successor);
						successor.addFather(cluster);
						successors.add((SafeMarking) successor.clone());
					} else {
						// add leaf link
						leaf.addFather(cluster);
						//System.out.println("Leaf Link: " + leaf + " " + cluster);
					}
				}
			}
			
			// merge leafs of all successors with leafset of current node
			for (SafeMarking node : successors) {
				cluster.mergeLeafs(node);
			}
			
		}

		// check if we have empty intersection already
		if (arrayClusters[arrayClusters.length - 1].isEmpty()) {
			root = arrayClusters[arrayClusters.length - 1];
		} else {
			// add clusters with no fathers as children of root
			for (SafeMarking cluster : arrayClusters) {
				if (cluster.getFathers().isEmpty()) {
					root.addChild(cluster);
					cluster.addFather(root);
				}
			}
		}
	}

	/**
	 * compute intersection between leaf set of cluster1 and leaf set of cluster
	 * 2
	 * 
	 * @param cluster1
	 * @param cluster2
	 */
	private HashSet<SafeMarking> computeIntersection(
			HashSet<SafeMarking> leafs1, HashSet<SafeMarking> leafs2) {
		HashSet<SafeMarking> result = new HashSet<SafeMarking>();

		for (SafeMarking leaf1 : leafs1) {
			if (leafs2.contains(leaf1)) {
				result.add(leaf1);
			}
		}

		/*
		 * System.out.println("[Intermediate]: Common intersection:"); for
		 * (SafeMarking node : result) { System.out.println(node); }
		 */

		return result;
	}

	/**
	 * compute intersection between all subsets of clusters array
	 * 
	 * @param clusters
	 */
	private void computeIntersections(ArrayList<SafeMarking> clusters) {
		System.out.println("Processing Clusters...");

		// generate all subsets of clusters
		int clustersSize = clusters.size();
		int noBits = 1;
		BitSet generator = new BitSet();
		generator.set(0);
		HashSet<SafeMarking> validClusters = new HashSet<SafeMarking>();

		while (!generator.get(clustersSize)) {
			System.out.println(generator);

			HashSet<SafeMarking> result = clusters.get(0).getLeafs();
			for (int i = 0; i < noBits; i++) {
				if (generator.get(i) == true) {
					SafeMarking cluster = clusters.get(i);
					result = computeIntersection(result, cluster.getLeafs());
					validClusters.add(cluster);
				}
			}

			if (result.size() == StateTree.INTERSECTION_SINGLE) {
				// we have a candidate!
				for (SafeMarking cluster : validClusters) {
					cluster.setDecomposed(true);
				}
			}

			System.out.println("Common intersection:");
			for (SafeMarking node : result) {
				System.out.println(node);
			}
			System.out.println();

			// generate next subset, simulate addition with 1
			int i = 0;
			while (generator.get(i) == true) {
				i++;
			}

			// if all noBits are set, increase noBits
			if (i == noBits) {
				generator.clear();
				noBits++;
			}
			generator.set(i);

			// reset clusters
			validClusters.clear();
		}
		System.out.println();
	}

	/**
	 * breadth first search traversal of the state tree
	 * compute intersections between all possible subtrees having the same depth
	 */
	public void traverseTree() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);

		// array with all clusters having same depth
		ArrayList<SafeMarking> currentClusters = new ArrayList<SafeMarking>();
		int currentDepth = 0;
		do {
			SafeMarking current = queue.poll();

			// check if depth is increased, if so, compute intersections
			if (currentDepth < current.getDepth()) {
				// compute intersections between clusters
				computeIntersections(currentClusters);

				// reset clusters array
				currentClusters.clear();
				currentDepth++;
			}
			currentClusters.add(current);

			// output current node
			// System.out.println(current + " d:" + current.getDepth());

			// push children in the queue
			HashSet<SafeMarking> children = current.getChildren();
			for (SafeMarking child : children) {
				child.setDepth(current.getDepth() + 1);
				queue.addLast(child);
			}

		} while (!queue.isEmpty());

		// call compute intersections for last set of clusters
		computeIntersections(currentClusters);
	}

	/**
	 * traverse tree in breadth first search order remove subtrees having the
	 * same set of leafs
	 */
	public void minimizeTree() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);

		// array with all clusters having same depth
		ArrayList<SafeMarking> currentClusters = new ArrayList<SafeMarking>();
		int currentDepth = 0;
		root.setDepth(0);

		do {
			SafeMarking current = queue.poll();

			// check if depth is increased, if so, compute intersections
			if (currentDepth < current.getDepth()) {
				// compute intersections between clusters
				removeDuplicates(currentClusters);

				// reset clusters array
				currentClusters.clear();
				currentDepth++;
			}
			currentClusters.add(current);

			HashSet<SafeMarking> children = current.getChildren();
			for (SafeMarking child : children) {
				queue.add(child);
			}

		} while (!queue.isEmpty());

	}

	/**
	 * TODO: implement remove duplicates
	 * @param currentClusters
	 */
	private void removeDuplicates(ArrayList<SafeMarking> currentClusters) {
		/*
		 * for (int i = 0; i<currentClusters.size(); i++) { for (int j = 0; ) }
		 */

	}

	/**
	 * breadth first search traversal of the state tree
	 * if direct successors of tree can be further refined, go further, otherwise add it to the final result
	 */
	public void gatherResult() {
		// TODO: implement gather results
		// traverse tree in breadth first search order

	}
}
