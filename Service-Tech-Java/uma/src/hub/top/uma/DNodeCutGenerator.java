/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. AGPL3.0
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

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;


public class DNodeCutGenerator implements Iterator< DNode[] >{

	public static boolean debug = false;
	
	private DNode[][] possibleMatches;
	private int[] cutIterators;
	protected DNode[] cutToMatch;	// a fixed order of the DNodes for which we want to find a cut
	private DNode[] currentCut;
	private int cutSize;
	private int cutSize_minusOne;
	
	//private HashSet<DNode> currentPasts[];
	
	private int dirty;		// the highest node that has been updated last
							// all nodes with equal or higher index have invalid
							// fields and need to be checked again
	
	private Map<DNode, Set<DNode>> co = null;
	
	private boolean hasSomeCut = true;
	
	public DNodeCutGenerator(DNode[] cutPattern, List<DNode> cutCandidates[],
			/*HashSet<DNode> knownPartialCut, */ Map<DNode, Set<DNode>> coRelation) {

		cutSize = cutPattern.length;
		cutSize_minusOne = cutSize - 1;
    cutToMatch = cutPattern;
		
		initFields(cutPattern, cutCandidates, coRelation);
	}
	
  public DNodeCutGenerator(DNode[] cutPattern, List<DNode>[] cutCandidates) {
    this(cutPattern, cutCandidates, null);
  }
	
	@SuppressWarnings("unchecked")
  public DNodeCutGenerator(DNode[] cutPattern, List<DNode> cutCandidates,
	                         Map<DNode, Set<DNode>> coRelation)
	{
    cutSize = cutPattern.length;
    cutSize_minusOne = cutSize - 1;
    cutToMatch = cutPattern;
	   
    // distribute the cutCandidates onto the possible matches
    // create one list for each ID in the cutToMatch
    LinkedList<DNode> possibleMatchList[] = new LinkedList[cutSize];
    for (int i = 0; i<cutSize; i++)
      possibleMatchList[i] = new LinkedList<DNode>();
  
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
        possibleMatchList[i].add(cand);
      }
    }
     
    //System.out.println(cutCandidates);
    //for (i = 0; i< cutSize; i++)
    //  System.out.println(i + ":" + possibleMatchList[i]);
    
    initFields(cutPattern, possibleMatchList, coRelation);
	}

	private void initFields (DNode[] cutPattern, List<DNode> cutCandidates[],
	                         Map<DNode, Set<DNode>> coRelation)
	{

    currentCut = new DNode[cutSize];
    cutIterators = new int[cutSize];  // cannot generate array for generic type
    possibleMatches = new DNode[cutSize][];
    dirty = 0;    // all nodes have to be checked
    
    //currentPasts = (HashSet<DNode>[]) new HashSet[cutSize];
    for (int i = 0; i < cutSize; i++) {
      if (cutCandidates[i].size() == 0) {
        hasSomeCut = false;
        break;
      }

      possibleMatches[i] = new DNode[cutCandidates[i].size()];

      // store the list of possibleMatches in an array
      int j = 0;
      for (DNode cand : cutCandidates[i]) {
        possibleMatches[i][j] = cand; // remember
        j++;
      }
      
      // and initialize the pointers and the initial cut variables
      cutIterators[i] = 0;
      currentCut[i] = possibleMatches[i][0];
      //currentPasts[i] = null;
    }
    
    co = coRelation;
    
    if (hasSomeCut)
      ensureNextCut();
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
	  //System.out.println("check "+DNode.toString(currentCut));
	  
		for (int i=dirty;i<cutSize;i++) {
			for (int j=0; j < i; j++) {
				if (!co.get(currentCut[i]).contains(currentCut[j])) {
				  //System.out.println(currentCut[i]+" and "+currentCut[j]+" are not concurrent");
					return false;
				}
			}
			dirty++;	// concurrency for this node index (and below) has been
						// verified
		}
		//System.out.println("is cut");
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
		while (!isCut_co()) {
			if (!next(cutSize_minusOne))
				break;	// cancel in case we iterated over all combinations
		}
	}
	
	public DNode[] next() {
		DNode[] result = currentCut.clone();
		if (next(cutSize_minusOne))
			ensureNextCut();
		return result;
	}

	public void remove() {
		// never remove elements from the given set
	}
	
	// =========================================================================
	//  Static functions to compute all cuts from a "comb" of potentially 
	//  concurrent conditions.
	//
	//  The "comb" is given as an array of lists of  conditions. A cut contains
	//  one condition from each list s.t. all conditions are pairwise 
	//  concurrent. The following functions compute all combinations of
	//  conditions of each list and determine to determine all cuts in th
	//  "comb".
  // =========================================================================
	
  // -------------------------------------------------------------------------
	//
	//  To compute the cuts, we use a simple form of dynamic programming. We
	//  compute "partial cuts" of increasing size. Beginning in cuts of size 1,
	//  we extend a list of partial cuts by new conditions as follows:
	//
	//     for each partial cut {b1,...,bk}
	//       for each condition b(k+1)
	//         if b(k+1) is concurrent to each bi, i=1,...,k then
	//           store {b1,...,bk,(k+1)} as an extended cut
	//         else
	//           discard {b1,...,bk,(k+1)}
	//
	//  method #extendByCo implements this procedure.
	//
  // -------------------------------------------------------------------------

	/**
	 * Extend a list of partial cuts of given <code>size</code> to a list of
	 * partial cuts of <code>size + 1</code>. A partial cut {b1,...,bk} in
	 * <code>partialCuts</code> extends to a partial cut {b1,...,bk,b(k+1)} in
	 * <code>extendedCuts</code> iff condition b(k+1) from <code>nextCuts</code>
	 * is concurrent (according to <code>co</code>) to each bi, i=1,...,k.
	 * 
	 * @param partialCuts  list of given partial cuts s.t.
	 *                     <code>partialCuts[i].id &lt; partialCuts[i+1].id</code>
	 *                     for all <code>i=1,...,size</code>
	 * @param nextConditions  list of conditions to extend the partial cuts
	 * @param size  size of all cuts in partial cuts, given explicitly to avoid 
	 *              recomputation from input data
	 * @param co  concurrency relation as computed by {@link DNodeBP}
	 * @param extendedCuts  list to take the results of the extension, can be
	 *                      nonempty; new cuts will be appended to the list
	 */
  public static void extendByCo( LinkedList< DNode[]> partialCuts, LinkedList<DNode> nextConditions, int size, HashMap<DNode, Set<DNode>> co, LinkedList< DNode[] > extendedCuts ) {

    int newSize = size + 1;
    for (DNode[] partialCut : partialCuts ) {
      for (DNode bNew : nextConditions) {
        boolean inConflict = false;
        for (int i=0; i < size; i++) {
          if (!co.get(bNew).contains(partialCut[i])) {
            inConflict = true; break;
          }
        }
        if (!inConflict) {
          DNode[] extendedCut = new DNode[newSize];
          for (int i=0; i < size; i++) extendedCut[i] = partialCut[i];
          extendedCut[size] = bNew;
          extendedCuts.addLast(extendedCut);
        }
      }
    }
  }
  
  /**
   * Standard method to compute all cuts from the conditions in the "comb"
   * <code>possibleMatchList</code>. A <b>cut</b> is a set of conditions {b1,...,bk}
   * where <code>possibleMatchList[i].contains(bi)</code> and all <code>bi</code>
   * are pairwise concurrent: <code>co.get(bi).contains(bj)</code> for all
   * i=1,...k and i != j.
   * 
   * The returned cuts are ordered "lexicographically" by the {@link DNode#globalId}s
   * of the conditions.
   * 
   * @param possibleMatchList
   * @param co
   * @return the list of cuts
   */
  public static LinkedList< DNode[] > generateCuts( LinkedList<DNode> possibleMatchList[], HashMap<DNode, Set<DNode>> co ) {
    
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();

    int current = 0;
    
    // we compute cuts by iteratively constructing partial cuts of increasing size
    // begin with the trivial partial cut for possibleMatchList[0]
    LinkedList< DNode[] > partialCuts = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[0]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      partialCuts.addLast(pCut);
    }
    current++;
    
    // extend the current partial cuts by the next list of conditions to a larger
    // partial cut
    while (current < possibleMatchList.length) {
      LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
      
      extendByCo(partialCuts, possibleMatchList[current], current, co, extendedCuts);
      partialCuts = extendedCuts;
      current++;
    }
    
    // until the complete cuts have been computed, return these
    return partialCuts;
  }
  
  /**
   * Optimized method compute all cuts from the conditions in the "comb"
   * <code>possibleMatchList</code>. 
   * 
   * This method optimizes {@link #generateCuts(LinkedList[], HashMap)}
   * by extending partial cuts with the least number of conditions that
   * have not been considered yet. In other words, for the partial cuts
   * {b1,...,bk}, b1 is taken from the shortest list
   * <code>possibleMatchList[i_1]</code>, b2 is taken from the second to
   * shortest list <code>possibleMatchList[i_2]</code>, etc. 
   * 
   * This way, the number of partial cuts computed intermediately remains
   * smaller than the number of partial cuts computed in {@link #generateCuts(LinkedList[], HashMap)}.
   * 
   * As a drawback, the computed partial cuts are not ordered by their IDs
   * and have to be ordered again before returned. So, in the worst case,
   * this method has a higher complexity than {@link #generateCuts(LinkedList[], HashMap)}.
   * 
   * @param possibleMatchList
   * @param co
   * @return the list of cuts
   */
  public static LinkedList< DNode[] > generateCuts_bySize( LinkedList<DNode> possibleMatchList[], HashMap<DNode, Set<DNode>> co ) {
    
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();
    
    // remember which list of conditions has already been considered
    boolean[] considered = new boolean[possibleMatchList.length];
    
    // determine the shortest list of conditions in the comb
    int minIndex = getShortestList(possibleMatchList, considered);
    considered[minIndex] = true;
    // count the number of dimensions of the comb that have been considered
    int current = 0;
    
    // we compute cuts by iteratively constructing partial cuts of increasing size
    // begin with the trivial partial cut consisting of a minimal set of conditions
    LinkedList< DNode[] > partialCuts = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[minIndex]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      partialCuts.addLast(pCut);
    }
    current++;
    
    // extend the current partial cuts by the next shortest list of conditions
    // to a larger partial cut
    while (current < possibleMatchList.length) {
      LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
      minIndex = getShortestList(possibleMatchList, considered);
      considered[minIndex] = true;
      
      extendByCo(partialCuts, possibleMatchList[minIndex], current, co, extendedCuts);
      partialCuts = extendedCuts;
      
      current++;
    }
    
    // the partial cuts are not sorted by their IDs, do this now
    LinkedList< DNode[] > sortedCuts = new LinkedList<DNode[]>();
    for (DNode[] unsortedCut : partialCuts) {
      sortedCuts.addLast(DNode.sortIDs(unsortedCut));
    }
    
    return sortedCuts;
  }
  
  /**
   * @param possibleMatchList
   * @param considered
   * @return index of the shortest <code>possibleMatchList</code> that has not been
   * <code>considered</code> yet, <code>-1</code> if all have been considered already  
   */
  private static int getShortestList ( LinkedList<DNode> possibleMatchList[], boolean considered[] ) {
    int min = Integer.MAX_VALUE;
    int minIndex = -1;
    // iterate over all lists
    for (int i=0; i < possibleMatchList.length; i++) {
      // skip lists that were considered
      if (considered[i]) continue;  
      // remember the shortest list
      if (min > possibleMatchList[i].size()) {
        min = possibleMatchList[i].size();
        minIndex = i;
      }
      return i;
    }
    return minIndex;
  }
	
	/**
   * Optimized method compute all cuts from the conditions in the "comb"
   * <code>possibleMatchList</code>. 
   * 
   * This method optimizes {@link #generateCuts(LinkedList[], HashMap)}
   * by skipping each cut that consist only of "old" conditions. Such a
   * cut has already been considered in a previous step. 
   * 
   * Old and new conditions are identified by {@link DNode#_isNew}. For
   * correctness of this method, two properties must hold: 
   * 
   * (1) each {@link DNode} <code>d</code> that is added to a branching
   * process evaluates <code>d._isNew == true</code>
   * (2) before firing all enabled events, all conditions of the branching
   * process must be set to <code>d._isNew = false</code>
   * 
   * Thiss method computes and returns less cuts compared to
   * {@link #generateCuts(LinkedList[], HashMap)}. The non-considered
   * cuts would yield only possible extensions of the branching process
   * in {@link DNodeBP} that have already been added. So, this
   * optimized method is complete.
   * 
   * As a drawback, this method returns the cuts in a different order
   * compared to {@link #generateCuts(LinkedList[], HashMap)}: the cuts
   * are not ordedered lexicographically. Thus may lead to cut-off events
   * being identified later in {@link DNodeBP} and hence to larger
   * branching processes. 
   *
	 * @param possibleMatchList
	 * @param co
	 * @return
	 */
  public static LinkedList< DNode[] > generateCuts_noOld( LinkedList<DNode> possibleMatchList[], HashMap<DNode, Set<DNode>> co ) {
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();

    // we compute cuts by iteratively constructing partial cuts of increasing size
    // begin with the trivial partial cut for possibleMatchList[0]
    LinkedList< DNode[] > partialCuts = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[0]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      partialCuts.addLast(pCut);
    }

    // extend the current partial cuts by the next list of conditions to a larger
    // partial cut
    for (int i=1; i< possibleMatchList.length; i++) {
      
      if (i == possibleMatchList.length - 1) {
        // when reaching the last extension step, do not compute
        // those combinations of conditions that yield cuts only
        // consisting of old conditions.
        
        // split the conditions that shall extend the current
        // 'partialCuts' into old and new
        LinkedList<DNode> next_old = new LinkedList<DNode>();
        LinkedList<DNode> next_new = new LinkedList<DNode>();
        for (DNode d : possibleMatchList[i]) {
          if (d._isNew) next_new.addLast(d);
          else next_old.addLast(d);
        }
        
        // and split all partial cuts into cuts consisting only of old
        // conditions and cuts containing at least one new condition
        LinkedList<DNode[]> partialCutsOld = new LinkedList<DNode[]>();
        LinkedList<DNode[]> partialCutsNew = new LinkedList<DNode[]>();
        for (DNode[] pCut : partialCuts) {
          boolean hasNew = false;
          // see if the current partial cut has a new condition
          for (DNode d : pCut) if (d._isNew) { hasNew = true; break; }
          if (hasNew) partialCutsNew.addLast(pCut);
          else partialCutsOld.addLast(pCut);
        }
        // now compute only those combinations that involves at least
        // one new condition
        LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
        extendByCo(partialCutsOld, next_new, i, co, extendedCuts);
        extendByCo(partialCutsNew, next_old, i, co, extendedCuts);
        extendByCo(partialCutsNew, next_new, i, co, extendedCuts);
        partialCuts = extendedCuts;
        
      } else {
        // if not in the last step, compute all combinations including old
        // ones because the last extension step may provide a new condition
        // for the cut
        LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
        extendByCo(partialCuts, possibleMatchList[i], i, co, extendedCuts);
        partialCuts = extendedCuts;
      }
    }
    return partialCuts;
  }
  


	
}
