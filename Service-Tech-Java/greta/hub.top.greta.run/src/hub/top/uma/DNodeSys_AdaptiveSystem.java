/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved. 
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

package hub.top.uma;

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
import hub.top.adaptiveSystem.Temp;
import hub.top.greta.cpn.ColoredConditionVisitor;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * A {@link DNode} representation of an {@link AdaptiveSystem} consisting of
 * {@link Oclet}s used for constructing a McMillan prefix of this system. This
 * class is used in {@link DNodeBP}.
 * 
 * 
 * @author Dirk Fahland
 */
public class DNodeSys_AdaptiveSystem extends DNodeSys {

	private int						nodeNum;	// counter number of nodes 
	
	private Map<Node, DNode>     nodeEncoding;    // encoding of oclet Nodes as DNodes
	private Map<DNode, Node>     nodeOrigin;      // inverse of #nodeEncoding
	private Map<Oclet, DNodeSet> ocletEncoding;   // encoding of oclets to DNodeSets
	
	private INameProcessor   namePostProcessor; // post-processor for names of nodes
	
	private boolean isDataDependent;

/**
   * Construct DNode representation of an {@link AdaptiveSystem}. Every event
   * of an {@link Oclet}s contribution ({@link Oclet#getDoNet()}) becomes a
   * {@link DNodeSys#fireableEvents}, every event of an {@link Oclet}s
   * precondition ({@link Oclet#getPreNet()}) becomes a {@link DNodeSys#preConEvents).
   * Pre- and post-conditions of these events are stored accordingly.
   * 
   * @param as
   * @param namePostProcessor helper to change names of nodes before materializing strings
   *                as identities, used to map a high-level oclet specification to a
   *                low-level oclet specification
   * @param isDataDependent whether the behavior of the system is data-dependent
   */
	 public DNodeSys_AdaptiveSystem(AdaptiveSystem as, INameProcessor namePostProcessor, boolean isDataDependent) throws InvalidModelException {

	    // initialize the standard data structures
	    super();
	    
	    this.namePostProcessor = namePostProcessor;
	    this.isDataDependent = isDataDependent;
	    
	    // create a name table initialize the translation tables
	    buildNameTable(as);
	    nodeEncoding = new HashMap<Node, DNode>(nodeNum);
	    nodeOrigin = new HashMap<DNode, Node>(nodeNum);
	    ocletEncoding = new HashMap<Oclet, DNodeSet>(as.getOclets().size());
	    
	    // then translate all oclets, first normal oclets, then anti-oclets
	    for (Oclet o : as.getOclets())
	      if (o.getOrientation() == Orientation.NORMAL)
	        ocletEncoding.put(o, buildDNodeRepresentation(o));
	    for (Oclet o : as.getOclets())
	      if (o.getOrientation() == Orientation.ANTI)
	        ocletEncoding.put(o, buildDNodeRepresentation(o));
	    
	    // and the adaptive process
	    initialRun = buildDNodeRepresentation(as.getAdaptiveProcess());
	    
	    finalize_setPreConditions();
	    finalize_initialRun();
	    finalize_generateIndexes();
	    
	    for (Map.Entry<Node, DNode> m : nodeEncoding.entrySet()) {
	      if (m.getKey().eContainer().eContainer() instanceof Oclet)
	        System.out.println(m.getKey().getName()+"/"+((Oclet)m.getKey().eContainer().eContainer()).getName()+" --> "+m.getValue());
	    }
	 }
	
	/**
	 * Construct DNode representation of an {@link AdaptiveSystem}. Every event
	 * of an {@link Oclet}s contribution ({@link Oclet#getDoNet()}) becomes a
	 * {@link DNodeSys#fireableEvents}, every event of an {@link Oclet}s
	 * precondition ({@link Oclet#getPreNet()}) becomes a {@link DNodeSys#preConEvents).
	 * Pre- and post-conditions of these events are stored accordingly.
	 * 
	 * @param as
	 */
	public DNodeSys_AdaptiveSystem(AdaptiveSystem as) throws InvalidModelException {
	  this(as, INameProcessor.IDENTITY, false);
	}

	/**
	 * Translate full strings of the Adaptive System to ids and fill
   * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
   * 
	 * @param as
	 */
	private void buildNameTable(AdaptiveSystem as) throws InvalidModelException {

		nodeNum = 0;
		
		if (as.getAdaptiveProcess() == null)
		  throw new InvalidModelException(InvalidModelException.NO_INITIAL_STATE, as);
		
		// collect all names and assign each new name a new ID
		for (Node n : as.getAdaptiveProcess().getNodes()) {
		  String name = namePostProcessor.process(n, n.getName());
			if (nameToID.get(name) == null)
				nameToID.put(name, currentNameID++);
			nodeNum++;
		}
		for (Oclet o : as.getOclets()) {
			for (Node n : o.getPreNet().getNodes()) {
			  String name = namePostProcessor.process(n, n.getName());
				if (nameToID.get(name) == null)
					nameToID.put(name, currentNameID++);
				nodeNum++;
			}
			for (Node n : o.getDoNet().getNodes()) {
			  String name = namePostProcessor.process(n, n.getName());
				if (nameToID.get(name) == null)
					nameToID.put(name, currentNameID++);
				nodeNum++;
			}
		}
		
		// build the translation table from IDs to names
		finalize_setProperNames();
	}

	/**
   * The actual translation algorithm for transforming an acylic Petri net
   * into a {@link DNodeSet}. The translations preserves the structure
   * of the net.
	 * 
	 * @param o
	 * @param isAnti
	 * @param beginAtMarked
	 * @return
	 */
	private Collection<DNode> translateToDNodes(OccurrenceNet o, boolean isAnti, boolean beginAtMarked) {
		// LinkedList<DNode> allNodes = new LinkedList<DNode>();
		LinkedList<DNode> maxNodes = new LinkedList<DNode>();
		LinkedList<Node> searchQueue = new LinkedList<Node>();
		
		Map<Node, Integer> depth = new HashMap<Node, Integer>();
		
		// construct DNode representation only from marked conditions
		if (beginAtMarked) {
		  for (Node n : o.getNodes()) {
		    if (n instanceof Condition) {
          Condition b = (Condition)n;
          if (b.getToken() == 1) {
            searchQueue.add(b);
            
            // we compute the length of the longest path that ends in an
            // event, a path that ends in the pre-event of this condition
            // has one edge lass than a path that ends in this condition
            depth.put(n, -1); 
          }
		    }
		  }
		  
		// construct DNode representation from all nodes
		} else {
  		// fill the queue with all maximal nodes of the occurrence net
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
		}

		// then do a breadth-first search on the structure of the oclet
		// and add each visited node to the buildStack, the same node
		// can be added multiple times
		LinkedList<Node> buildStack = new LinkedList<Node>();
		while (!searchQueue.isEmpty()) {
			Node n = searchQueue.removeFirst();
			
			int n_depth = depth.get(n);
			
			for (Node pre : getPredecessors(n, false)) {

	      Integer d = depth.get(pre);
				searchQueue.addLast(pre);

				if (d == null || d <= n_depth) {
					depth.put(pre, n_depth+1);	// increment depth of node
					// and remember maximum depth of a node
					maxHistoryDepth = Math.max(n_depth+1, maxHistoryDepth);
				}
			}
			if (buildStack.contains(n)) buildStack.remove(n);
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
			
      // save the predecessors of node n in an array
			LinkedList<Node> n_pre_direct = getDirectPredecessors(n);
      DNode pre[] = new DNode[n_pre_direct.size()];
      for (int i=0; i<pre.length; i++) {
        pre[i] = nodeEncoding.get(n_pre_direct.get(i));
      }
      LinkedList<Node> n_pre_trans = getPredecessors(n, true);
      DNode preTrans[] = new DNode[n_pre_trans.size()];
      for (int i=0; i<preTrans.length; i++) {
        preTrans[i] = nodeEncoding.get(n_pre_trans.get(i));
      }
			
			// count all successors that will be translated to a DNode
			int postSize = 0; 
      for (Node postNode : n.getPostSet()) {
        if (depth.containsKey(postNode)) postSize++;
      }
			
			// create new DNode d for Node n
      String name = namePostProcessor.process(n, n.getName());
			DNode d;
			if (nodeEncoding.containsKey(n)) {
			  System.out.println("visiting "+n+" again ");
			  d = nodeEncoding.get(n);
		  } else {
  			if (preTrans.length == 0)
  			  d = new DNode(nameToID.get(name), pre);
  			else
  			  d = new DNodeTransitive(nameToID.get(name), pre, preTrans);
  			//allNodes.add(d);
			}
			
			//System.out.println(d+" has predecessors "+DNode.toString(pre)+" and "+DNode.toString(preTrans));
      
			if (n instanceof Event) {
			  
				d.isEvent = true;
				// remember all events of a DoNet: these will be fired if enabled
				if (n.eContainer() instanceof DoNet) {
          boolean completeEvent = true;
          for (Node m : n.getPreSet()) if (m.isAbstract()) completeEvent = false;
          for (Node m : n.getPostSet()) if (m.isAbstract()) completeEvent = false;
          
          // can only fire complete events
          if (completeEvent) fireableEvents.add(d);
				} else if (n.eContainer() instanceof PreNet) {
					preConEvents.add(d);
				}
				
				// for each pre-condition 'b' of this event, store this event's id
				// as the visible event that must not appear in the transitive history
				// of 'b' when checking for enabled conditions
				for (DNode b : pre) {
				  if (!b.isEvent && b instanceof DNodeTransitive) {
				    //System.out.println("giving "+b+" visible event "+d);
				    ((DNodeTransitive)b).visibleEvent = d.id;
				  }
				}
        for (DNode b : preTrans) {
          if (!b.isEvent && b instanceof DNodeTransitive) {
            //System.out.println("giving "+b+" visible event "+d);
            ((DNodeTransitive)b).visibleEvent = d.id;
          }
        }
			}
			d.isAnti = isAnti;
			d.isHot = (n.getTemp() == Temp.HOT); 
			
			// initialize post-set of new DNode d
			if (postSize > 0) {
				d.post = new DNode[postSize];
				for (int i=0; i<postSize; i++)
					d.post[i] = null;
			} else {
			  d.post = new DNode[0];
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
			nodeOrigin.put(d, n);     // and its reverse mapping
		}
		// return allNodes;
		return maxNodes;
	}
	
	private LinkedList<Node> getPredecessors(Node n, boolean indirectOnly) {
	  LinkedList<Node> queue = new LinkedList<Node>();
	  HashSet<Node> seen = new HashSet<Node>();
	  LinkedList<Node> concretePred = new LinkedList<Node>();
	  for (Node m : n.getPreSet()) {
	    if (!indirectOnly || m.isAbstract()) {
  	    queue.add(m);
  	    seen.add(m);
	    }
	  }
	  while (!queue.isEmpty()) {
	    Node m = queue.removeFirst();
	    if (!m.isAbstract()) {
	      concretePred.add(m);
	    } else {
	      for (Node m2 : m.getPreSet()) {
  	      if (!seen.contains(m2)) {
  	        queue.addLast(m2);
  	        seen.add(m2);
  	      }
	      }
	    }
	  }
	  //System.out.println(n+" has predecessors "+concretePred+" "+indirectOnly);
	  return concretePred;
	}

  private LinkedList<Node> getDirectPredecessors(Node n) {
    LinkedList<Node> concretePred = new LinkedList<Node>();
    for (Node m : n.getPreSet()) {
      if (!m.isAbstract())
        concretePred.add(m);
    }
    return concretePred;
  }

	/**
	 * Translate an oclet into a {@link DNodeSet} of its {@link DNode}s.
	 * 
	 * @param o
	 * @return
	 */
	private DNodeSet buildDNodeRepresentation(Oclet o) {
		
		Collection<DNode> nodesOfO = translateToDNodes(o.getDoNet(), o.getOrientation() == Orientation.ANTI, false);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : nodesOfO)
			ds.add(d);
		
		// a maximal condition in the PreNet may not have a successor
		// so translate the pre-net and add all additional maximal nodes to
		// the DNode set as well
		Collection<DNode> maxNodesOfHist = translateToDNodes(o.getPreNet(), false, false);
    for (DNode d : maxNodesOfHist) {
      if (!ds.allConditions.contains(d))
        ds.add(d);
    }
    
    
    if (o.getOrientation() == Orientation.ANTI) {
      hasDynamicSynchronization = true;
    }
		
		return ds;
	}
	
	/**
	 * Translate the initial run of an {@link AdaptiveSystem} into a {@link DNodeSet}
	 * representation.
	 *  
	 * @param ap
	 * @return
	 */
	private DNodeSet buildDNodeRepresentation(AdaptiveProcess ap) {
		
		Collection<DNode> nodesOfAP = translateToDNodes(ap, false, true);
		
		// store the constructed DNodes in a DNodeSet, this represents the oclet
		DNodeSet ds = new DNodeSet(properNames.length);
		for (DNode d : nodesOfAP)
			ds.add(d);
		return ds;
	}

	/**
   * the maximal cut of each oclet's history in the system, required
   * for finding implied scenarios
   *
	 * @return the maximal cut of each oclet's history in the system
	 */
	public LinkedList<DNode[]> getHistories() {
	  LinkedList<DNode[]> histories = new LinkedList<DNode[]>();
	  // iterate over all oclets
	  for (Oclet o : ocletEncoding.keySet()) {
	    // and return the maximal conditions of the pre-nets = global history
	    Collection<Condition> histMax = o.getPreNet().getMarkedConditions();
	    DNode[] dConditions = new DNode[histMax.size()];
	    int i=0;
	    for (Node p : histMax) {
	      if (nodeEncoding.get(p) == null) System.err.println(p+" of "+o+"was not translated");
	      dConditions[i++] = nodeEncoding.get(p);
	    }
	    DNode.sortIDs(dConditions);
	    histories.add(dConditions);
	  }
	  return histories;
	}
	
	/**
	 * @param d
	 * @return the node that caused the definition of {@link DNode} 'd' in this system
	 */
	public Node getOriginalNode(DNode d) {
	  return nodeOrigin.get(d);
	}
	
  /**
   * @param d
   * @return the node that represents {@link Node} 'n' in this system
   */
  public DNode getResultNode(Node n) {
    return nodeEncoding.get(n);
  }
  
  @Override
  public DNodeEmbeddingVisitor getEmbeddingVisitor() {
    if (isDataDependent)
      return new ColoredConditionVisitor(this);
    else
      return super.getEmbeddingVisitor();
  }
	
	@Override
	public String getInfo() {
    
    int numPlaces = 0;
    int numTransition = 0;
    int numArcs = 0;
    
    for (Node n : nodeEncoding.keySet()) {
      if (n instanceof Event)
        numTransition++;
      else
        numPlaces++;
      numArcs += n.getPreSet().size();
    }
	  return "|P| = "+numPlaces+" |T| = " + numTransition + " |F| = "+numArcs;
	}
}
