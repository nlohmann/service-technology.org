package hub.top.greta.oclets.canonical;

import java.util.Arrays;
import java.util.Iterator;

public class DNode {
	
	// table for translating DNodes and IDs to the original model 
	public static DNodeAS		translationTable;	
	
	// for globally identifying nodes, debugging only
	public static int idGen = 0;
	public int localId = idGen++;

	//	public hub.top.adaptiveSystem.Node origin;
	public short	id;				// unique id of this node
	public DNode  pre[] = null;		// sorted: ids ascending
	public DNode  post[] = null;	// sorted: ids ascending
	
	public boolean	isEvent = false;
	public boolean  isCutOff = false;
	public boolean  isAnti = false;
	
	public int causedBy[] = null;	// remember localIDs of events that generated this event
	
	public DNode (short id, DNode[] pre) {
		//origin = n;
		this.id = id;
		this.pre = sortIDs(pre);

	}
	
	public DNode (short id, int preSize) {
		//origin = n;
		this.id = id;
		this.pre = new DNode[preSize];
	}
	
	@Override
	public boolean equals(Object obj) {
		if (!(obj instanceof DNode))
			return false;
		
		DNode other = (DNode)obj;
		return (this.id == other.id
				&& pre.equals(other.pre));
	}
	
	public void addPostNode(DNode d) {
		if (post == null) {
			post = new DNode[1];
			post[0] = d;
		}
		else {
			post = Arrays.copyOf(post, post.length+1);
			post[post.length-1] = d;
			DNode.sortIDs(post);
		}
	}
	
	/**
	 * @param other
	 * @param depth
	 * 		compare to the given depth, set to <code>-1</code> if full depth
	 * 		shall be compared
	 * @return
	 * 		<code>true</code> iff other (and its predecessors) is a suffix
	 * 		of this node (and its predecessors)
	 */
	public boolean endsWith(DNode other) {
		if (this.id != other.id)
			return false;
		// nodes have equal id
		
		if (other.pre == null)
			// other has no predecessor, this ends with other
			return true;
		// need to compare predecessors
		
		if (this.pre == null && other.pre != null)
			// other has predecessors, this not
			return false;
		// both nodes have predecessors
		
		if (this.pre.length < other.pre.length)
			// this cannot end with other: this node has too few predecessors
			return false;
		
		int i=0;	// index pointing at the current pre-node of this
		// iterate over all pre-nodes of other to find a corresponding
		// matching pre-node of this, pre-nodes are ordered by their ids
		for (int j=0; j<other.pre.length && i<this.pre.length; j++) {
			// the IDs of the predecessors are ordered, increment i to find the
			// predecessor 'i' of this node that has the same ID as the predecessor 'j'
			// of the other node, move 'i' until the IDs are equal
			// or id of 'i' is larger than id of 'j', in the latter case, this node
			// has no matching predecessor 'i' for the predecessor 'j' of other 
			while ( i < this.pre.length && this.pre[i].id < other.pre[j].id ) i++;
			
			if (i == this.pre.length)
				// the ID of the j'th predecessor is higher than any ID of this node's
				// predecessors, no match
				return false;
			
			// this.pre[i].id >= other.pre[j].id
			if (this.pre[i].id == other.pre[j].id) {
				
				if (!this.pre[i].endsWith(other.pre[j]))
					return false;	// not matching
			} else {
				// these nodes 'i' and 'j' have no matching IDs, as all predecessors
				// are ordered by their IDs, there is no further predecessor 'i+k'
				// of this node that can match 'j': this does not end with other
				return false;
			}
		}
		// successfully found that for each pre-node X of other
		// exists a pre-node of this that ends with X
		return true;
	}
	
	public boolean equals(DNode other, int depth) {
		if (this.id != other.id)
			return false;
		// nodes have equal id
		
		if (depth == 0)	// compare only up to this depth, done
			return true;
		
		// check size of predecessors for equality
		if (this.pre == null && other.pre != null)
			return false;
		if (this.pre != null && other.pre == null)
			return false;
		if (this.pre.length != other.pre.length)
			return false;
		
		// predecessors are ordered by their IDs, ascending
		// assume structures where no node has two equally-labeled predecessors
		for (int i=0; i<this.pre.length; i++)
			// compare all predecessors with same index pairwise
			if (!this.pre[i].equals(other.pre[i], depth-1))
				return false;
		return true;
	}
	
	/**
	 * @param other
	 * @return
	 * 		<code>true</code> iff this node has other as its predecessor,
	 * 		i.e. other == pre[i] for some i
	 */
	public boolean preContains_identity(DNode other) {
		// iterate the predecessors of this node
		for (int i=0; i<pre.length; i++) {
			if (pre[i].id > other.id)
				// only up to the last id that could match 
				return false;
			if (pre[i] == other)
				return true;
		}
		return false;
	}
	
	@Override
	public String toString() {
		return translationTable.properNames[this.id]+" ("+this.id+")["+this.localId+"]"; 
	}
	
	/**
	 * Merge two DNode arrays half1 and half2 into a DNode array by sorting
	 * the nodes by their IDs in ascending order. The result is either written
	 * to result[] or, if <code>result == null</code> a new appropriate array
	 * is created.
	 *  
	 * @param result
	 * @param half1
	 * @param half2
	 * @return
	 */
	public static DNode[] merge(DNode result[], DNode half1[], DNode half2[]) {
		if (result == null)
			result = new DNode[half1.length+half2.length];
		
		int j1=0, j2=0;
		int i = 0;
		while (j1 < half1.length && j2 < half2.length) {
			if (half1[j1].id < half2[j2].id)
				result[i++] = half1[j1++];
			else
				result[i++] = half2[j2++];
		}
		while (j1 < half1.length) {
			result[i++] = half1[j1++];
		}
		while (j2 < half2.length) {
			result[i++] = half2[j2++];
		}
		return result;
	}
	
	/**
	 * Merge-sort implementation to sort an array of DNodes by
	 * their IDs in ascending order. Result will be changed in
	 * the argument.
	 * 
	 * @param nodes
	 * 			array of DNodes to order
	 * @return sorted array for convenience
	 */
	public static DNode[] sortIDs(DNode nodes[]) {
		if (nodes == null || nodes.length <= 1) return nodes;	// nothing to sort
		
		int split = nodes.length/2;
		DNode half1[] = new DNode[split];
		DNode half2[] = new DNode[nodes.length-split];
		
		int i=0;
		for (; i<split; i++)
			half1[i] = nodes[i];
		for (int j=0; i<nodes.length; i++, j++)
			half2[j] = nodes[i];
		
		sortIDs(half1);
		sortIDs(half2);
		merge(nodes, half1, half2);
			
		return nodes;
	}
	
	public static class SortedLinearList implements Iterable<DNode> {
		private DNode 			d;
		private SortedLinearList next;
		
		public SortedLinearList() {
			this(null);
		}
		
		public SortedLinearList(DNode node) {
			this(node, null);
		}
		
		public SortedLinearList(DNode node, SortedLinearList rest) {
			this.d = node;
			this.next = rest;
		}

		public SortedLinearList add(DNode node) {
			if (d == null) {	// no value set so far
				d = node;
				return this;
			}
			
			if (node.id < d.id) {	// new node is smaller than current node
				// put new node in front, append remaining list
				SortedLinearList l = new SortedLinearList(node, this);
				return l;	// new list is now rest of the list,
							// taking place of "this"
				
			} else {	// new node is greater/equal compared to current node
				if (next == null) {
					next = new SortedLinearList(node);
				} else {
					// append to successor
					next = next.add(node);	// and store what is returned as
											// new rest of the list
				}
				return this;	// this node is rest of the list
			}
		}
		
		public void removeDuplicateIDs() {
			if (d == null || next == null)
				return;
			
			while (next != null && next.d != null && d.id == next.d.id) {
				// if successor has same id

				SortedLinearList old = next;
				next = next.next;		// drop it from the list
				
				old.next = null;		// and remove references
				old.d = null;			// for garbage collection
			}
			if (next != null)
				next.removeDuplicateIDs();
		}
		
		public int length() {
			if (d == null) return 0;
			if (next == null) return 1;
			return next.length() + 1;
		}

		public Iterator<DNode> iterator() {
			return new Iterator<DNode>() {
				
				// save start element
				private SortedLinearList current = SortedLinearList.this;
			
				public void remove() {
					// not supported
				}
			
				public DNode next() {
					DNode value = current.d;
					current = current.next;
					return value;
				}
			
				public boolean hasNext() {
					return current != null && current.d != null;
				}
			};
		}
		
		@Override
		public String toString() {
			String result = "[";
			for (DNode d : this) {
				result += d+", ";
			}
			return result + "]";
		}
	}
	
	/*
	public static void main (String [] args) {
		short ids_1[] = { 0, 1, 2, 3, 4, 5, 6};
		short ids_2[] = { 067, 14, 4527, 63, -4, 4575, 106};
		
		sortIDs(ids_1);
		sortIDs(ids_2);
		
		for (int i=0;i<ids_1.length;i++)
			System.out.print(ids_1[i]+" ");
		System.out.println();
		
		for (int i=0;i<ids_2.length;i++)
			System.out.print(ids_2[i]+" ");
		System.out.println();
	}
	*/
}
