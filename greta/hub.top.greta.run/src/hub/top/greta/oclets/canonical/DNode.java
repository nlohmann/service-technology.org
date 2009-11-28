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

import java.util.Collection;
import java.util.Iterator;

/**
 * A node in a branching process. It has a name and possibly empty set of
 * predecessor nodes.
 * 
 * @author Dirk Fahland
 *
 */
public class DNode {
	
	// global table for translating DNodes and IDs to the original model 
	public static DNodeSys		translationTable;	
	
	// each node has a unique global ID, the global member idGen holds the
	// next ID for the next new node
	public static int idGen = 0;

	// the node's global ID, automatically assign latest ID upon construction
	public int globalId = idGen++;

	public short	id;				      // id of this node (its name)
	public DNode  pre[] = null;		// sorted: ids ascending
	public DNode  post[] = null;	// sorted: ids ascending
	
	public boolean	isEvent = false;
	public boolean  isCutOff = false;
	public boolean  isAnti = false;
	
	/**
	 * For finding cutOff events using the signature method, we compare whether
	 * two events in the branching process are fired by the same set of oclet
	 * events (see {@link DNodeBP#equivalentCuts_signature}). This array stores
	 * the globalIDs of the oclet events that were fired for creating this event.
	 * Is only used for events. 
	 */
	public int causedBy[] = null;	// remember globalIDs of events that generated this event
	
	/**
	 * New node with known predecessors nodes. Sorts the precedessor array by
	 * their ids.
	 * @param id   of the new node
	 * @param pre
	 */
	public DNode (short id, DNode[] pre) {
		//origin = n;
		this.id = id;
		this.pre = sortIDs(pre);

	}
	
	/**
	 * New node with unknown predecessors.
	 * @param id       of the new node
	 * @param preSize  number of predecessors of the new node
	 */
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
	
	/**
	 * Adds new successor to this node and sorts successors by their ids.
	 * Dynamically enlarges array of successors by re-initialization and
	 * copying.
	 * 
	 * @param d
	 */
	public void addPostNode(DNode d) {
		if (post == null) {
			post = new DNode[1];
			post[0] = d;
		}
		else {
			DNode[] newPost = new DNode[post.length+1];
			for (int i=0; i<post.length; i++) newPost[i] = post[i];
			newPost[newPost.length-1] = d;
			post = DNode.sortIDs(newPost);
		}
	}
	
	 /**
   * Adds new predecessor to this node and sorts predecessor by their ids.
   * Dynamically enlarges array of predecessor by re-initialization and
   * copying.
   * 
   * This method should not be used during the standard branching process
   * construction. Rather initialize the size of the predecessor array in the
   * constructor.
   * 
   * @param d
   */
  public void addPreNode(DNode d) {
    if (pre == null) {
      pre = new DNode[1];
      pre[0] = d;
    }
    else {
      DNode[] newPre = new DNode[pre.length+1];
      for (int i=0; i<pre.length; i++) newPre[i] = pre[i];
      newPre[newPre.length-1] = d;
      pre = DNode.sortIDs(newPre);
    }
  }
	
	/**
	 * Check whether this node's history contains a partial history that is
	 * isomorphic to the entire history of the other node.
	 * 
	 * @param other
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
	
	/**
	 * Check whether the history of this node is isomorphic to the history
	 * of the other node up to the given depth.
	 * 
	 * @param other
	 * @param depth
	 * @return
	 */
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
	
	/*
	 * (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return translationTable.properNames[this.id]+" ("+this.id+")["+this.globalId+"]"; 
	}

	
	/* =======================================================================

	     static util functions
	     
	   ======================================================================= */
	
	/* ------------------------------ merge sort ----------------------------- */ 
	
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
	
	/* ------------------ data structure for collecting DNodes ------------- */
	
	/**
	 * Sorted linear list of {@link DNode}s. The nodes are sorted by their
	 * {@link DNode#id}.
	 */
	public static class SortedLinearList implements Iterable<DNode> {
		private DNode 			d;
		private SortedLinearList next;
		
		/**
		 * new empty list
		 */
		public SortedLinearList() {
			this(null);
		}
		
		/**
		 * new list with a new node
		 * @param node
		 */
		public SortedLinearList(DNode node) {
			this(node, null);
		}
		
		/**
		 * new list with head <code>node</code> and tail <code>rest</code>  
		 * @param node
		 * @param rest
		 */
		public SortedLinearList(DNode node, SortedLinearList rest) {
			this.d = node;
			this.next = rest;
		}

		/**
		 * insert <code>node</code> into list
		 * @param node
		 * @return new head of list
		 */
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
		
		/**
		 * Remove nodes from the list. For each {@link DNode#id} keep only the first
		 * node with this id.
		 */
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
		
		/**
		 * @return length of the list
		 */
		public int length() {
			if (d == null) return 0;
			if (next == null) return 1;
			return next.length() + 1;
		}

		/**
		 * Standard iterator for {@link DNode.SortedLinearList}. Cannot remove
		 * elements from the list via this iterator. 
		 */
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
		
		/*
		 * (non-Javadoc)
		 * @see java.lang.Object#toString()
		 */
		@Override
		public String toString() {
			String result = "[";
			for (DNode d : this) {
				result += d+", ";
			}
			return result + "]";
		}
	}
	
	
  /* ---------------------------- misc. util functions ---------------------- */
	
	/**
	 * Convert a Java {@link Collection} into an array of {@link DNode}.
	 * The array is filled in the order that is provided by the collections
	 * iterator.
	 * 
	 * @param nodes
	 * @return
	 */
	public static DNode[] asArray(Collection<DNode> nodes) {
	  DNode[] result = new DNode[nodes.size()];
	  int i = 0;
	  for (DNode d : nodes)
	    result[i++] = d;
	  return result;
	}
	
}
