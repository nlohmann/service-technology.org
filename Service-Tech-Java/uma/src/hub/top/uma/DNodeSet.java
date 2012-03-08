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

package hub.top.uma;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * A set of {@link DNode}s that represents a branching process.
 * 
 * Firstly, a {@link DNodeSet} presents a collection of {@link DNode}s with
 * index structures for accessing specific nodes in the set. Specifically the
 * 'maximal' nodes that have no successor nodes.
 * 
 * Secondly, a {@link DNodeSet} is interpreted as an acyclic Petri net that
 * represents a branching process. This branching process can be extended at
 * its 'maximal' nodes by firing events.  
 * 
 * @author Dirk Fahland
 */
public class DNodeSet {
	
  /**
   * This shall become an efficient data structure for storing and indexing
   * sets of {@link DNode}s.
   * 
   * TODO implement
   * 
   * @author Dirk Fahland
   */
	public static class DNodeSetElement extends java.util.LinkedList<DNode>  {

		private static final long serialVersionUID = -9020905669586738898L;

	}

	/**
	 * nodes without successor before firing the first event
	 */
	private HashSet<DNode> maxNodesInitial = null;
	
	/**
	 * The conditions in this {@link DNodeSet} that are maximal before the first
	 * event is fired in this set.
	 */
	public DNodeSetElement initialConditions = null;
	
	public DNodeSetElement allConditions = null;
	public DNodeSetElement allEvents = null;
	
	/**
	 * Auxiliary representation of {@link DNodeSet#initialConditions} that is
	 * used in {@link DNodeBP} for finding cut-off events.
	 */
	public DNode[] initialCut = null;
	
	/**
	 * Initialize new {@link DNodeSet}
	 * @param num  initial number of nodes in this set
	 */
	public DNodeSet (int num) {
	  maxNodesInitial = new HashSet<DNode>();	// initialize hash-table size
		allConditions = new DNodeSetElement();
		allEvents = new DNodeSetElement();
	}

	/**
	 * Set the {@link DNodeSet#initialConditions} of this {@link DNodeSet},
	 * this are not its minimal nodes but the conditions that are maximal
	 * before the first event is fired in this set.
	 * 
	 * This method must be called before {@link DNodeSet#fire(DNode, DNode[])}
	 * or {@link DNodeSet#fire(DNode[], DNode[])} is called. 
	 */
	public void setInitialConditions () {
		initialConditions = new DNodeSetElement();	// initialize hash-table size
		// all maximal nodes of the branching process before firing any event
		// represent the initial conditions of this branching process, store them
		initialConditions.addAll(maxNodesInitial);
		
		// and translate into the array for detecting cut-off events 
		initialCut = new DNode[initialConditions.size()];
		int i=0;
		for (DNode b : initialConditions)
			initialCut[i++] = b;
		DNode.sortIDs(initialCut);
	}
	
	/**
	 * Add the nodes of the initial process to the data structures that contain
	 * all nodes of the process.
	 * 
   * This method must be called before {@link DNodeSet#fire(DNode, DNode[])}
   * or {@link DNodeSet#fire(DNode[], DNode[])} is called. 
	 */
	public void addInitialNodesToAllNodes() {
	  HashSet<DNode> seen = new HashSet<DNode>();
	  LinkedList<DNode> queue = new LinkedList<DNode>();
	  // add all initial nodes and all their transitive predecessors to the
	  // set of all nodes of this set
	  queue.addAll(initialConditions);
	  seen.addAll(initialConditions);
	  while (!queue.isEmpty()) {
	    DNode d = queue.removeFirst();
	    
	    if (d.isEvent) {
	      if (allEvents.contains(d)) System.out.println(d+" already in");
	      else allEvents.add(d);
	    }
	    //if (!d.isEvent && !initialConditions.contains(d)) allConditions.add(d);
	    
	    if (d.pre != null)
	      for (DNode pre : d.pre) {
	        if (!seen.contains(pre)) {
	          queue.add(pre);
	          seen.add(pre);
	        }
	      }
	  }
	}
	
	/**
	 * Add a new node to this {@link DNodeSet} (without firing an event) and update
	 * all internal data structures. This method is used to construct a
	 * {@link DNodeSet} from scratch or by copying.
	 * 
	 * @param newNode
	 */
	public void add(DNode newNode) {
		assert newNode != null : "Error! Trying to insert null node into DNodeSet";
		
		// the predecessors of newNode are no longer maxNodes in this set
		// remove all nodes from this set's maxNodes that are in the pre-set of newNode
		DNodeSetElement toRemove = new DNodeSetElement();
		for (DNode d : maxNodesInitial) {
			if (newNode.preContains_identity(d)) {
				toRemove.add(d);
			}
		}
		maxNodesInitial.removeAll(toRemove);
		// and add the newNode to maxNodes
		maxNodesInitial.add(newNode);
		
		// and store the newNode as event or condition
		if (newNode.isEvent) allEvents.add(newNode);
		else allConditions.add(newNode);
	}

	/**
	 * Remove node b from this set.
	 * TODO: update {@link #maxNodesInitial} 
	 * 
	 * @param b
	 */
  public void remove(DNode b) {
    
    if (b.post != null)
      for (DNode e : b.post) {
        // remove 'b' from the pre-set of each of its post-events
        DNode[] e_pre_new = new DNode[e.pre.length-1];
        for (int i=0,j=0; i<e.pre.length; i++) {
          if (e.pre[i] == b) continue;
          e_pre_new[j++] = e.pre[i];
        }
        e.pre = e_pre_new;
      }

    if (b.pre != null)
      for (DNode e : b.pre) {
        // remove 'b' from the pre-set of each of its post-events
        DNode[] e_post_new = new DNode[e.post.length-1];
        for (int i=0,j=0; i<e.post.length; i++) {
          if (e.post[i] == b) continue;
          e_post_new[j++] = e.post[i];
        }
        e.post = e_post_new;
      }
    if (b.isEvent) allEvents.remove(b);
    else allConditions.remove(b);
  }

  /**
   * Remove the 'nodes' from the node set.
   * @param nodes
   */
  public void removeAll(Collection<DNode> nodes) {
    HashSet<DNode> touched = new HashSet<DNode>();
    
    HashSet<DNode> toRemove = new HashSet<DNode>(nodes);
    
    for (DNode n : toRemove) {
      if (n.pre != null) {
        for (DNode pre : n.pre) {
          if (pre == null) continue;
          if (toRemove.contains(pre)) continue;
          
          for (int i=0; i<pre.post.length; i++) {
            if (pre.post[i] == n) {
              pre.post[i] = null;
              break;
            }
          }
          touched.add(pre);
        }
      }
      
      if (n.post != null) {
        for (DNode post : n.post) {
          if (post == null) continue;
          if (toRemove.contains(post)) continue;
          
          for (int i=0; i<post.pre.length; i++) {
            if (post.pre[i] == n) {
              post.pre[i] = null;
              break;
            }
          }
          
          touched.add(post);
        }
      }
    }
    
    DNodeSetElement newConditions = new DNodeSetElement();
    for (DNode b : allConditions) {
      if (!toRemove.contains(b)) newConditions.add(b);
    }
    allConditions = newConditions;
    
    DNodeSetElement newEvents = new DNodeSetElement();
    for (DNode b : allEvents) {
      if (!toRemove.contains(b)) newEvents.add(b);
    }
    allEvents = newEvents;
    
    for (DNode n : touched) {
      
      if (n.pre != null) {
        int preCount = 0;
        for (DNode pre : n.pre) {
          if (pre != null) preCount++;
        }
        
        DNode newPre[] = new DNode[preCount];
        int newPrePos = 0;
        for (DNode pre : n.pre) {
          if (pre != null) {
            newPre[newPrePos] = pre;
            newPrePos++;
          }
        }
        
        n.pre = newPre;
      }
      
      if (n.post != null) {
        int postCount = 0;
        for (DNode post : n.post) {
          if (post != null) postCount++;
        }
        
        DNode newPost[] = new DNode[postCount];
        int newPostPos = 0;
        for (DNode post : n.post) {
          if (post != null) {
            newPost[newPostPos] = post;
            newPostPos++;
          }
        }
        
        n.post = newPost;
      }
    }
  }
	  

	
	/**
	 * @return all nodes in this {@link DNodeSet} by backwards search from its
	 * {@link DNodeSet#maxNodes}
	 */
	public DNodeSetElement getAllNodes() {
	  /*
		LinkedList<DNode> queue = new LinkedList<DNode>(maxNodes);
		HashSet<DNode> allNodes = new HashSet<DNode>(maxNodes);
		while (!queue.isEmpty()) {
			DNode n = queue.removeFirst();
			if (n == null) continue;
			for (int i=0; i<n.pre.length; i++) {
			  if (n.pre[i] == null) continue;
			  if (!allNodes.contains(n.pre[i])) {
			    queue.add(n.pre[i]);
		      allNodes.add(n.pre[i]);
			  }
			}
		}*/
	  DNodeSetElement allNodes = new DNodeSetElement();
	  allNodes.addAll(allConditions);
	  allNodes.addAll(allEvents);
		return allNodes;
	}
	
	/**
	 * @return a list of all nodes that have no successor
	 */
	public LinkedList<DNode> getCurrentMaxNodes() {
	  LinkedList<DNode> currentMaxNodes = new LinkedList<DNode>();
	  for (DNode b : allConditions) {
	    if (b.post == null || b.post.length == 0) currentMaxNodes.add(b);
	  }
    for (DNode e : allEvents) {
      if (e.post == null || e.post.length == 0) currentMaxNodes.add(e);
    }
	  return currentMaxNodes;
	}
	
	/**
	 * @return all conditions
	 */
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

  /**
   * @return all events
   */
	public DNodeSetElement getAllEvents() {
		return allEvents;
	}
	
	/**
	 *  This is a private field that gets updated in every call of
	 *  {@link DNodeSet#getPrimeCut(DNode, boolean)}
	 *  The field contains the number of predecessor events of the
	 *  parameter 'event', not including 'event' itself.
	 */
	public int getPrimeConfiguration_size;
	
	/**
   *  This is a public field that gets updated in every call of
   *  {@link DNodeSet#getPrimeCut(DNode, boolean)}
   *  if parameter 'collectPredecessors' is set to <code>true</code>.
   *
   *  The field contains all predecessor events of the parameter 'event',
   *  unsorted and iterable.
	 */
	public HashSet<DNode> getPrimeConfiguration;
	
	/**
	 * Compute the prime cut and all predecessor events of the given event. 
	 * 
	 * @param event
	 *           the event for which the prime cut and
	 *           the predecessors shall be computed
	 * @param collectPredecessors
	 *           a list that gets filled with all predecessor events of
	 *           <code>event</code>. Can be <code>null</code>, then the
	 *           predecessors of <code>event</code> are not stored.
	 * @param configIncludeEvent
	 *           whether the prime configuration shall contain 'event'
	 * @return
	 *         a sorted array of {@link DNode}s representing the prime cut of
	 *         <code>event</code>
	 */
	public DNode[] getPrimeCut(DNode event, boolean collectPredecessors, boolean configIncludeEvent) {
		
		// the set of post-conditions of all predecessors of 'event', this set gets
		// updated as the predecessors of 'event' are discovered
		HashSet<DNode> postConditions = new HashSet<DNode>();
		
    // the events of the prime configuration of 'event'
    HashSet<DNode> primeConfiguration = new HashSet<DNode>();

    // find all predecessor events of 'event' and their post-conditions
    // by backwards breadth-first search from 'event'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.add(event);
    primeConfiguration.add(event);
		
		// conditions with a post-event that is a predecessor of 'event', these
		// conditions cannot be in the prime cut of 'event'
		HashSet<DNode> consumedConditions = new HashSet<DNode>();

		// run breadth-first search
		while (!queue.isEmpty()) {
			
			DNode first = queue.removeFirst();
      
			// add all post-conditions to the prime-cut that are not consumed by successors
			for (DNode post : first.post) {
				if (!consumedConditions.contains(post) && !post.ignore)
					postConditions.add(post);
			}
			
			for (DNode preCondition : first.pre)
			{
				// all pre-conditions of this node are consumed
				consumedConditions.add(preCondition);
				
				// add all predecessor events to the queue
				for (DNode preEvent : preCondition.pre) {
					if (!primeConfiguration.contains(preEvent)) {
					  // only add to the queue if not queued already
						queue.add(preEvent);
			      primeConfiguration.add(preEvent);
					}
				} // all predecessor events of preCondition
			} // all preconditions of the current event
		} // breadth-first search

		// the initialConditions of this braching process potentially belong
		// to the prime cut of this 'event'
    postConditions.addAll(initialConditions);
    
		// as we are doing simple breadth-first search, we may have added
		// some conditions to the post-conditions that turned out later to
		// be consumed by other events, remove these from the postConditions
		postConditions.removeAll(consumedConditions);
		
		// transform the prime cut into a sorted array 
		DNode[] primeCut = new DNode[postConditions.size()];
		postConditions.toArray(primeCut);
		DNode.sortIDs(primeCut);

		// remove the 'event' from the prime configuration if desired
    if (!configIncludeEvent) primeConfiguration.remove(event);
		getPrimeConfiguration_size = primeConfiguration.size();
		  
		if (collectPredecessors) {
		  /*
  		getPrimeCut_predecessors = new DNode[primeConfiguration.size()];
  		primeConfiguration.toArray(getPrimeCut_predecessors);
  		DNode.sortIDs(getPrimeCut_predecessors);
  		*/
      getPrimeConfiguration = primeConfiguration;

		} else {
		  //getPrimeCut_predecessors = null;
		  getPrimeConfiguration = null;
		}
		
		return primeCut;
	}
	
  public int getConfigSize(DNode[] extensionLocation) {
    return getConfiguration(extensionLocation).size();
  }
  
  public HashSet<DNode> getConfiguration(DNode[] cut) {
    // the events of the configuration before 'cut'
    HashSet<DNode> configuration = new HashSet<DNode>();

    // find all predecessor events of 'cut'
    // by backwards breadth-first search from 'cut'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (DNode b : cut) {
      for (DNode preEvent : b.pre) {
        if (!configuration.contains(preEvent)) {
          // only add to the queue if not queued already
          queue.add(preEvent);
          configuration.add(preEvent);
        }
      }
    }
    // run breadth-first search
    while (!queue.isEmpty()) {
      
      DNode first = queue.removeFirst();
      
      for (DNode preCondition : first.pre)
      {
        // add all predecessor events to the queue
        for (DNode preEvent : preCondition.pre) {
          if (!configuration.contains(preEvent)) {
            // only add to the queue if not queued already
            queue.add(preEvent);
            configuration.add(preEvent);
          }
        } // all predecessor events of preCondition
      } // all preconditions of the current event
    } // breadth-first search

    return configuration;
  }
  
  /**
   * @param d
   * @return all transitive predecessors of 'd'
   */
  public HashSet<DNode> getAllPredecessors(DNode d) {
    // the events of the configuration before 'cut'
    HashSet<DNode> predecessors = new HashSet<DNode>();

    // find all predecessor events of 'cut'
    // by backwards breadth-first search from 'cut'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.add(d);
    predecessors.add(d);
    
    // run breadth-first search
    while (!queue.isEmpty()) {
      
      DNode first = queue.removeFirst();
      
      for (DNode pre : first.pre)
      {
        if (!predecessors.contains(pre)) {
          // only add to the queue if not queued already
          queue.add(pre);
          predecessors.add(pre);
        }
      } // all preconditions of the current event
    } // breadth-first search

    return predecessors;
  }
  
  /**
   * @param d
   * @return all transitive predecessors of 'd'
   */
  public HashSet<DNode> getAllSuccessors(DNode d) {
    // the events of the configuration before 'cut'
    HashSet<DNode> successors = new HashSet<DNode>();

    // find all predecessor events of 'cut'
    // by backwards breadth-first search from 'cut'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.add(d);
    successors.add(d);
    
    // run breadth-first search
    while (!queue.isEmpty()) {
      
      DNode first = queue.removeFirst();
      
      if (first.post == null) continue;
      for (DNode post : first.post)
      {
        if (!successors.contains(post)) {
          // only add to the queue if not queued already
          queue.add(post);
          successors.add(post);
        }
      } // all preconditions of the current event
    } // breadth-first search

    return successors;
  }
	
	/**
	 * Fire ocletEvent in this set at the given location. This
	 * appends a new event with the same id as <code>ocletEvent</code> to
	 * this set, and corresponding post-conditions.
	 * 
	 * @param ocletEvent
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode ocletEvent, DNode[] fireLocation, boolean safe) {
	  
	  
	  
	  // FIXME: DNodeBP causes multiple occurrences of the same event
	  if (!safe && eventExistsAtLocation(ocletEvent.id, fireLocation)) {
      System.err.println("trying to fire existing event "+ocletEvent+" at "+DNode.toString(fireLocation));
    }

		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvent.id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// remember the oclet event that caused this node, required for determining
		// cutOff events
		newEvent.causedBy = new int[1];
		newEvent.causedBy[0] = ocletEvent.globalId;
		
		System.out.println("fired "+newEvent+" cause: "+newEvent.causedBy);
		
		// set post-node of conditions
		for (DNode preNode : fireLocation) {
			preNode.addPostNode(newEvent);
			
	    // remove pre-set of newEvent from the maxNodes
			//maxNodes.remove(preNode);
		}
		
		// instantiate the post-conditions of the oclet event
		DNode postConditions[] = new DNode[ocletEvent.post.length];
		for (int i=0; i<postConditions.length; i++) {
			postConditions[i] = new DNode(ocletEvent.post[i].id, newEvent);
			allConditions.add(postConditions[i]);
			//assert postConditions[i] != null : "Error, created null post-condition!";
			
		  // add post-set of newEvent to the maxNodes
			//maxNodes.add(postConditions[i]);
		}
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		return postConditions;
	}
	
	/**
	 * Synchronously fire a set of ocletEvents in this set at
	 * the given location. This appends ONE new event with the
	 * same id as all ocletEvents to this set, and corresponding
	 * post-conditions for all the given ocletEvents 
	 * 
	 * @param ocletEvents
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode[] ocletEvents, DNode[] fireLocation, boolean safe) {
	   
    // FIXME: DNodeBP causes multiple occurrences of the same event
	  if (!safe && eventExistsAtLocation(ocletEvents[0].id, fireLocation)) {
	    System.err.println("trying to fire existing events "+DNode.toString(ocletEvents)+" at "+DNode.toString(fireLocation));
	  }
	  
		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvents[0].id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// set post-node of conditions
		for (DNode preNode : fireLocation) {
			preNode.addPostNode(newEvent);
		  // remove pre-set of newEvent from the maxNodes
			//maxNodes.remove(preNode);
		}

    // remember the oclet event that caused this node, required for determining
    // cutOff events
		newEvent.causedBy = new int[ocletEvents.length];

    // collect the IDs of all post-conditions of all events
		// for improved efficiency, create a boolean-array for all condition IDs
		// in the post-conditions of 'ocletEvents'
		
		// get length of the array = max ID
		short maxPostConditionID = 0; 
		for (DNode e : ocletEvents) {
		  if (e.post[e.post.length-1].id > maxPostConditionID)
		    maxPostConditionID = e.post[e.post.length-1].id;
		}
		// set array entry to true if a post-condition has the corresponding ID
		boolean[] postConditionIDs = new boolean[maxPostConditionID+1];
		int postCount = 0;
		for (int i=0; i<ocletEvents.length; i++) {

      // remember the oclet event that caused this node, required for determining
      // cutOff events
      newEvent.causedBy[i] = ocletEvents[i].globalId;
      for (DNode b : ocletEvents[i].post) {
        if (!postConditionIDs[b.id]) {
          postCount++;
          postConditionIDs[b.id] = true;  
        }
      }
    }
    // now convert the boolean array to a short array of IDs 
		
		System.out.println("fired* "+newEvent+" cause: "+newEvent.causedBy);
		
		// instantiate the remaining post-conditions of the new oclet event
		DNode postConditions[] = new DNode[postCount];
		int i = 0;
		for (short id = 0; id < postConditionIDs.length; id++) {
		  if (!postConditionIDs[id]) continue;
			DNode b = new DNode(id, newEvent);
			allConditions.add(b);
			//maxNodes.add(b);
			//assert postConditions[i] != null : "Error, created null post-condition!";
			
			postConditions[i] = b;
			i++;
		}
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		return postConditions;
	}
	
	/**
   * Synchronously fire a set of ocletEvents in this set at
   * the given location. This appends ONE new event with the
   * same id as all ocletEvents to this set, post-conditions
   * ARE NOT APPENDED. 
   * 
	 * @param ocletEvents
	 * @param fireLocation
	 * 
	 * @param the new event
	 */
  public DNode fire_eventOnly(DNode[] ocletEvents, DNode[] fireLocation) {
     
    // instantiate the oclet event
    DNode newEvent = new DNode(ocletEvents[0].id, fireLocation);
    newEvent.isEvent = true;
    allEvents.add(newEvent);
    
    // set post-node of conditions
    for (DNode preNode : fireLocation) {
      preNode.addPostNode(newEvent);
      // remove pre-set of newEvent from the maxNodes
      //maxNodes.remove(preNode);
    }
    
    return newEvent;
  }
	
	public static boolean eventExistsAtLocation(short id, DNode[] fireLocation) {
	  
    for (int g=0; g<fireLocation.length; g++) {
      if (fireLocation[g].post != null) {
        
        // check for each event in b's post-set that has the given id
        // whether it consumes from the entire fireLocation. if yes, return false
        for (DNode e : fireLocation[g].post) {
          if (e.id == id) {

            // found event e with same id, we could collect events in a set to
            // prevent repeated execution of the subsequent loop, but creating
            // and managing the set takes more time and resources than executing
            // the loop several times
            
            boolean containsAllPre = true;

            if (e.pre.length == fireLocation.length) {
              // same number of conditions, arrays are ordered, so they must match exactly
              for (int i=0; i<e.pre.length;i++) {
                if (e.pre[i] != fireLocation[i]) { containsAllPre = false; break; }
              }
            } else {
              // different number of conditions, arrays are ordered,
              // allow to skip some conditions in fireLocation
              int f=0;
              for (int i=0; i<e.pre.length;i++) {
                // move f to the condition in fireLocation with the same id as
                // precondition pre[i]
                while (e.pre[i].id < fireLocation[f].id && f < fireLocation.length) f++;
                // moved out of array: not found
                if (f == fireLocation.length) { containsAllPre = false; break; }
  
                // now e.pre[i].id >= fireLocation[f].id
                // if e.pre[i].id == fireLocation[f].id, then this is the spot where it holds,
                // if not then there is no other condition in fireLocation that equals e.pre[i]
                if (e.pre[i] != fireLocation[f]) { containsAllPre = false; break; }
              }
            }
            // event e has all pre-conditions in fireLocation
            if (containsAllPre) return true;
          }
        }
      }
    }
    return false;
	}
	
	/**
	 * Compute the maximal distance of each node from the {@link #initialConditions}
	 * of this branching process. The result is returned as a {@link HashMap} from
	 * {@link DNode}s to shorts. The depth of a node can be used see whether two 
	 * events of this branching process are direct successor events. In this case
	 * they are separated by only one condition and the difference of their depths
	 * is exactly 2. If the difference is larger, then there are more events in
	 * between, even if there is a direct dependency between the two events via a
	 * condition.
	 * 
	 * @return
	 */
	
  public HashMap<DNode, Short> getDepthMap() {
    // initialize depth map
    HashMap<DNode, Short> depth = new HashMap<DNode, Short>();
    
    // then run a breadth-first search beginning at the initial conditions
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (DNode d : this.initialConditions) {
      depth.put(d, (short)0); // begin with depth 0
      queue.addLast(d);
    }
    
    while (!queue.isEmpty()) {
      DNode d = queue.remove();
      if (d.post == null) continue;
      for (DNode dPost : d.post) {
        // for each successor, take the maximum of the depths + 1
        Short dPostDepth = depth.get(dPost);
        if (dPostDepth == null) depth.put(dPost, (short)(depth.get(d)+1));
        else {
          if (dPostDepth < depth.get(d)) dPostDepth = (short)(depth.get(d)+1);
          depth.put(dPost, (short)(depth.get(d)+1));
        }
        queue.addLast(dPost);
      }
    }
    
    return depth;
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

	/**
	 * Option: Print nodes of the branching process that are removed due to
	 * anti-oclets. 
	 */
	public static boolean option_printAnti = true;
	
	public String toDot (String properNames[]) {
	  return toDot(properNames, new HashMap<DNode, String>());
	}
	
	/**
	 * Create a GraphViz' dot representation of this branching process.
	 * 
	 * @return 
	 */
	public String toDot (String properNames[], HashMap<DNode,String> coloring) {
		StringBuilder b = new StringBuilder();
		b.append("digraph BP {\n");
		
		// standard style for nodes and edges
		b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
		b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
		b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

		//String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
		String cutOffFillString = "fillcolor=gold";
		String antiFillString = "fillcolor=red";
		String impliedFillString = "fillcolor=violet";
		String hiddenFillString = "fillcolor=grey";
		
		// first print all conditions
		b.append("\n\n");
		b.append("node [shape=circle];\n");
		for (DNode n : getAllConditions()) {
			
			if (!option_printAnti && n.isAnti)
				continue;
			/* - print current marking
			if (cutNodes.contains(n))
				b.append("  c"+n.localId+" ["+tokenFillString+"]\n");
			else
			*/
			if (coloring.containsKey(n)) {
			  String rgbColorFillString = "fillcolor="+coloring.get(n);
			  b.append("  c"+n.globalId+" ["+rgbColorFillString+"]\n");
			}
			else if (n.isAnti && n.isHot)
				b.append("  c"+n.globalId+" ["+antiFillString+"]\n");
      else if (n.isCutOff)
        b.append("  c"+n.globalId+" ["+cutOffFillString+"]\n");
      else if (n.isImplied)
        b.append("  c"+n.globalId+" ["+impliedFillString+"]\n");
			else
				b.append("  c"+n.globalId+" []\n");
			
			String auxLabel = "";
      String antiLabel = n.isAnti ? "--" : "";
      String nodeLabel = "'"+properNames[n.id]+"'"+antiLabel+" ("+n.id+")["+n.globalId+"]";
				
			b.append("  c"+n.globalId+"_l [shape=none];\n");
			b.append("  c"+n.globalId+"_l -> c"+n.globalId+" [headlabel=\""+nodeLabel+" "+auxLabel+"\"]\n");
		}

    // then print all events
		b.append("\n\n");
		b.append("node [shape=box];\n");
		for (DNode n : getAllEvents()) {
			
			if (!option_printAnti && n.isAnti)
				continue;
			
			
			if (coloring.containsKey(n)) {
        String rgbColorFillString = "fillcolor="+coloring.get(n);
        b.append("  e"+n.globalId+" ["+rgbColorFillString+"]\n");
      }
      else if (n.isAnti && n.isHot)
				b.append("  e"+n.globalId+" ["+antiFillString+"]\n");
			else if (n.isAnti && !n.isHot)
        b.append("  e"+n.globalId+" ["+hiddenFillString+"]\n");
			else if (n.isImplied)
        b.append("  e"+n.globalId+" ["+impliedFillString+"]\n");
			else if (n.isCutOff)
				b.append("  e"+n.globalId+" ["+cutOffFillString+"]\n");
			else
				b.append("  e"+n.globalId+" []\n");
			
      String auxLabel = "";
      String antiLabel = n.isAnti ? "--" : "";
      String nodeLabel = "'"+properNames[n.id]+"'"+antiLabel+" ("+n.id+")["+n.globalId+"]";

			b.append("  e"+n.globalId+"_l [shape=none];\n");
			b.append("  e"+n.globalId+"_l -> e"+n.globalId+" [headlabel=\""+nodeLabel+" "+auxLabel+"\"]\n");
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
		
		// finally, print all edges
		b.append("\n\n");
		b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
		for (DNode n : getAllNodes()) {
			String prefix = n.isEvent ? "e" : "c";
			
			for (int i=0; i<n.pre.length; i++) {
				if (n.pre[i] == null) continue;
				if (!option_printAnti && n.isAnti) continue;
				
				String rgbColorString = "";
	      if (coloring.containsKey(n)) {
	        rgbColorString = "color="+coloring.get(n);
	      }
				
				if (n.pre[i].isEvent)
					b.append("  e"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0 "+rgbColorString+"]\n");
				else
					b.append("  c"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0 "+rgbColorString+"]\n");
			}
		}
		
		b.append("}");
		return b.toString();
	}
}
