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


public class DNodeCutGenerator implements Iterator< DNode[] >{

	public static boolean debug = false;
	
	private DNode[][] possibleMatches;
	private int[] cutIterators;
	protected DNode[] cutToMatch;	// a fixed order of the DNodes for which we want to find a cut
	private DNode[] currentCut;
	private int cutSize;
	
	private HashSet<DNode> currentPasts[];
	
	private int dirty;		// the highest node that has been updated last
							// all nodes with equal or higher index have invalid
							// fields and need to be checked again
	
	private Map<DNode, Set<DNode>> co = null;
	
	private boolean hasSomeCut = true;
	
	@SuppressWarnings("unchecked")
	public DNodeCutGenerator(DNode[] cutPattern, DNode.SortedLinearList cutCandidates,
			/*HashSet<DNode> knownPartialCut, */ Map<DNode, Set<DNode>> coRelation) {

		cutSize = cutPattern.length;
		
		cutToMatch = cutPattern;
		currentCut = new DNode[cutSize];
		cutIterators = new int[cutSize];	// cannot generate array for generic type
		possibleMatches = new DNode[cutSize][];
		dirty = 0;		// all nodes have to be checked
		
		currentPasts = (HashSet<DNode>[]) new HashSet[cutSize];

		// distribute the cutCandidates onto the possible matches
		// create one list for each ID in the cutToMatch
		DNode.SortedLinearList possibleMatchList[] = new DNode.SortedLinearList[cutSize];
		for (int i = 0; i<cutSize; i++)
			possibleMatchList[i] = new DNode.SortedLinearList();

		// then do a JOIN over cutCandidates and cutToMatch on their IDs
		int i = 0;
		Iterator<DNode> candIt = cutCandidates.iterator();
		while (candIt.hasNext() && i < cutSize) {
			// ASSUMPTION: i < k => cutToMatch[i].id < cutToMatch[k].id
			
			DNode cand = candIt.next();

			// move cutToMatch-pointer to the current candidate id
			while (i < cutSize && cand.id > cutToMatch[i].id) i++;
			
			// if pointer is legal and id matches, store candidate
			// as a possible match for the current cutToMatch-node
			if (i < cutSize && cand.id == cutToMatch[i].id) {
				possibleMatchList[i] = possibleMatchList[i].add(cand);
			}
		}
		
		/*
		System.out.println(cutCandidates);
		for (i = 0; i< cutSize; i++)
			System.out.println(i + ":" + possibleMatchList[i]);
		 */

		for (i = 0; i < cutSize; i++) {
			if (possibleMatchList[i].length() == 0) {
				hasSomeCut = false;
				break;
			}

			possibleMatches[i] = new DNode[possibleMatchList[i].length()];

			// store the list of possibleMatches in an array
			int j = 0;
			for (DNode cand : possibleMatchList[i]) {
				possibleMatches[i][j] = cand;	// remember
				j++;
			}
			
			// and initialize the pointers and the initial cut variables
			cutIterators[i] = 0;
			currentCut[i] = possibleMatches[i][0];
			currentPasts[i] = null;
		}
		
		co = coRelation;
		
		if (hasSomeCut)
			ensureNextCut();
	}
	
	public DNodeCutGenerator(DNode[] cutPattern, DNode.SortedLinearList cutCandidates) {
		this(cutPattern, cutCandidates, /* null,*/ null);
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
		
		assert false : "Called DNodeCutGenerator with null concurrency relation.";
		return false;
		
		/*
		HashSet<DNode> cut = new HashSet<DNode>(cutSize);
		
		for (int i=0;i<cutSize;i++)
			cut.add(currentCut[i]);
		
		for (int i=dirty;i<cutSize;i++) {
			
			if (i > 0)
				currentPasts[i] = new HashSet<DNode>(currentPasts[i-1]);
			else
				currentPasts[i] = new HashSet<DNode>();
			
			if (!currentCut[i].isConcurrentTo(cut, currentPasts[i]))
				return false;
			
			dirty++;	// the information for this node index has been updated and
						// is valid
		}
		return true;
		*/
		
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
	}
	
	private void ensureNextCut () {
		while (!isCut()) {
			if (!next(cutSize-1))
				break;	// cancel in case we iterated over all combinations
		}
	}
	
	public DNode[] next() {
		DNode[] result = currentCut.clone();
		if (next(cutSize-1))
			ensureNextCut();
		return result;
	}

	public void remove() {
		// never remove elements from the given set
	}
	
	
}
