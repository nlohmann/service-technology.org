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

import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/**
 * Being replaced by {@link DNodeCutGenerator}.
 * 
 * @author Dirk Fahland
 */
@Deprecated
public class CNodeCutGenerator implements Iterator< CNode[] >{

	public static boolean debug = false;
	
	private CNode[][] possibleMatches;
	private int[] cutIterators;
	protected CNode[] cutToMatch;	// a fixed order of the CNodes for which we want to find a cut
	private CNode[] currentCut;
	private int cutSize;
	
	private HashSet<CNode> currentPasts[];
	
	private int dirty;		// the highest node that has been updated last
							// all nodes with equal or higher index have invalid
							// fields and need to be checked again
	
	private Map<CNode, Set<CNode>> co = null;
	
	private boolean hasSomeCut = true;
	
	@SuppressWarnings("unchecked")
	public CNodeCutGenerator(Set<CNode> cutPattern, Map<CNode, Set<CNode> > cutCandidates,
			HashSet<CNode> knownPartialCut, Map<CNode, Set<CNode>> coRelation) {

		cutSize = cutPattern.size();
		
		cutToMatch = new CNode[cutSize];
		currentCut = new CNode[cutSize];
		cutIterators = new int[cutSize];	// cannot generate array for generic type
		possibleMatches = new CNode[cutSize][];
		dirty = 0;		// all nodes have to be checked
		
		currentPasts = (HashSet<CNode>[]) new HashSet[cutSize];

		int i=0;
		for (CNode c : cutPattern) {
			cutToMatch[i] = c;
			
			Set<CNode> candidates = new HashSet<CNode>(cutCandidates.get(c));
			if (knownPartialCut != null && coRelation != null) {
				Set<CNode> nonCandidates = new HashSet<CNode>();
				// remove all candidate nodes that are not concurrent to a node
				// from the known partial cut
				for (CNode cutNode : knownPartialCut) {
					
					Set<CNode> coSet = coRelation.get(cutNode);
					if (coSet.size() > 0)
						for (CNode other : candidates) {
							
							// a node is never concurrent to itself, so don't check 
							if (other == cutNode)
								continue;
							
							// CNode other is not concurrent to one of the nodes
							// from the known partial cut: remove from the candidate set
							if (!coSet.contains(other)) {
								nonCandidates.add(other);
								//System.out.println("~("+other+" co "+cutNode);
							}
						}
				}
				candidates.removeAll(nonCandidates);
			}
			
			possibleMatches[i] = new CNode[candidates.size()];
			
			int j = 0;
			for (CNode other : candidates) {
				possibleMatches[i][j] = other;
				j++;
			}
			
			cutIterators[i] = 0;
			if (candidates.size() == 0)
				hasSomeCut = false;
			else
				currentCut[i] = possibleMatches[i][0];
			
			currentPasts[i] = null;
			
			i++;
		}
		
		co = coRelation;
		
		if (hasSomeCut)
			ensureNextCut();
	}
	
	public CNodeCutGenerator(Set<CNode> cutPattern, Map<CNode, Set<CNode> > cutCandidates) {
		this(cutPattern, cutCandidates, null, null);
	}

	public boolean hasNext() {
		return hasSomeCut && currentCut[0] != null;
	}

	private boolean next(int i) {
		cutIterators[i]++;
		if (cutIterators[i] < possibleMatches[i].length) {
			currentCut[i] = possibleMatches[i][cutIterators[i]];
			if (dirty > i)
				dirty = i;	// overwrite dirty value, as this method is called with
							// descending parameter i, this end of recursion marks
							// the lowest node index that was updated
			return true;
		} else {
			if (i == 0) {
				currentCut[i] = null;
				return false;
			} else {
				if (next(i-1)) {
					cutIterators[i] = 0;
					currentCut[i] = possibleMatches[i][0];
					return true;
				} else {
					currentCut[i] = null;
					return false;
				}
			}
		}
	}
	
	private boolean isCut_co() {
		for (int i=dirty;i<cutSize;i++) {
			for (int j=0; j < i; j++) {
				if (!co.get(currentCut[i]).contains(currentCut[j]))
					return false;
			}
			dirty++;	// concurrency for this node index (and below) has been
						// verified
		}
		return true;
	}
	
	private boolean isCut() {
		
		if (co != null)
			return isCut_co();
		
		HashSet<CNode> cut = new HashSet<CNode>(cutSize);
		
		for (int i=0;i<cutSize;i++)
			cut.add(currentCut[i]);
		
		for (int i=dirty;i<cutSize;i++) {
			
			if (i > 0)
				currentPasts[i] = new HashSet<CNode>(currentPasts[i-1]);
			else
				currentPasts[i] = new HashSet<CNode>();
			
			if (!currentCut[i].isConcurrentTo(cut, currentPasts[i]))
				return false;
			
			dirty++;	// the information for this node index has been updated and
						// is valid
		}
		
		/*
		for (int i=0;i<cutSize;i++) {
			for (int j=0;j<cutSize;j++) {
				if (i==j)
					continue;
				if (!dirty[i] && !dirty[j])
					continue;
				if (currentCut[j].causallyDependsOn(currentCut[i])) {
					return false;
				}
			}
		}
		*/
		return true;
	}
	
	private void ensureNextCut () {
		while (!isCut()) {
			if (!next(cutSize-1))
				break;	// cancel in case we iterated over all combinations
		}
	}
	
	public CNode[] next() {
		CNode[] result = currentCut.clone();
		if (next(cutSize-1))
			ensureNextCut();
		return result;
	}

	public void remove() {
		// never remove elements from the given set
	}
	
	
}
