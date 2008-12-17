/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.greta.oclets.canonical;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.greta.oclets.ts.AdaptiveSystemStep;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;

public class CNodeSet {

	private HashSet<CNode> nodes = new HashSet<CNode>();
	private HashSet<CNode> cutNodes = new HashSet<CNode>();
	private HashSet<CNode> maxNodes = new HashSet<CNode>();
	
	private Map<Integer, List<CNode> > indexedNodes;
	
	private EMap<Node, CNode> translationMap;
	
	public int num;
	
	public static CNodeSet fromPreNet(Oclet o) {
		EList<Node> preNodes = o.getPreNet().getNodes();
		EList<Node> preNetMaxNodes = new BasicEList<Node>();

		// initialize breadth-first search queue
		for (Node n : preNodes) {
			if (n instanceof Condition) {
				Condition con = (Condition)n;
				if (con.isMaximal()) {
					preNetMaxNodes.add(con);
				}
			}
		}
		
		CNodeSet set = new CNodeSet(preNetMaxNodes, preNodes.size());
		set.cutNodes.clear();
		set.cutNodes = set.maxNodes;	// overwrite
		return set;
	}
	
	public static CNodeSet fromOclet(Oclet o) {
		EList<Node> ocletNodes = new BasicEList<Node>();
		ocletNodes.addAll(o.getPreNet().getNodes());
		ocletNodes.addAll(o.getDoNet().getNodes());
		
		EList<Node> ocletMaxNodes = new BasicEList<Node>();

		// initialize breadth-first search queue
		for (Node n : ocletNodes) {
			if (n instanceof Condition) {
				if (((Condition) n).getPostEvents().size() == 0)
					ocletMaxNodes.add(n);
			} else if (n instanceof Event) {
				if (((Event) n).getPostConditions().size() == 0)
					ocletMaxNodes.add(n);
			}
		}
		
		return new CNodeSet(ocletMaxNodes, ocletNodes.size());
	}
	
	public static CNodeSet fromAdaptiveProcess(AdaptiveProcess ap) {
		EList<Node> apCut = new BasicEList<Node>();
		apCut.addAll(ap.getMarkedConditions());
		return new CNodeSet(apCut, ap.getNodes().size());
	}
	
	/**
	 * Create a copy of the subset of nodes after the cut of the
	 * given {@code CNodeSet} while preserving references to non-copied
	 * nodes, the {@code CNodeSet#cutNodes} of the copy point to the
	 * original {@code CNodeSet#cutNodes} (which are predecessors of
	 * the copied nodes)
     *
	 * @param otherSet
	 * @return
	 */
	public static CNodeSet fromCNodeSet_afterCut(CNodeSet otherSet) {
		CNode.MatchingRelation upToCut = new CNode.MatchingRelation();
		//HashSet<CNode> past = new HashSet<CNode>(otherSet.size());
		for (CNode cutNode : otherSet.cutNodes) {
			//cutNode.getPast(past);
			upToCut.put(cutNode, cutNode);
		}
		
		CNodeSet futureCopy = new CNodeSet(otherSet.size());
		
		// create a copy of each node of otherSet going down to
		// the nodes that are in the cut of otherSet, thus only
		// the nodes after the cut and up to the maximal nodes
		// of otherSet are copied
		for (CNode c : otherSet.getMaxNodes()) {
			futureCopy.maxNodes.add(futureCopy.toCNode(c, upToCut));
		}
		futureCopy.cutNodes = (HashSet<CNode>) otherSet.cutNodes.clone();
		
		return futureCopy;
	}
	
	/**
	 * create empty {@code CNodeSet} 
	 * @param size	initial size of the indexing hash table, will
	 * 				automatically be extended at run-time if necessary
	 */
	public CNodeSet (int size) {
		// prepare indexing structure for this set of CNodes
		indexedNodes = new HashMap<Integer, List<CNode> >(size);
	}
	
	/**
	 * Translate a set of oclet nodes into a set of canonically named nodes.
	 * All nodes that can be reached by following the flow relation backwards
	 * will also be translated and be stored in this set. Therefore, it is sufficient
	 * to provide the maximal nodes wrt. the flow relation as parameters.
	 *  
	 * @param ocletMaxNodes the maximal nodes of the oclet to be translated
	 * @param size initial number of nodes in this set of CNodes 
	 */
	public CNodeSet (EList<Node> ocletMaxNodes, int size) {
		this (size);
		
		assert(ocletMaxNodes.size() > 0);
		
		translationMap = new BasicEMap<Node, CNode>();
		for (Node n : ocletMaxNodes) {
			maxNodes.add(toCNode(n));
		}
		translationMap.clear();
		
		assert(maxNodes.size() > 0);
	}
	
	/**
	 * translate a given Petri net node into a canonically named node ({@link CNode})
	 * @param n node to translate
	 * @return
	 */
	private CNode toCNode (Node n) {
		// must be initialized externally/by the constructor
		assert(translationMap != null);	
		
		if (translationMap.containsKey(n))
			return translationMap.get(n);
		
		boolean cutNode = false;
		EList<CNode> cPred = new BasicEList<CNode>();
		if (n instanceof Condition) {
			for (Node nPred : ((Condition)n).getPreEvents()) {
				CNode c = toCNode(nPred);
				cPred.add(c);
			}
			
			if (((Condition) n).isMarked())
				cutNode = true;
			
		} else if (n instanceof Event) {
			for (Node nPred : ((Event)n).getPreConditions()) {
				CNode c = toCNode(nPred);
				cPred.add(c);
			}
		}
		CNode c = new CNode(n, cPred);
		translationMap.put(n, c);
		addNode(c);
		if (cutNode)
			cutNodes.add(c);
		
		return c;
	}
	
	/**
	 * Create a recursive copy of the given {@code CNode} while re-using
	 * copies that are provided in the <code>copyMap</code>. The <code>copyMap</code>
	 * is extended by this method mapping each original node to its copy. 
	 * @param c			node to copy
	 * @param copyMap	map storing all known copies
	 * @return copy of <code>c</code>
	 */
	private CNode toCNode(CNode c, CNode.MatchingRelation copyMap) {
		if (copyMap.containsKey(c))	// node was copied before
			return copyMap.get(c);
		
		// copy all predecessors
		EList<CNode> copyPred = new BasicEList<CNode>();
		for (CNode pred : c.pred)
			copyPred.add(toCNode(pred, copyMap));
		
		// copy this node and assign the copied predecessors
		CNode copy = CNode.localCopy(c);
		copy.pred.addAll(copyPred);
		
		// extend copy map
		copyMap.put(c, copy);
		addNode(copy);
		// TODO set cut nodes
		
		return copy;
	}

	/**
	 * store node in this set
	 * @param c
	 */
	public CNode addNode(CNode c) {
		if (!nodes.add(c)) {
			
			// there is already an equivalent node in this set
			// retrieve it via the index structure
			Integer key = new Integer(c.hashCode());
			List<CNode> matches = indexedNodes.get(key);
			for (CNode existing : matches) {
				if (existing.equals(c))
					return existing;
			}
			
			return null;
		}
		
		// store new CNode in the index structure
		Integer key = new Integer(c.hashCode());
		List<CNode> bucket = indexedNodes.get(key);
		if (bucket == null) {
			bucket = new LinkedList<CNode>();
			indexedNodes.put(new Integer(c.hashCode()), bucket);
		}
		bucket.add(c);
		
		// predecessors of the new node are no longer maximal in
		// this set
		maxNodes.removeAll(c.pred);	
		
		return c;
	}
	
	/**
	 * remove a node from this set and all its causal successors
	 * @param c
	 * @return all nodes that were removed
	 */
	public Collection<CNode> removeNode(CNode c) {
		HashSet<CNode> nodesToRemove = getAllCausalSuccessors(c);
		HashSet<CNode> possibleMaxNodes = new HashSet<CNode>();
		for (CNode r : nodesToRemove) {
			
			assert(cutNodes.contains(r) == false);
			hideNode(r, possibleMaxNodes);
		}
		
		updateMaxNodes(possibleMaxNodes);

		return nodesToRemove;
	}
	
	/**
	 * Remove a node from this set and from the index structure,
	 * but leave its causal successors in the net. This function
	 * aids in constructing suffixes of CNodeSets. This concenvience
	 * method also updates the maximal nodes of this CNodeSet
	 * accordingly.
	 * 
	 * @param c
	 */
	public void hideNode(CNode r) {
		HashSet<CNode> possibleMaxNodes = new HashSet<CNode>();
		hideNode(r, possibleMaxNodes);
		updateMaxNodes(possibleMaxNodes);
	}
	
	/**
	 * Remove a node from this set and from the index structure,
	 * but leave its causal successors in the net. This function
	 * aids in constructing suffixes of CNodeSets. Updating the
	 * set of maximal nodes must be done by the help of
	 * {@code CNodeSet#updateMaxNodes(HashSet)}, candidate nodes
	 * are provided in <code>possibleMaxNodes</code>.
	 * 
	 * @param c
	 */
	public void hideNode(CNode r, HashSet<CNode> possibleMaxNodes) {
		
		// remove node r from the data structures of this set
		nodes.remove(r);
		maxNodes.remove(r);
		
		// all predecessors of a removed node are possible new maxNodes
		possibleMaxNodes.addAll(r.pred);
		possibleMaxNodes.remove(r);
		
		// remove node r from the index structures of this set
		Integer key = new Integer(r.hashCode());
		List<CNode> bucket = indexedNodes.get(key);
		if (bucket != null) bucket.remove(r);
	}
	
	/**
	 * Update the set of maximal nodes of this CNodeSet, candidates for
	 * new maximal nodes are provided as a parameter. The method checks
	 * whether a candidate has no successor. If this is the case, then
	 * it is a maximal node.
	 * 
	 * @param possibleMaxNodes
	 */
	private void updateMaxNodes(HashSet<CNode> possibleMaxNodes) {
		for (CNode c2 : nodes) {
			// remove all possibly maximal nodes that have a successor
			// in this set
			possibleMaxNodes.removeAll(c2.pred);
		}
		// add the remaining nodes to the max nodes
		maxNodes.addAll(possibleMaxNodes);
	}
	
	public HashSet<CNode> getAllCausalSuccessors(CNode pred) {
		HashSet<CNode> dependingNodes = new HashSet<CNode>(this.size());
		for (CNode max : maxNodes) {
			max.causallyDependsOn(pred, dependingNodes);
		}
		return dependingNodes;
	}
	
	public boolean contains(CNode c) {
		return nodes.contains(c);
	}
	
	public List<CNode> get(Integer key) {
		return indexedNodes.get(key);
	}
	
	public HashSet<CNode> getAllNodes () {
		return nodes;
	}
	
	public LinkedList<CNode> getAllNodesOrdered () {
		LinkedList<CNode> queue = new LinkedList<CNode>(maxNodes);
		LinkedList<CNode> result = new LinkedList<CNode>();

		// breadth-first search on all nodes of this set
		while (queue.size() > 0) {
			CNode c = queue.removeFirst();
			for (CNode p : c.pred) {
				// check whether node p is in this set (it might have
				// been hidden earlier!)
				if (nodes.contains(p) && !queue.contains(p))
					queue.addLast(p);
			}
			// pushing each processed node to front of results
			result.addFirst(c);	
		}
		// result is now a linearization of all nodes of
		// this set s.t. for any two nodes with index i and j
		// holds: i < j implies node(i) < node(j) or node(i) || node(j)
		return result;
	}
	
	public HashSet<CNode> getCutNodes () {
		return cutNodes;
	}
	
	public Collection<CNode> getMaxNodes () {
		return maxNodes;
	}
	
	public void computeSuccRelation (Set<CNode> forNodes, Map< CNode, Set<CNode> > succ) {
		for (CNode c : forNodes) {
			for (CNode p : c.pred ) {
				Set<CNode> p_succ;
				p_succ = succ.get(p); 
				if (p_succ == null) {
					p_succ = new HashSet<CNode>();
					succ.put(p, p_succ);
				}
				p_succ.add(c);
			}
		}
	}
	
	public Map< CNode, Set<CNode> > computeSuccRelation () {
		Map< CNode, Set<CNode> > succ = new HashMap<CNode, Set<CNode>>(this.size());
		computeSuccRelation(nodes, succ);
		return succ;
	}
	
	public Collection< CNode[] > getAllCuts(boolean skipMax) {
		LinkedList< CNode[] > result = new LinkedList<CNode[]>();
		
		CNode cut[] = new CNode[maxNodes.size()];
		maxNodes.toArray(cut);
		
		if (!skipMax)
			result.add(cut);

		getAllCuts(cut, result, computeSuccRelation());
		return result;
	}
	
	public void getAllCuts(CNode[] cut, Collection< CNode[]> allCuts, Map< CNode, Set<CNode> > succ) {
		
		HashSet< CNode > preEvents = new HashSet< CNode >(cut.length);
		for (int i=0; i < cut.length; i++)
			preEvents.addAll(cut[i].pred);
		for (CNode preEvent : preEvents) {
			Set<CNode> postConditions = succ.get(preEvent);
			int postConditionsInCut = 0;
			int preCutSize = cut.length - postConditions.size() + preEvent.pred.size();
			
			int preCutPos = 0;
			CNode preCut[] = new CNode[preCutSize];
			for (int j=0; j < cut.length && preCutPos < preCutSize ;j++)
				if (postConditions.contains(cut[j]))
					postConditionsInCut++;
				else
					// this cut condition will be in the preCut
					preCut[preCutPos++] = cut[j];
			
			if (postConditionsInCut < postConditions.size())
				continue;

			for (CNode preCond : preEvent.pred) {
				preCut[preCutPos++] = preCond;
			}
			allCuts.add(preCut);
			
			getAllCuts(preCut, allCuts, succ);
		}
	}
	
	public Collection<CNode> getEnabledEvents () {
		return getEnabledEvents(cutNodes);
	}
	
	public Collection<CNode> getEnabledEvents (HashSet<CNode> cut) {
		LinkedList<CNode> enabledEvents = new LinkedList<CNode>();
		for (CNode c : nodes) {
			if (c.isEvent && cut.containsAll(c.pred)) {
				assert(c.isEvent == true);
				enabledEvents.add(c);
			}
		}
		return enabledEvents;
	}
	
	public Collection<CNode> getPostSet(CNode e) {
		LinkedList<CNode> postConditions = new LinkedList<CNode>();
		for (CNode c : nodes) {
			if (c.pred.contains(e)) {
				postConditions.add(c);
			}
		}
		return postConditions;
	}
	
	public CNodeSet fire(CNode e) {
		CNodeSet s = CNodeSet.fromCNodeSet_afterCut(this);
		s.cutNodes.removeAll(e.pred);
		s.cutNodes.addAll(s.getPostSet(e));
		
		List<CNode> conflictingNodes = new LinkedList<CNode>();
		for (CNode c : s.maxNodes) {
			if (e.isInConflictTo(c))
				conflictingNodes.add(c);
		}
		s.maxNodes.removeAll(conflictingNodes);
		
		return s;
	}
	
	public HashSet<CNode> fire(CNode e, HashSet<CNode> cut) {
		HashSet<CNode> succCut = new HashSet<CNode>(cut);
		
		succCut.removeAll(e.pred);
		succCut.addAll(getPostSet(e));
		
		return succCut;
	}
	
	public int size() {
		return nodes.size();
	}
	

	@Override
	public boolean equals(Object obj) {
		if (!(obj instanceof CNodeSet))
			return false;
		CNodeSet set = (CNodeSet)obj;
		
		if (maxNodes.size() != set.getMaxNodes().size())
			return false;
		
		for (CNode c : maxNodes) {
			if (!(set.getMaxNodes().contains(c)))
				return false;
		}
		return true;
	}
	
	public boolean equivalentTo(CNodeSet other) {
		if (maxNodes.size() != other.getMaxNodes().size())
			return false;
		if (cutNodes.size() != other.getCutNodes().size())
			return false;
		
		CNode.MatchingRelation mr = new CNode.MatchingRelation();
		if (!nodesContainedIn(maxNodes, other.getMaxNodes(), mr))
			return false;
		
		for (CNode c : cutNodes) {
			if (!other.cutNodes.contains(mr.get(c)))
				return false;
		}
		
		return true;
	}
	
	
	public boolean maxContainedIn(CNodeSet set) {
		return nodesContainedIn(maxNodes, set.getMaxNodes(), null);
	}
	
	public static boolean nodesContainedIn(Collection<CNode> myNodes, Collection<CNode> otherNodes, CNode.MatchingRelation matchingRelation) {

		if (myNodes.size() > otherNodes.size())
			return false;
		
		for (CNode myNode : myNodes) {
			boolean found = false;
			
			Iterator<CNode> it = otherNodes.iterator();
			while (it.hasNext()) {
				CNode maxNode_of_n = it.next();
				if (myNode.strictSuffixOf(maxNode_of_n, matchingRelation))
					found = true;
			}
			
			if (!found)
				return false;
		}
		return true;
	}
	
	public static boolean nodesContainedIn(Collection<CNode> myNodes, CNode[] otherNodes, CNode.MatchingRelation matchingRelation) {

		if (myNodes.size() > otherNodes.length)
			return false;
		
		for (CNode myNode : myNodes) {
			boolean found = false;
			
			for (CNode cutNode_of_n : otherNodes) {
				if (myNode.strictSuffixOf(cutNode_of_n, matchingRelation))
					found = true;
			}
			
			if (!found)
				return false;
		}
		return true;
	}

	/**
	 * extended containment check that returns true also in the case when some of
	 * myNodes are not contained in otherNodes (in case such a node has no predecessor
	 * and at least one node of myNodes is contained in otherNodes)
	 *  
	 * @param myNodes
	 * @param otherNodes
	 * @param matchingRelation
	 * @return
	 */
	public static boolean nodesContainedIn(CNode[] myNodes, Collection<CNode> otherNodes, CNode.MatchingRelation matchingRelation) {

		if (myNodes.length > otherNodes.size())
			return false;

		boolean partialMatch = false;
		for (CNode myNode : myNodes) {
			boolean found = false;
			
			Iterator<CNode> it = otherNodes.iterator();
			while (it.hasNext()) {
				CNode maxNode_of_n = it.next();
				if (myNode.strictSuffixOf(maxNode_of_n, matchingRelation)) {
					found = true;
					partialMatch = true;
				}
			}
			
			if (!found && myNode.pred.size() > 0)
				return false;
		}
		return partialMatch;
	}

	public boolean containedIn(CNodeSet set, Map<CNode, Set<CNode> > possibleMatches) {
		// the other set is too small
		if (this.nodes.size() > set.size())
			return false;
		
		for (CNode n : this.nodes) {
			Integer key = new Integer(n.hashCode());
			
			Collection<CNode> matchByHash = set.get(key);
			if (matchByHash == null)
				return false;
			
			Set<CNode> candidateNodes = new HashSet<CNode>();

			// check all nodes of CNodeSet set that have the same hash value as the current node n
			for (CNode other : matchByHash) {
				// if such a node of CNodeSet set has equal label and matching number of predecessors
				if (n.label.equals(other.label) && ((n.pred.size() == 0) || (n.pred.size() == other.pred.size()))) {
					// add it to the candidates
					candidateNodes.add(other);
				}
			}

			if (candidateNodes.isEmpty())
				// no candidate match: CNodeSet set cannot contain this set
				return false;
			// remember all possible matches for this CNode n
			possibleMatches.put(n, candidateNodes);
		}
		
		for (CNode n : this.maxNodes) {
			//System.err.println(n+" : "+possibleMatches.get(n));
			if (!n.hasPossibleMatches(possibleMatches))
				return false;
		}
		
		return true;
	}
	
	public boolean enabledBy(CNodeSet set) {
		
		Map<CNode, Set<CNode> > possibleMatches = new HashMap<CNode, Set<CNode>>(nodes.size());
		if (!containedIn(set, possibleMatches))
			return false;
		
		boolean activated = false;

		CNodeCutGenerator cng = new CNodeCutGenerator(this.maxNodes, possibleMatches);
		//System.err.println("---cut candidates---");
		while (cng.hasNext()) {
			CNode[] cut = cng.next();
			CNode.MatchingRelation matchingRelation = new CNode.MatchingRelation();
			boolean matchingCut = true;
			
			
			//String cutString = "[";
			for (int i=0;i<cut.length;i++) {

				if (!cng.cutToMatch[i].strictSuffixOf(cut[i], matchingRelation)) {
					matchingCut = false;
					break;
				}
				
				//cutString += cut[i]+",";
			}
			//cutString += "]";
			
			if (matchingCut) {
				for (int i=0;i<cut.length;i++) {
					if (cut[i].activeNode) {
						activated = true;
						//cutString += "*";
					}
				}
			}
			
			//System.err.println(cutString);
		}
		//System.err.println("---end of cut candidates---");
		
		return activated;
	}
	
	public HashSet<CNode> reachedCut(CNode event) {
		if (!event.isEvent)
			return null;
		
		HashSet<CNode> cut = new HashSet<CNode>();
		
		HashSet<CNode> ePast = new HashSet<CNode>();
		event.getPast(ePast);
		ePast.add(event);	  // elementary configuration also contains the event itself
		
		for (CNode c : nodes) {
			if (c.isEvent) continue;
			// if it is part of the past of this event, it is not in the cut
			if (ePast.contains(c)) continue;

			// a condition without predecessors is a minimal condition
			if (c.pred.isEmpty()) {
				cut.add(c);

			// any other condition must have all its predecessors
			// i.e. its single pre-event in the past of this event
			} else if (ePast.containsAll(c.pred))
				cut.add(c);
		}
		return cut;
	}
	
	public LinkedList<CNode[]> findEnablingCuts(HashSet<CNode> patternToFind, HashSet<CNode> knownPartialCut, HashSet<CNode> candidateNodes) {
		
		Map<CNode, Set<CNode> > possibleMatches = new HashMap<CNode, Set<CNode>>(nodes.size());
		if (!gatherPossibleMatches(patternToFind, knownPartialCut,
				                   candidateNodes, possibleMatches))
			return null;
		
		// the mapping possibleMatches now assigns each node of patternToFind
		// possible candidate nodes for matching in the candidate set and uses
		// at least one node from the partial cut
		CNodeCutGenerator cng = new CNodeCutGenerator(patternToFind, possibleMatches);
		LinkedList<CNode[]> result = new LinkedList<CNode[]>();
		while (cng.hasNext())
			result.add(cng.next());
		
		return result;
	}
	
	/**
	 * Find a candidate set of possibly matching nodes for a given set of nodes. The
	 * findings of this methods are stored in the mapping possibleMatches that assigns
	 * to each node of the set toMatch the set of all possibly matching nodes of this
	 * CNodeSet. The set of matching candidates is restricted by
	 *   (1) candidateNodes  - only nodes from this set are considered candidates for
	 *                         matching) and                  
	 *   (2) knownPartialCut - if a node of this set is considered as matching candidate
	 *                         then it is the only matching candidate (truncating
	 *                         search and restricting the resulting mapping) 
	 * 
	 * @param toMatch          nodes for which matching nodes shall be found in this
	 *                         <code>CNodeSet</code>
	 * @param knownPartialCut  nodes of this <code>CNodeSet</code> that uniquely
	 *                         match nodes of <code>toMatch</code> 
	 * @param candidateNodes   only nodes of this set are candidates for matching
	 * @param possibleMatches  the result mapping
	 * @return <code>false</code> iff no matching was found or no node from knownCut
	 *                                is part of the candidate nodes
	 */
	public boolean gatherPossibleMatches(HashSet<CNode> toMatch, HashSet<CNode> knownPartialCut, HashSet<CNode> candidateNodes, Map<CNode, Set<CNode> > possibleMatches) {

		if (toMatch.size() > candidateNodes.size())
			return false;	// don't even try...
		
		boolean usesKnownPartialCut = false;
		for (CNode n : toMatch) {
			// retrieve all nodes that might match CNode n via the
			// hash key index of this CNodeSet
			Integer key = new Integer(n.hashCode());
			
			Collection<CNode> matchByHash = this.get(key);
			if (matchByHash == null)
				return false;
			
			// we found at least one possibly matching node, now
			// check whether this node really matches structurally
			// and whether it is part of the candidateNodes, store
			// all such nodes in the following set
			HashSet<CNode> n_candidateNodes = new HashSet<CNode>();

			// check all nodes of CNodeSet set that have the same hash value as the current node n
			for (CNode other : matchByHash) {
				
				// if such a node of CNodeSet set has equal label and matching number of predecessors
				if (n.label.equals(other.label) && ((n.pred.size() == 0) || (n.pred.size() == other.pred.size()))) {
					
					if (knownPartialCut.contains(other)) {
						// assume a safe system: the current candidate node
						// other has equal labels and matches
						// the preset, it is part of the cut we are considering:
						// it is the only matching candidate
						n_candidateNodes.clear();
						n_candidateNodes.add(other);
						usesKnownPartialCut = true;
						break;
					} else if (candidateNodes.contains(other)) {
						// we found a node in the candidateNodes, add it
						n_candidateNodes.add(other);
					}
				}
			}

			if (n_candidateNodes.isEmpty())
				// no candidate match: CNodeSet set cannot contain this set
				return false;
			
			// remember all possible matches for this CNode n
			possibleMatches.put(n, n_candidateNodes);
		}

		// we found something for each node, check if it involves the cut
		return usesKnownPartialCut;
	}

	/**
	 * instantiate this set CNodeSet by replacing nodes of this
	 * set with nodes of another CNodeSet, the replacement mapping
	 * is provided in the matching relation rel and guarantees
	 * that the replacement preserves labels
	 *  
	 * @param rel
	 * @return the instantiated set (a true copy where nodes have been replaced)
	 */
	public CNodeSet instantiate(CNode.MatchingRelation rel) {
		return instantiate(rel, false);
	}

	/**
	 * instantiate this set CNodeSet by replacing nodes of this
	 * set with nodes of another CNodeSet, the replacement mapping
	 * is provided in the matching relation rel and guarantees
	 * that the replacement preserves labels
	 *  
	 * @param rel
	 * @param hideCut	hide the nodes of the cut and all its predecessors in the
	 *                  instantiated set
	 * @return the instantiated set (a true copy where nodes have been replaced)
	 */
	public CNodeSet instantiate(CNode.MatchingRelation rel, boolean hideCut) {
		CNodeSet inst = new CNodeSet(this.size());
		
		CNode.MatchingRelation copyMap = new CNode.MatchingRelation();
		// initialize copyMap pointing all matching nodes of this
		// CNodeSet to nodes of another CNodeSet
		copyMap.addAll(rel);
		
		// and copy the nodes of this set, while replacing predecessors
		// with corresponding nodes from the copyMap
		for (CNode c : this.getMaxNodes()) {
			inst.maxNodes.add(inst.toCNode(c, copyMap));
		}
		
		// initialize the cut of the instantiated set
		for (CNode c : this.cutNodes) {
			inst.cutNodes.add(copyMap.get(c));
		}

		
		if (hideCut) {
			// and hide the cut and all predecessor nodes of the cut
			HashSet<CNode> possibleMaxNodes = new HashSet<CNode>(inst.cutNodes.size());
			LinkedList<CNode> preCond = new LinkedList<CNode>(inst.cutNodes);
			while (preCond.size() > 0) {
				CNode c = preCond.removeFirst();
				preCond.addAll(c.pred);
				inst.hideNode(c, possibleMaxNodes);
			}
			inst.updateMaxNodes(possibleMaxNodes);
		}
		
		return inst;
	}

	@Override
	public String toString() {
		return nodes.toString()+ " cut: " + cutNodes;
	}
	
	public String toDot () {
		return toDot(new HashSet<CNode>(1));
	}
	
	public String toDot (HashSet<CNode> cutoff) {
		StringBuilder b = new StringBuilder();
		b.append("digraph BP {\n");
		
		b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
		b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
		b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

		String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
		String cutOffFillString = "fillcolor=grey";
		
		b.append("\n\n");
		b.append("node [shape=circle];\n");
		for (CNode n : nodes) {
			if (n.isEvent)
				continue;
			if (n.pred.isEmpty())
				b.append("  c"+n.localId+" ["+tokenFillString+"]\n");
			else
				b.append("  c"+n.localId+" []\n");
				
			b.append("  c"+n.localId+"_l [shape=none];\n");
			b.append("  c"+n.localId+"_l -> c"+n.localId+" [headlabel=\""+n+"\"]\n");
		}
		
		b.append("\n\n");
		b.append("node [shape=box];\n");
		for (CNode n : nodes) {
			if (!n.isEvent)
				continue;
			if (cutoff.contains(n))
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
		for (CNode n : nodes) {
			String prefix = n.isEvent ? "e" : "c";
			for (CNode pre : n.pred) {
				if (pre.isEvent)
					b.append("  e"+pre.localId+" -> "+prefix+n.localId+" [weight=10000.0]\n");
				else
					b.append("  c"+pre.localId+" -> "+prefix+n.localId+" [weight=10000.0]\n");
			}
		}
		b.append("}");
		return b.toString();
	}
}
