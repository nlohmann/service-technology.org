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

import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Temp;

import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;

public class CNode {
	
	public static int idGen = 0;
	public int localId = idGen++;

	protected String label;
	protected HashSet<CNode> pred = new HashSet<CNode>();
	protected boolean activeNode = false;
	protected boolean isEvent = false;
	
	public CNode (hub.top.adaptiveSystem.Node node, EList<CNode> predecessors) {
		label = node.getName();
		for (CNode p : predecessors) {
			pred.add(p);
		}
		
		if (node.getName().length() > 0 && node.getTemp() != Temp.WITHOUT) {
			activeNode = true;
		}
		if (node instanceof Event) {
			isEvent = true;
		}
	}
	
	public CNode (String aLabel, boolean isEvent) {
		this.label = aLabel;
		this.isEvent = isEvent;
	}
	
	// copy only local fields of a CNode
	private static final int COPY_FLAG_LOCAL_FIELDS = 1;
	// copy references of members of a CNode (but do not copy the referenced objects)
	private static final int COPY_FLAG_CLONE = 2;
	
	/**
	 * private constructor for creating different kinds of copies of a
	 * given <code>{@link CNode}</code> depending on the provided
	 * copyflag
	 * 
	 * @param c 		{@link CNode} to copy
	 * @param copyflag 	controls which fields and members of <code>c</code>
	 *                 	are copied in which way
	 */
	private CNode(CNode c, int copyflag) {
		if ((copyflag & COPY_FLAG_LOCAL_FIELDS) != 0) {
			label = c.label;
			activeNode = c.activeNode;
			isEvent = c.isEvent;
		}
		if ((copyflag & COPY_FLAG_CLONE) != 0) {
			pred.addAll(c.pred);
		}
	}
	
	/**
	 * creates a local copy of the given <code>{@link CNode}</code> without
	 * copying any of its predecessors
	 * 
	 * @param c {@link CNode} to copy
	 */
	public static CNode localCopy(CNode c) {
		return new CNode(c, COPY_FLAG_LOCAL_FIELDS);
	}
	
	/**
	 * creates a shallow copy of the given <code>{@link CNode}</code>, that
	 * duplicates the given node including references to its predecessors
	 * but does not copy the predecessors
	 * 
	 * @param c {@link CNode} to copy
	 */
	public CNode(CNode c) {
		this(c, COPY_FLAG_LOCAL_FIELDS|COPY_FLAG_CLONE);
	}
	
	@Override
	public int hashCode() {
		return label.hashCode();
	}

	@Override
	public boolean equals(Object anObject) {
		if (!(anObject instanceof CNode))
			return false;
		
		CNode n = (CNode)anObject;
		if (!label.equals(n.label))
			return false;
		if (pred.size() != n.pred.size())
			return false;
		for (CNode p : pred) {
			if (!n.pred.contains(p))
				return false;
		}
		return true;
	}
	
	/**
	 * @return label of this node
	 */
	public String getLabel () {
		return label;
	}
	
	public boolean isEvent() {
		return isEvent;
	}
	
	/**
	 * recursively populate the given list <code>past</code> with nodes
	 * that are transitively reachable via the {@code CNode#pred} relation
	 * @param past
	 */
	public void getPast (Set<CNode> past) {
		for (CNode c : pred) {
			if (past.add(c)) c.getPast(past);
		}
	}
	
	public Set<CNode> getPred () {
		return pred;
	}
	
	/**
	 * @return the unique pre-event of this CNode if it exists, 
	 *         <code>null</code> otherwise
	 */
	public CNode getPreEvent () {
		if (isEvent || pred.size() == 0) return null;
		return pred.iterator().next();
	}
	
	/**
	 * replace this node's predecessor CNode p by another CNode newP
	 * @param p
	 * @param newP
	 */
	public void replacePredecessorNode(CNode p, CNode newP) {
		if (pred.remove(p))
			pred.add(newP);
	}

	public boolean getPast_checkConflict (Set<CNode> past) {
		return !getPast_checkConcurrency(past, null);
	}
	
	
	/**
	 * Check whether the current node is concurrent to the given set of nodes
	 * <code>notDepend</code>. The method re-uses information that was gathered
	 * in previous calls of getPast_checkConcurrency to check for conflicts
	 * in the common past of all nodes. If the transitive past of this node
	 * neither contains a node of <code>notDepend</code>, nor is in conflict
	 * to any of their pasts (partially explored in <code>past</code>, then
	 * this node is concurrent to <code>notDepend</code>.
	 * 
	 * The execution of this method extends <code>past</code> with all predecessors
	 * of this node (until a witness for violation of concurrency is found).
	 * 
	 * @param past
	 * @param notDepend
	 * @return true iff this node is concurrent to <code>notDepend</code>
	 */
	private boolean getPast_checkConcurrency (Set<CNode> past, Set<CNode> notDepend) {
		if (past.contains(this)) {
			// this node is part of the already explored past
			// return false if this is not an event, i.e. the
			// past of this node and the explored past are in
			// conflict
			return this.isEvent;
		}
		
		// unknown whether this node is in conflict to the known past
		for (CNode c : pred) {
			
			// check predecessors
			
			// if predecessor is one of the node on which the current must not
			// depend, the the current node is not concurrent wrt. the given
			// past and dependency constraints
			if (notDepend != null && notDepend.contains(c))
				return false;

			// otherwise check whether the predecessor is in conflict or
			// causally dependent
			else if (!c.getPast_checkConcurrency(past, notDepend))
				return false;
			
			// if not, then add the predecessor to the explored past 
			else
				past.add(c);
		}
		
		// no conflicting node found, and no node found on which this node
		// must not depend, concurrency confirmed
		return true;
	}
	
	/**
	 * naive implementation of checking causal dependency by reachability via
	 * predecessor relation
	 * @param other
	 * @return true iff there exists a path from CNode other to this node
	 */
	public boolean causallyDependsOn(CNode other) {
		return causallyDependsOn(other, null);
	}
	
	/**
	 * implementation of checking causal dependency by reachability via
	 * predecessor relation, the method extends the set of nodes that are known
	 * to depend on {@code CNode} <code>other</code> as it visits them and uses
	 * this information to cut-off the search tree wherever possible
	 * 
	 * @param other
	 * @param dependingNodes   set of nodes that is known to depend
	 * 						   on <code>other</code>, can be <code>null</code>
	 * @return true iff there exists a path from CNode other to this node
	 */
	public boolean causallyDependsOn(CNode other, HashSet<CNode> dependingNodes) {
		// this node depends on itself
		if (this == other) {
			if (dependingNodes != null) dependingNodes.add(this);
			return true;
		}
		
		// see if this node is known to depend on CNode other
		if (dependingNodes != null && dependingNodes.contains(this))
			return true;
		
		// recursively search all predecessors
		for (CNode p : pred) {
			if (p.causallyDependsOn(other, dependingNodes)) {
				// and add a node if it does depend on CNode other for further
				// searches
				if (dependingNodes != null) dependingNodes.add(this);
				return true;
			}
		}
		
		return false;
	}
	
	/**
	 * Check whether the current node is concurrent to the given set of <code>otherNodes</code>.
	 * The result is complete only if this method has been called for each node of
	 * <code>otherNodes</code> reusing the results stored in <code>knownPastOhter</code>.
	 * 
	 * @param otherNodes
	 * @param knownPastOther
	 * @return <code>true</code> iff this node is concurrent to <code>otherNodes</code>,
	 *                               reusing information stored in <code>knownPastOther</code>
	 */
	public boolean isConcurrentTo(HashSet<CNode> otherNodes,
			                      HashSet<CNode> knownPastOther)
	{
		return getPast_checkConcurrency(knownPastOther, otherNodes);
	}
	
	public boolean isInConflictTo(CNode other) {
		if (this == other)
			return false;

		HashSet<CNode> myPast = new HashSet<CNode>();
		getPast(myPast);
		if (myPast.contains(other))
			return false;
		// add this node to the past to check against, to make sure
		// that nodes which have this node has a predecessor are not
		// marked as conflicting
		myPast.add(this);	
		if (other.getPast_checkConflict(myPast))
			return true;
		return false;
	}
	
	public boolean hasPossibleMatches(Map<CNode, Set<CNode> > possibleMatches) {
		if (pred.isEmpty())
			return true; // assume possibleMatches is not empty
		
		// reduce possible matches of this node's predecessor
		for (CNode p : pred) {
			if (!p.hasPossibleMatches(possibleMatches)) {
				possibleMatches.remove(this);
				return false;
			}
		}
		
		List<CNode> nonMatchingNodes = new LinkedList<CNode>();
		
		// now check if the possible matches of this node suit
		// the possible matches of this node's predecessors
		for (CNode other : possibleMatches.get(this)) {
			boolean isCandidate = true; // whether CNode other is a matching candidate for this node
			for (CNode p : pred) {
				boolean match_to_other = false;
				for (CNode p_other : other.pred) {
					if (possibleMatches.get(p).contains(p_other))
						match_to_other = true;
				}
				if (!match_to_other) {
					// CNode other has no predecessor node that matches the current
					// predecessor of this node
					isCandidate = false;
					break;
				}
			}
			if (!isCandidate)
				nonMatchingNodes.add(other);
		}
		possibleMatches.get(this).removeAll(nonMatchingNodes);
		
		if (possibleMatches.get(this).isEmpty()) {
			// no possible matches, return false
			possibleMatches.remove(this);
			return false;
		}
		// found at least one possible match
		return true;
	}

	
	public boolean strictSuffixOf(CNode n, MatchingRelation matchingRelation) {

		// check local equality of nodes
		if (!label.equals(n.label))
			return false;
		if ((pred.size() > 0) && pred.size() != n.pred.size())
			return false;
		
		// for each predecessor of this node find a locally equal predecessor
		// of CNode n
		for (CNode p : pred) {
			boolean found = false;
			
			Iterator<CNode> it = n.pred.iterator();
			while (it.hasNext()) {
				CNode pred_of_n = it.next();
				if (p.strictSuffixOf(pred_of_n, matchingRelation)) {
					found = true;
				}
			}
			
			if (!found)
				return false;
		}
		
		if (matchingRelation != null)
			matchingRelation.put(this, n);

		return true;
	}
	
	@Override
	public String toString() {
		return label +"("+localId+")";/*+"(active="+activeNode+")"*/
	}
	
	/**
	 * convenience type of a map from CNodes to CNodes
	 * @author Dirk Fahland
	 */
	public static class MatchingRelation extends BasicEMap<CNode, CNode> {
		private static final long serialVersionUID = 1L;
	};
	
}
