/*****************************************************************************\
 * Copyright (c) 2008-2011 All rights reserved. Dirk Fahland. AGPL3.0
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

import java.util.Collection;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

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
	public final int globalId = idGen++;

	public final short	id;				      // id of this node (its name)
	public DNode  pre[] = null;		// sorted: ids ascending
	public DNode  post[] = null;	// sorted: ids ascending
	
	public boolean	isEvent = false;
	public boolean  isCutOff = false;
	public boolean  isAnti = false;
	public boolean  isHot = false;
	public boolean  isImplied = false;  // event of an implied scenario
	public boolean  ignore = false;
	
	/**
	 * field set during construction of the branching process, a node is new
	 * if it has just been added to the branching process. It becomes old after
	 * all possible extensions in the next step are computed.
	 */
	public boolean  _isNew = true; 
	
	/**
	 * For finding cutOff events using the signature method, we compare whether
	 * two events in the branching process are fired by the same set of oclet
	 * events (see {@link DNodeBP#equivalentCuts_signature}). This array stores
	 * the globalIDs of the oclet events that were fired for creating this event.
	 * Is only used for events. 
	 */
	public int causedBy[] = null;	// remember globalIDs of events that generated this event
	
	 /**
   * New node with a single known predecessors node.
   * 
   * @param id   of the new node
   * @param pre  predecessor node
   */
  public DNode (short id, DNode pre) {
    //origin = n;
    this.id = id;
    this.pre = new DNode[]{pre};
  }
	
	/**
	 * New node with known predecessors nodes. Sorts the precedessor array by
	 * their ids.
	 * @param id   of the new node
	 * @param pre
	 */
	public DNode (short id, DNode[] pre) {
//System.out.println("create "+id+" globaL "+globalId);
		this.id = id;
		this.pre = sortIDs(pre);
	}
	
	/**
	 * New node with unknown predecessors.
	 * @param id       of the new node
	 * @param preSize  number of predecessors of the new node
	 */
	public DNode (short id, int preSize) {
//System.out.println("create "+id+" globaL "+globalId);
		this.id = id;
		this.pre = new DNode[preSize];
	}
	
	@Override
	public boolean equals(Object obj) {
	  /*
		if (!(obj instanceof DNode))
			return false;
		
		DNode other = (DNode)obj;
		return (this.id == other.id
				&& pre.equals(other.pre));
		*/
	  return this == obj;
	}
	
	/**
	 * Comparative equals methods for comparing two DNodes
	 * for structural equality.
	 * 
	 * @param other
	 * @return
	 */
	public boolean structuralEquals(DNode other) {
	  if (this.id != other.id) return false;
	  if (this.pre == null && other.pre == null) return true;
	  if (this.pre == null && other.pre != null || this.pre != null && other.pre == null) return false;
	  if (this.pre.length != other.pre.length) return false;
	  for (int i=0; i<this.pre.length; i++) {
	    if (!this.pre[i].structuralEquals(other.pre[i])) return false;
	  }
	  return true;
	}
	
	/**
	 * Adds new successor to this node and sorts successors by their ids.
	 * Dynamically enlarges array of successors by re-initialization and
	 * copying.
	 * 
	 * @param d
	 */
	public void addPostNode(DNode d) {
		if (post == null || post.length == 0) {
			post = new DNode[1];
			post[0] = d;
		}
		else {
		  /*
			DNode[] newPost = new DNode[post.length+1];
			for (int i=0; i<post.length; i++) newPost[i] = post[i];
			newPost[newPost.length-1] = d;
			post = DNode.sortIDs(newPost);
			*/
		  int pos_min = 0;
		  int pos_max = post.length;
		  int mid = post.length;
		  do {
		    mid = (pos_min + pos_max) / 2;
		    if (post[mid].id > d.id) {
		      // mid is larger than d, d must be left of mid, search between min and mid
		      pos_max = mid-1;
		    } else {
		      pos_min = mid+1;
		    }
		  } while (post[mid].id != d.id && pos_min < pos_max);
		  if (post[mid].id < d.id) mid++;
		  
		  DNode[] newPost = new DNode[post.length+1];
		  for (int i=0; i<mid; i++) {
		    newPost[i] = post[i];
		  }
		  newPost[mid] = d;
		  for (int i=mid+1,j=mid; j<post.length; i++,j++) {
		    newPost[i] = post[j];
		  }
		  post = newPost;
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
   * @return the set of all (transitive) predecessor of this node 
   */
  public Set<DNode> getAllPredecessors() {
    Set<DNode> config = new HashSet<DNode>();
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.add(this);
    config.add(this);
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      if (d.pre == null) continue;
      for (DNode dPre : d.pre) {
        if (!config.contains(dPre)) {
          config.add(dPre);
          queue.addLast(dPre);
        }
      }
    }
    return config;
  }
	
	/**
	 * Check whether this node's history is a suffix of the history
	 * of another node.
	 * 
	 * @param complete
	 * @return
	 * 		<code>true</code> iff this node (and its predecessors) is a suffix
	 * 		of node complete (and its predecessors)
	 */
	public boolean suffixOf(DNode complete) {
	  DNode suffix = this;
		if (complete.id != suffix.id)
			return false;
		// nodes have equal id
		
		if (suffix.pre == null)
			// suffix has no predecessor, this ends with other
			return true;
		// need to compare predecessors
		
		if (complete.pre == null && suffix.pre != null)
			// suffix has predecessors, complete not
			return false;
		// both nodes have predecessors
		
		if (complete.pre.length < suffix.pre.length)
			// complete cannot end with suffix: complete node has too few predecessors
			return false;
		
		int i=0;	// index pointing at the current pre-node of complete
		// iterate over all pre-nodes of suffix to find a corresponding
		// matching pre-node of complete, pre-nodes are ordered by their ids
		for (int j=0; j<suffix.pre.length && i<complete.pre.length; j++) {
			// the IDs of the predecessors are ordered, increment i to find the
			// predecessor 'i' of this node that has the same ID as the predecessor 'j'
			// of the other node, move 'i' until the IDs are equal
			// or id of 'i' is larger than id of 'j', in the latter case, complete node
			// has no matching predecessor 'i' for the predecessor 'j' of suffix 
			while ( i < complete.pre.length && complete.pre[i].id < suffix.pre[j].id ) i++;
			
			if (i == complete.pre.length)
				// the ID of the j'th predecessor is higher than any ID of complete node's
				// predecessors, no match
				return false;
			
			// complete.pre[i].id >= suffix.pre[j].id
			if (complete.pre[i].id == suffix.pre[j].id) {
				
				if (!suffix.pre[j].suffixOf(complete.pre[i]))
					return false;	// not matching
			} else {
				// these nodes 'i' and 'j' have no matching IDs, as all predecessors
				// are ordered by their IDs, there is no further predecessor 'i+k'
				// of this node that can match 'j': complete does not end with suffix
				return false;
			}
		}
		// successfully found that for each pre-node X of suffix
		// exists a pre-node of complete that ends with X
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
		return "'"+translationTable.properNames[this.id]+"' ("+this.id+")["+this.globalId+"]" + (isAnti ? "-" : ""); 
	}

  /*
   * (non-Javadoc)
   * @see java.lang.Object#toString()
   */
  public String toString(DNodeSys translationTable) {
    return "'"+translationTable.properNames[this.id]+"' ("+this.id+")["+this.globalId+"]" + (isAnti ? "-" : ""); 
  }

	
	/* =======================================================================

	     static util functions
	     
	   ======================================================================= */
	
  /**
   * @param first
   * @param second
   * @return <code>true</code> iff the nodes in <code>first</code>
   * are also nodes in <code>second</code> by strict identity
   */
  public static boolean containedIn(DNode[] first, DNode[] second) {
    
    for (int i=0,j=0; i < first.length; i++) {
      // search in otherLoc for node with same id as loc[l]
      while (j < second.length && first[i].id > second[j].id) j++;
      if (j == second.length) return false; // not found
  
      // now check whether first[i] == second[j], it may be that
      // second contains another node with the same id, iterate
      // over these but remember j
      boolean match = false;
      int j_offset = j;
      while (j_offset < second.length && first[i].id == second[j_offset].id) {
        if (first[i] == second[j_offset]) { match = true; break; }
        j_offset++;
      }
      if (!match) return false;
    }
    return true;
  }
	
  /**
   * @param arr
   * @return a string representation of the given {@link DNode} array
   */
  public static String toString(DNode[] arr) {
    if (arr == null) return "null";
    String result = "[";
    for (DNode d : arr) {
      result += d+", ";
    }
    return result + "]";
  }
  
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
	
  
  public static Comparator<DNode> compare = new Comparator<DNode>() {
    public int compare(DNode o1, DNode o2) {
      if (o1.id < o2.id) return -1;
      if (o1.id == o2.id) return 0;
      return 1;
    }
  };
	
}
