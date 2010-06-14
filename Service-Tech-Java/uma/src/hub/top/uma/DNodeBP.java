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

import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys.EventPreSet;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

/**
 * Algorithm for computing McMillan unfoldings of Petri nets ({@link PtNet})
 * and oclet systems ({@link AdaptiveSystem}). It initializes and extends
 * a branching process (stored as a {@link DNodeSet}) by firing events from
 * a given {@link PtNet} or a given {@link AdaptiveSystem}; the construction
 * stops by identifying cut off events after which the branching process
 * need to be explored further.
 * 
 * This class provides different strategies for computing cutOff events that
 * can be set by the <code>options_cutOffStrat_</code> fields.
 * 
 * In addition, behavioral properties of the {@link PtNet} and the
 * {@link AdaptiveSystem} can be checked on the fly by setting the
 * <code>options_checkProperty_</code> fields. 
 * 
 * @author Dirk Fahland
 */
public class DNodeBP {
  
  //// --- search strategies for finding cut-off events
  /**
   *  check for cut-off events using the lexicographic order on events:
   *  compare each candidate event only with events that are predecessors
   *  of the candidate event
   */
  private boolean options_searchStrat_predecessor = false;
	/**
	 *  check for cut-off events using the size of the local configurations:
	 *  compare each candidate event only with events that have been added
	 *  previously and which have a strictly smaller prime configuration
	 */
  private boolean options_searchStrat_size = false;
  /**
   *  check for cut-off events using a lexicographic order in the transition names:
   *  compare each candidate event only with events that have been added
   *  previously and which have a smaller or equal prime configuration, if
   *  two configurations have equal size, then their lexicographic order determines
   *  the order of the configuration
   */
  private boolean options_searchStrat_lexicographic = false;
	
  //// --- equivalence notions for detecting whether two events are equivalent
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  enabling the same sets of events
   */
  private boolean options_cutOffEquiv_eventSignature = false;
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  isomorphism to the maximal depth of the histories in the input system
   */
  private boolean options_cutOffEquiv_history = false;
	/**
	 * check for equivalence of cuts by comparing the reached markings only
	 */
  private boolean options_cutOffEquiv_marking = false;
	/**
	 *  check for equivalence of cuts by comparing the histories of conditions
	 *  with respect to the conditions of the given system
	 */
  private boolean options_cutOffEquiv_conditionHistory = false;
  
  /**
   * compute finite complete prefix until all reachable states have been
   * computed (default: <code>true</code>)
   */
  private boolean options_cutOffTermination_reachability = true;
  /**
   * compute finite complete prefix until finite complete prefix is successor
   * complete and can be folded into an equivalent Petri net
   * (default: <code>false</code>)
   */
  private boolean options_cutOffTermination_synthesis = false;


	//// --- analysis flags
	/**
	 * whether properties shall be checked on the fly at all
	 */
  private boolean options_checkProperties = false;
	/**
	 * check whether the system is unsafe
	 */
  private boolean options_checkProperty_Unsafe = false;
	/**
	 * check whether the system has dead conditions (which have no post-event
	 */
  private boolean options_checkProperty_DeadCondition = false;
	/**
	 * stop BP construction if the property to be checked has been found
	 */
  private boolean options_stopIfPropertyFound = true;

  public StringBuilder log = new StringBuilder();
  
  ////--- fields and members of the branching process construction algorithm
  
  private boolean abort = false;  // set to true to abort construction after this step
  
	/**
	 * A {@link DNode}-compatible representation of the original system for
	 * which the McMillan unfolding shall be constructed.
	 */
	protected DNodeSys			dNodeAS;
	
	/**
	 * The branching process that is extended by this algorithm. 
	 */
	protected DNodeSet		bp;
	
	/**
	 * The explicit concurrency relation of this branching process assigns each
	 * condition of the branching process all its concurrent conditions.
	 */
	protected HashMap<DNode, Set<DNode>> co = new HashMap<DNode, Set<DNode> >();
	
	/**
	 * Summarizes for each node the ids of the nodes in its {@link #co}-set
	 */
	protected HashMap<DNode, Set< Short>> coID = new HashMap<DNode, Set<Short> >();
	

	/**
	 * Initialize branching process construction for an {@link AdaptiveSystem}.
	 * This construction uses lexicographic search order and uses history of
	 * conditions as equivalence notion.
	 * 
	 * @param as
	 */
	public DNodeBP(DNodeSys system) {

		// instantiate DNode representation of the adaptive system
		dNodeAS = system;
		
		initialize();
		
    options_searchStrat_size = true;
		options_cutOffEquiv_conditionHistory = true;
		
    options_checkProperties = true;
    options_checkProperty_Unsafe = false;
    options_checkProperty_DeadCondition = true;
	}
	
  /**
   * configure unfolding algorithm for scenario-based specifications
   */
  public void configure_Scenarios() {
    // search strategy: lexicographic
    options_searchStrat_size = true;
    options_searchStrat_predecessor = false;
    options_searchStrat_lexicographic = true;
    
    // determine equivalence of cuts by history
    options_cutOffEquiv_conditionHistory = true;
    options_cutOffEquiv_marking = false;
  }
	
	 /**
   * configure unfolding algorithm for Petri net semantics
   */
  public void configure_PetriNet() {
    // search strategy: predecessor
    options_searchStrat_size = true;
    options_searchStrat_predecessor = false;
    options_searchStrat_lexicographic = true;
    
    // determine equivalence of cuts by markings
    options_cutOffEquiv_conditionHistory = true;
    options_cutOffEquiv_marking = false;
  }
  
  /**
   * configure unfolding algorithm to construct BP only
   */
  public void configure_buildOnly() {
      options_checkProperties = false;
      options_checkProperty_Unsafe = false;
      options_checkProperty_DeadCondition = false;
      
      options_cutOffTermination_reachability = true;
  }
  
  /**
   * configure unfolding algorithm to construct BP only
   */
  public void configure_checkProperties() {
      options_checkProperties = true;
      options_checkProperty_Unsafe = true;
      options_checkProperty_DeadCondition = true;
  }
  
  /**
   * configure unfolding algorithm to stop if system is unsafe
   */
  public void configure_stopIfUnSafe() {
      options_checkProperties = true;
      options_checkProperty_Unsafe = true;
  }
  
  /**
   * configure unfolding algorithm to stop if system is unsafe
   */
  public void configure_synthesis() {
    options_checkProperties = false;
    options_checkProperty_Unsafe = false;
    options_checkProperty_DeadCondition = false;
      
    options_cutOffTermination_reachability = false;
    options_cutOffTermination_synthesis = true;
  }

	/**
	 * Initialize all members for the branching process construction.
	 * Sets the initial branching process based on the given system and
	 * initializes the explicit concurrency relation for the algorithm.
	 */
	private void initialize() {
	  
	  _debug_executionTimeProfile.append("time;numConditions;numConditions_red;numConditions_red2;maxDim;Tfind;numCuts;Tcuts\n");
	
		// get initial BP
		bp = dNodeAS.initialRun;
		bp.setInitialConditions();
		bp.addInitialNodesToAllNodes();
		
		//System.out.println("max nodes: "+bp.maxNodes);
		
		// initialize explicit concurrency relation
		for (DNode d : bp.maxNodes) {
			co.put(d, new HashSet<DNode>());
			coID.put(d, new HashSet<Short>());
		}
		for (DNode d : bp.maxNodes) {
		  for (DNode d2 : bp.maxNodes) {
		    if (d == d2) continue;
		    co.get(d).add(d2);
		    co.get(d2).add(d);
		    coID.get(d).add(d2.id);
		    coID.get(d2).add(d.id);
		  }
			//updateConcurrencyRelation(d);
		}
    
		
    //System.out.println("----- init done -----");
    //System.out.println(co);
	}
	
	private static DNode getPreEvent(DNode d) {
		return (d.pre.length > 0) ? d.pre[0] : null;
	}
	
	/**
	 * Set the explicit concurrency relation for <code>newNode</code> that
	 * has just been added to the branching process.
	 * 
	 * @param newNode
	 */
	private void updateConcurrencyRelation (DNode newNode) {
		if (newNode.isEvent) return;
		
		// the pre-event of newNode has just been added to the branching process
		DNode preNew = getPreEvent(newNode);
		
		//System.out.println("newNode = "+newNode+", "newNode = "+preNew);
		
		//HashSet<CNode> coset = new HashSet<CNode>();
		Set<DNode> coset = co.get(newNode);
		
		for (DNode existing : bp.getAllConditions()) {
			if (existing == newNode)
				continue;
			
			DNode preExisting = getPreEvent(existing);
			Set<DNode> cosetEx = co.get(existing);
			
			// both have the same predecessor (which is an event)
			if (preNew == preExisting || preNew == null ) {
				// both are concurrent
				coset.add(existing);
				cosetEx.add(newNode);
        
        coID.get(newNode).add(existing.id);
        coID.get(existing).add(newNode.id);
        
				// check for unsafe behavior
				if (options_checkProperty_Unsafe) {
				  if (existing.id == newNode.id) {
				    propertyCheck |= PROP_UNSAFE;
				    System.out.println("found two concurrent conditions with the same name: "+existing+", "+newNode);
				  }
				}
				
				continue;
			}
			
			boolean notCo = false;
			for (DNode p : preNew.pre) {
				if (!cosetEx.contains(p)) {
					notCo = true;
					//System.out.println("~("+existing+" co "+ p+") --> ~("+existing+" co "+newNode+")");
					break;
				}
			}
			// DNode existing is concurrent to every predecessor of
			// DNode newNode's pre-event: both are concurrent
			if (!notCo) {
				coset.add(existing);
				cosetEx.add(newNode);
				
				coID.get(newNode).add(existing.id);
				coID.get(existing).add(newNode.id);
				
        // check for unsafe behavior
        if (options_checkProperty_Unsafe) {
          if (existing.id == newNode.id) {
            propertyCheck |= PROP_UNSAFE;
            System.out.println("found two concurrent conditions with the same name: "+existing+", "+newNode);
          }
        }

			}
		}
		
		co.put(newNode, coset);
	}
	
	/**
	 * This map stores for each local history of an event in the system all
	 * cuts that have been found in the current {@link #step()} of the
	 * branching process construction. {@link #findEnablingCuts(EventPreSet, Map, DNode[], boolean)
	 * fills this map and re-uses found cuts for events with the same local
	 * history. This improves performance in finding all cuts where an event
	 * is enabled.
	 */
	private HashMap<EventPreSet, LinkedList<DNode[]>> cashedCuts = new HashMap<EventPreSet, LinkedList<DNode[]>>();
	
	public StringBuilder _debug_executionTimeProfile = new StringBuilder();
	long t0 = System.currentTimeMillis();
	
	/**
	 * Find all cuts that match the given <code>preConditions</code>.
	 * 
	 * @param preConditions
	 * @param coRelation
	 * @param conditionsToSearch
	 * @param synchronizes
	 * @return
	 */
	@SuppressWarnings("unchecked")
  public LinkedList<DNode[]> findEnablingCuts(EventPreSet preConditions, Map<DNode, Set<DNode>> coRelation, DNodeSetElement conditionsToSearch, boolean synchronizes)
	{
    // check if cuts for the current pre-conditions have already been found
    // in the curren step
    if (cashedCuts.containsKey(preConditions)) {
      // yes, so return the previously found cuts
      return cashedCuts.get(preConditions);
    }
    
//    long _debug_t1 = System.currentTimeMillis();
    
    // we generate three arrays of lists of conditions
    // for each condition 'b' in preConditions, we collect all conditions in this
    // branching process that end with 'b'; only these conditions can be part of
    // a cut that enables an event with having 'preConditions' as pre-set
	  LinkedList<DNode> possibleMatches[] = (LinkedList<DNode>[])(new LinkedList[preConditions.conds.length]);
	  // first optimization: reduces 'possibleMatchList' to those conditions that
	  // can be a extended to a complete cut of that ends with 'preConditions'
	  LinkedList<DNode> canCompleteToCut[] = (LinkedList<DNode>[])(new LinkedList[preConditions.conds.length]);
	  // second optimization: contains only those conditions that can be extended
	  // to a cut that contains at least one new condition, i.e. rules out all cuts
	  // that have been examined in a preceding step
	  LinkedList<DNode> canCompleteToNewCut[] = (LinkedList<DNode>[])(new LinkedList[preConditions.conds.length]);
	  
	  // initialize all lists
	  for (int i = 0; i< preConditions.conds.length; i++) {
	    possibleMatches[i] = new LinkedList<DNode>();
	    canCompleteToCut[i] = new LinkedList<DNode>();
	    canCompleteToNewCut[i] = new LinkedList<DNode>();
	  }
	  
	  // an auxiliary map that helps to store conditions of the current
	  // branching process in the right entry of 'possibleMatches'
	  HashMap<Short, Integer> preConIndex = new HashMap<Short, Integer>();
	  for (int i=0; i<preConditions.conds.length; i++) {
	    // conditions with the same ID as the i-th precondition are
	    // stored in the i-th entry of 'possibleMatches'
	    preConIndex.put(preConditions.conds[i].id, i);
	  }
	  
	  // ==========================================================================
	  // FIRST STEP: find all condition of the current branching process
	  // that end with one of the 'preConditions'
    // ==========================================================================
	  // store each found condition in the corresponding entry of 'possibleMatches'
	  
    // if no restriction: search all conditions
		if (conditionsToSearch == null)
		  conditionsToSearch = bp.getAllConditions();
		
		for (DNode cond : conditionsToSearch) {

		  // no event is enabled at an anti-condition
		  if (cond.isAnti) continue;
		  
      // check whether condition max has an id that also occurs in the preConditions
		  Integer putIndex = preConIndex.get(cond.id);
		  if (putIndex == null) continue;  // no, skip condition
		  
      // consider only conditions which are NOT successors of a cutOff event
      if (options_cutOffTermination_reachability
          && (cond.pre != null && cond.pre.length > 0 && cond.pre[0].isCutOff))
      {
        continue;
      }

      // check whether the branching process condition 'cond' ends with the
      // corresponding condition of 'preConditions' (based on the ID of 'cond')
      if (!cond.endsWith(preConditions.conds[putIndex]))
        continue;   //no: skip
      
      // optimization: check whether 'cond' could in principle participate in a
      // cut that ends with 'preConditions', this holds true only if 'cond' for each
      // condition 'b' of 'preConditions', there exists some condition 'b*' that is
      // concurrent to 'cond' and has the same ID; the set of "concurrent IDs" is
      // stored in coID
      boolean canComplete = true;
      for (int i=0; i<preConditions.conds.length; i++) {
        // don't check for concurrency of its own ID
        if (preConditions.conds[i].id == cond.id) continue;
        // for the i-th ID, 'cond' has no concurrent condition
        if (!coID.get(cond).contains(preConditions.conds[i].id)) {
          canComplete = false;  // cannot complete
          break;
        }
      }
      if (!canComplete)
        continue; // skip all conditions that cannot complete to a cut
      
      // we have a match: put condition of the branching process to the corresponding index 
      possibleMatches[putIndex].addLast(cond);
		}
		// 'possibleMatches' contains all conditions of the branching process that
		// end with one of the 'preConditions' and can in principle participate in
		// a cut that ends with 'possibleMatches'. We have to find all cuts that end
		// with 'preConditions'. To do this, we compute all combinations of all
		// 'possibleMatches' index-wise. 
		
		//System.out.println("possible matches: "+possibleMatches);
		
    // ==========================================================================
		// SECOND STEP: to reduce the computational effort, we remove from
		// 'possibleMatches' all conditions which cannot participate in a cut ending
		// with 'preConditions' by two simple heuristics, this step is optional
    // ==========================================================================
		
   	// --------------------------------------------------------------------------
		// FIRST OPTIMIZATION: a condition 'b' in dimension i of 'possibleMatches'
		// participates in a cut only if in each dimension j < i of 'possibleMatches'
		// there exists a condition 'c' that is concurrent to 'b'. 
    // --------------------------------------------------------------------------
		
		// We compute for each dimension j the set of all conditions that are concurrent
		// to the conditions in j. For a second optimization below, we split these sets of
		// concurrent conditions in old conditions that have been considered in the last step
		// and new conditions that have been added in the last step.
    HashSet<DNode> coNodes_old[] = new HashSet[preConditions.conds.length];
    HashSet<DNode> coNodes_new[] = new HashSet[preConditions.conds.length];
    for (int i=0;i<preConditions.conds.length; i++) {
      coNodes_old[i] = new HashSet<DNode>();
      coNodes_new[i] = new HashSet<DNode>();
    }

//		long _debug_size = 1;
//		long _debug_size2 = 1;
//		long _debug_size3 = 1;
//    int _debug_max = 0;
    
		int min = Integer.MAX_VALUE;

		// check for each dimension i
		for (int i=0;i<possibleMatches.length; i++) {
		  // and each condition in i
		  for (DNode d : possibleMatches[i]) {
        // whether d is concurrent to at least one of the other possible conditions,
	      // for each dimension j < i
        boolean canMatch = true;
        for (int j=0; j<i; j++) {
          // each j < i dimension needs to contain d as co-node          
          if (!coNodes_old[j].contains(d) && !coNodes_new[j].contains(d)) {
            // this one does not: remove from matches
            canMatch = false; break; 
          }
        }
        
        if (canMatch) {
          // all nodes that occur in the co-set of all j < i can complete
          // to a cut
          canCompleteToCut[i].add(d);
          // extend the set of concurrent conditions of dimension i by the
          // co-set of node 'd' - separated into conditions added in the last step()
          if (d._isNew) coNodes_new[i].addAll(co.get(d));
          // and conditions that have already been checked in the last step()
          else coNodes_old[i].addAll(co.get(d));
        }
		  }
//		  _debug_size *= possibleMatches[i].size();
//		  _debug_size2 *= canCompleteToCut[i].size();
		}
		// 'canCompleteToCut' contains only nodes s.t. for all dimensions j < i, each
		// condition 'b' in i is concurrent to at least one condition 'c' in j
		
    // --------------------------------------------------------------------------
    // SECOND OPTIMIZATION: We ignore all cuts that have been checked in an
		// earlier iteration. In such an "old" cut, all conditions b evaluate
		// b._isNew = false. Thus, a condition 'b' is only in this step if it is
		// either "new" by itself or concurrent to a "new" condition condition.
    // --------------------------------------------------------------------------
		
		
    for (int i=0;i<canCompleteToCut.length; i++) {

      // This optimization only works of 'preConditions' describes all preconditions
      // of the event to fire. This is not the case if the event may synchronize with
      // another event.
      if (!synchronizes) {
        for (DNode d : canCompleteToCut[i]) {
          // a new condition can participate in any cut of other conditions
          boolean canMatch_someJ = d._isNew ? true : false;
          // an old conditions needs another new condition in all its cuts
          if (!canMatch_someJ) {
            // check whether some new condition is concurrent to 'd' 
            for (int j=0; j<canCompleteToCut.length; j++) {
              if (i == j) continue; // skip self
              // some new condition of dimension j is concurrent to 'd'  
              if (coNodes_new[j].contains(d)) { canMatch_someJ = true; break; }
            }
          }
          if (canMatch_someJ) {
            // condition 'd' can participate in a cut with a new condition, keep it
            canCompleteToNewCut[i].add(d);
          }
        }
        
      } else {
        // a synchronizing event may depend here only on old conditions as it might
        // synchronize with events depending on new conditions later
        canCompleteToNewCut[i] = canCompleteToCut[i];
      }
      // remember smallest number of candidates per dimension
      min = (canCompleteToNewCut[i].size() < min ? canCompleteToNewCut[i].size() : min);
      
//      _debug_max = (_debug_max > canCompleteToNewCut[i].size() ? _debug_max : canCompleteToNewCut[i].size());
//      _debug_size3 *= canCompleteToNewCut[i].size();
      
    }
		
//		long _debug_t2 = System.currentTimeMillis();
//		System.out.print("#"+_debug_size3+" < #"+_debug_size2+" < #"+_debug_size+" < ["+min+","+_debug_max+"]^"+possibleMatches.length+" of "+DNode.toString(preConditions.conds)+" in "+(_debug_t2-_debug_t1)+"ms ");
		
    // ==========================================================================
    // THIRD STEP: compute all combinations of conditions per dimension that
		// are pairwise concurrent; these conditions form a cut that ends with
		// 'preConditions'
    // ==========================================================================
    //System.out.println("  generate cuts from "+_debug_size3+" conditions");
    
    LinkedList<DNode[]> result;
    if (min > 0)
      result = generateCuts_order(canCompleteToNewCut);
    else
      result = new LinkedList<DNode[]>();
		
//		long _debug_t3 = System.currentTimeMillis();
//    System.out.println(" found "+result.size()+" in "+(_debug_t3-_debug_t2)+"ms");
//    _debug_executionTimeProfile.append((_debug_t3-t0)+";"+_debug_size+";"+_debug_size2+";"+_debug_size3+";"+_debug_max+";"+(_debug_t2-_debug_t1)+";"+result.size()+";"+(_debug_t3-_debug_t2)+"\n");
		
    cashedCuts.put(preConditions, result); // cache the found cuts for later use
		return result;
	}
  
  private LinkedList< DNode[] > generateCuts( LinkedList<DNode> possibleMatchList[] ) {
    
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();
    
    LinkedList< DNode[] > partialCutsOld = new LinkedList<DNode[]>();
    LinkedList< DNode[] > partialCutsNew = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[0]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      if (d._isNew) partialCutsNew.addLast(pCut);
      else partialCutsOld.addLast(pCut);
    }
    
    for (int i=1; i< possibleMatchList.length; i++) {
      
      LinkedList<DNode> next_old = new LinkedList<DNode>();
      LinkedList<DNode> next_new = new LinkedList<DNode>();
      for (DNode d : possibleMatchList[i]) {
        if (d._isNew) next_new.addLast(d);
        else next_old.addLast(d);
      }
      
      LinkedList< DNode[] > extendedCutsOld = new LinkedList<DNode[]>();
      if (i < possibleMatchList.length-1) {
        extendByCo(partialCutsOld, next_old, i, extendedCutsOld);
      } else {
        //System.out.print(" skipping "+partialCutsOld.size()*next_old.size()+" ");
      }

      LinkedList< DNode[] > extendedCutsNew = new LinkedList<DNode[]>();
      extendByCo(partialCutsOld, next_new, i, extendedCutsNew);
      extendByCo(partialCutsNew, next_old, i, extendedCutsNew);
      extendByCo(partialCutsNew, next_new, i, extendedCutsNew);
      
      partialCutsOld = extendedCutsOld;
      partialCutsNew = extendedCutsNew;
    }
    
    return partialCutsNew;
  }
  
  private LinkedList< DNode[] > generateCuts_order( LinkedList<DNode> possibleMatchList[] ) {
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();

    LinkedList< DNode[] > partialCuts = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[0]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      partialCuts.addLast(pCut);
    }
    
    for (int i=1; i< possibleMatchList.length; i++) {
      
      if (i == possibleMatchList.length - 1) {
        LinkedList<DNode> next_old = new LinkedList<DNode>();
        LinkedList<DNode> next_new = new LinkedList<DNode>();
        for (DNode d : possibleMatchList[i]) {
          if (d._isNew) next_new.addLast(d);
          else next_old.addLast(d);
        }
        
        LinkedList<DNode[]> partialCutsOld = new LinkedList<DNode[]>();
        LinkedList<DNode[]> partialCutsNew = new LinkedList<DNode[]>();
        for (DNode[] pCut : partialCuts) {
          boolean hasNew = false;
          for (DNode d : pCut) if (d._isNew) { hasNew = true; break; }
          if (hasNew) partialCutsNew.addLast(pCut);
          else partialCutsOld.addLast(pCut);
        }
        LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
        extendByCo(partialCutsOld, next_new, i, extendedCuts);
        extendByCo(partialCutsNew, next_old, i, extendedCuts);
        extendByCo(partialCutsNew, next_new, i, extendedCuts);
        partialCuts = extendedCuts;
      } else {
        LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
        extendByCo(partialCuts, possibleMatchList[i], i, extendedCuts);
        partialCuts = extendedCuts;
      }
    }
    return partialCuts;
  }
  
  private int getShortest ( LinkedList<DNode> possibleMatchList[], boolean taken[] ) {
    int min = Integer.MAX_VALUE;
    int minIndex = -1;
    for (int i=0; i < possibleMatchList.length; i++) {
      if (taken[i]) continue;
      /*
      if (min > possibleMatchList[i].size()) {
        min = possibleMatchList[i].size();
        minIndex = i;
      }
      */
      return i;
    }
    return minIndex;
  }
  
  private LinkedList< DNode[] > generateCuts_all( LinkedList<DNode> possibleMatchList[] ) {
    
    if (possibleMatchList.length == 0) return new LinkedList<DNode[]>();
    
    boolean[] taken = new boolean[possibleMatchList.length];
    int[] orderIndex = new int[possibleMatchList.length];
    
    int current = 0;
    int minIndex = getShortest(possibleMatchList, taken);
    taken[minIndex] = true;
    
    LinkedList< DNode[] > partialCuts = new LinkedList<DNode[]>();
    for (DNode d : possibleMatchList[minIndex]) {
      DNode[] pCut = new DNode[1]; pCut[0] = d;
      partialCuts.addLast(pCut);
    }
    orderIndex[current] = minIndex;
    current++;
    
    while (current < possibleMatchList.length) {
      LinkedList< DNode[] > extendedCuts = new LinkedList<DNode[]>();
      minIndex = getShortest(possibleMatchList, taken);
      taken[minIndex] = true;
      
      extendByCo(partialCuts, possibleMatchList[minIndex], current, extendedCuts);
      partialCuts = extendedCuts;
      
      orderIndex[current] = minIndex;
      current++;
    }
    
    return partialCuts;
  }

  
  
  private void extendByCo( LinkedList< DNode[]> existingCo, LinkedList<DNode> nextConditions, int size, LinkedList< DNode[] > extendedCo ) {

    int newSize = size + 1;
    for (DNode[] partialCut : existingCo ) {
      for (DNode bNew : nextConditions) {
        boolean inConflict = false;
        for (int i=0;i<size; i++) {
          if (!co.get(bNew).contains(partialCut[i])) {
            inConflict = true; break;
          }
        }
        if (!inConflict) {
          DNode[] extendedCut = new DNode[newSize];
          for (int i=0;i<size;i++) extendedCut[i] = partialCut[i];
          extendedCut[size] = bNew;
          extendedCo.addLast(extendedCut);
          //extendedCo.addLast(DNode.sortIDs(extendedCut));
        }
      }
    }
  }

	/**
	 * Collect the events that are enabled in the current construction step
	 * together with the locations of the branching process where this event
	 * is enabled.
	 *  
	 * Also stores whether a set of events is synchronously enabled at a given
	 * location.  
	 */
	class EnablingInfo {
		public LinkedList< DNode > enabledEvents = new LinkedList<DNode>();
		public LinkedList< DNode[] > enablingLocation = new LinkedList<DNode[]>();
		
		public LinkedList< LinkedList<DNode> >
						synchronizedEvents = new LinkedList< LinkedList<DNode> >();
	}
	
	/**
	 * Find all enabled events and their locations of enabling in the
	 * current branching process.
	 * 
	 * @return
	 */
	private EnablingInfo getAllEnabledEvents() {
		EnablingInfo info = new EnablingInfo();
		
    //System.out.println(co);

		short lastEnabledID = -1;
		int beginIDs = 0;
		for (DNode e : dNodeAS.fireableEvents)
		{
		
			//System.out.println("ENABLED "+e+" "+DNode.toString(e.pre)+":");
			LinkedList<DNode[]> cuts = findEnablingCuts(dNodeAS.eventPreSetAbstraction.get(e), co, null, e.isAnti);
			//System.out.println("checking enabling locations...");
			
			//Iterable<DNode> cutNodes = bp.maxNodes;
			
		  for (DNode[] cutNodes : cuts) {
		    
        //System.out.println("  @ "+DNode.toString(cutNodes) +" ???");
  
  			// see if this event is already present at the given cut
  
		    boolean allCutOff = true;
		    boolean foundCutOff = false;
		    boolean noCompletenessPred = true; 
		    
  			int foundNum = 0;
  			for (DNode b : cutNodes)
  			{
  			  if (!b.isCutOff) allCutOff = false;
  			  if (b.isCutOff) foundCutOff = true;
  				if (b.post != null) {
  					for (int j = 0; j < b.post.length; j++)
  						if (b.post[j].id == e.id) {
  							foundNum++;
  							break;
  						}
  				}
  				if (b.pre != null && b.pre.length > 0) {
  				  DNode event = b.pre[0];
  				  if (!event.isCutOff) {
  				    noCompletenessPred = false;
  				  }
  				}
  			}
  			// we found a post-event with the same ID as the transition that
  			// we want to fire at every node of the current location: the transition
  			// was already fired here, so skip this one
  			if (foundNum == cutNodes.length) {
  			  //System.out.println("foundNum == cutNodes.length");
  			  //System.out.println(e+" is already present at "+DNode.toString(cutNodes)+ ", not firing ");
  				continue;	
  			}
  			if (allCutOff == true) {
  			  // the early cut-off event detection in #findEnablingCuts did not
  			  // apply, but an event that depends only on cut-off events definitely
  			  // does not need to be considered, e.g. for synthesis
  			  //System.out.println("skipping "+DNode.toString(cutNodes)+" because all are cut-offs");
  			  continue;
  			}
  			if (foundCutOff && noCompletenessPred) {
  			  // the event depends on a cut-off event and none of its predecessor
  			  // events requires dependency completeness; do not explore this event 
  			  System.out.print("#");
  			  continue;
  			}
  			
  			//log.append(e+" has valid cut at "+DNode.toString(cutNodes)+"\n");
  
  			DNode[] loc = new DNode[e.pre.length];
  			  
  			// see if each precondition of event e is
  			// in max BP (current maximal conditions of the current BP)
  			int i=0;
  			for (; i<e.pre.length; i++) {
  				loc[i] = null;
  				
  				// find a matching condition for e.pre[i] in max BP
  				for (DNode b : cutNodes)
  				{
  					if (b.id == e.pre[i].id) {
  						if (b.endsWith(e.pre[i])) {
  							// same ID and "endsWith"
  							loc[i] = b;	// found
  							break;
  						}
  					}
  				}
  				if (loc[i] == null)	{ // no match, cancel
  				  //System.out.println("no match for "+ e.pre[i]);
  					break;
  				}
  			} // finish searching for matching preconditions in max BP
  			
  			if ( i == e.pre.length ) {
  				// all precondition of event i have been found in max BP
  			  
  	      //log.append(e+" is enabled at "+DNode.toString(cutNodes)+"\n");
  				
  				assert loc[e.pre.length] != null : "Error, adding invalid enabling location";
  			
  				//System.out.println("  event is enabled");
  				if (lastEnabledID != e.id) {
  
            // skip to fire any anti-event that occurs as first event at that location, i.e.
            // the anti-event would not block any other event
            if (!e.isAnti) {
    					// this is the first event with "name"
    					lastEnabledID = e.id;
    					info.enablingLocation.addLast(loc);
    					info.enabledEvents.addLast(e);
    
    					info.synchronizedEvents.addLast(null);
    					
    					// remember first entry of events with this name
    					beginIDs = info.enabledEvents.size()-1;
            }
            
  				} else {
  				  
  				  // we have multiple events with the same id that are enabled
  				  // synchronize them properly
  				  
            // search for an event with the same ID to synchronize with
            int syncEntry = -1;
  				  
  				  if (!e.isAnti) {
  				    // history-based synchronization: two enabled events synchronize
  				    // iff they have the same ID and occur at the same enabling location
  				    // if they synchronize, their synchronized occurrence results in one event
  				    // if they don't synchronize, each occurs as a separate event
  				    
    					for (int j=beginIDs; j<info.enablingLocation.size(); j++) {
    						if (Arrays.equals(loc, info.enablingLocation.get(j))) {
    							syncEntry = j;
    							break;
    						}
    					}
            } else {
              // history-contained synchronization: two enabled events synchronize
              // iff they have the same ID and one enabling location is contained in
              // the other enabling location
              
              for (int j=beginIDs; j<info.enablingLocation.size(); j++) {
                DNode[] otherLoc = info.enablingLocation.get(j);
                if (containedIn(loc, otherLoc)) {
                  syncEntry = j;
                  break;
                }
              }
            }
  				  
            // if there is a synchronizing event synchronize, their synchronized occurrence
  				  // results in one event; if they don't synchronize, each occurs as a separate event
  					if (syncEntry == -1) {
              // this is the first event with "name" at this location

              // skip to fire any anti-event that occurs as first event at that location, i.e.
  					  // the anti-event would not block any other event
  					  if (!e.isAnti) {
    						info.enablingLocation.addLast(loc);
    						info.enabledEvents.addLast(e);
    
    						info.synchronizedEvents.addLast(null);
  					  }
  						
  					} else {
  						// this is the second/n-th event with "name" at the same
  						// firing location
  						// we have to synchronize these events upon firing,
  						// prepare corresponding list
  						LinkedList<DNode> sync = info.synchronizedEvents.get(syncEntry);
  						if (sync == null) {
  							sync = new LinkedList<DNode>();
  							sync.addLast(info.enabledEvents.get(syncEntry));
  	
  							// set list of events for synchronization at the
  							// corresponding entry in the lists 
  							info.synchronizedEvents.set(syncEntry, sync);
  						}
  						// insert current event into list of synchronization events
  						sync.addLast(e);
  					}
  				}
  			} else {
  			  //System.out.println("  incomplete match");
  			}
		  } // for all cuts
		}
		
		for (Entry<EventPreSet, LinkedList<DNode[]>> cuts : cashedCuts.entrySet()) cuts.getValue().clear();
		cashedCuts.clear();
		
		// set all existing conditions to old
		for (DNode b : bp.allConditions) {
		  b._isNew = false;
		}
		
		return info;
	}
	
	private static boolean containedIn(DNode[] first, DNode[] second) {
    
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
	 * Fire all events from the given {@link EnablingInfo}.
	 * 
	 * @param info
	 * @return the number of created events in the branching process
	 */
	private int fireAllEnabledEvents(EnablingInfo info) {
		if (info.enabledEvents.size() == 0)
			return 0;
		
		HashMap<Integer, LinkedList<Integer> > priorityQueue = new HashMap<Integer, LinkedList<Integer> >();
		int maxSize = 0;
		for (int i=0; i<info.enabledEvents.size(); i++) {
		  int configSize = bp.getConfigSize(info.enablingLocation.get(i));
		  maxSize = (maxSize > configSize ? maxSize : configSize);
		  if (!priorityQueue.containsKey(configSize)) {
		    priorityQueue.put(configSize, new LinkedList<Integer>() );
		  }
		  priorityQueue.get(configSize).addLast(i);
		}
		
		int firedEvents = 0;
		
		//for (int fireIndex=0; fireIndex<info.enabledEvents.size(); fireIndex++) {
		for (int i=0;i<=maxSize;i++)
		{
		 if (!priorityQueue.containsKey(i)) continue;
		 // fire all events with priority i
		 for (Integer fireIndex : priorityQueue.get(i)) {

			// flag, set to true iff this event corresponds to an event of an anti-oclet,
			// in this case the event will be added to the branching process (BP) but no
			// subsequent event is fired. In the final result, such anti-events will
			// be removed from the BP. By adding anti-events to the BP, we prevent that
			// an anti-event is "not added" to the BP in every round.
			boolean setAnti = false;
			// like for anti-events, flag that one of the fired events was hot
			boolean setHot = false;
			
			// array containing the post-conditions that are newly created in the BP by
			// firing this event
			DNode[] postConditions = null;
			
			if (info.synchronizedEvents.get(fireIndex) == null) {

				//System.out.println("fire "+info.enabledEvents.get(fireIndex)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex)));
			  //log.append("fire "+info.enabledEvents.get(fireIndex)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex))+"\n");
				DNode e = info.enabledEvents.get(fireIndex);

				if (e.isAnti) {
					// remember that this event was an anti-event
					setAnti = true;
					// System.out.println("not firing "+e+" (is anti)");
				}
				if (e.isHot) {
				  setHot = true;
				}
				postConditions = bp.fire(e, info.enablingLocation.get(fireIndex));

			} else {
				DNode events[] = new DNode[info.synchronizedEvents.get(fireIndex).size()];
				events = info.synchronizedEvents.get(fireIndex).toArray(events);
				
				//System.out.println("fire "+DNode.toString(events)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex)));
				//log.append("fire "+DNode.toString(events)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex))+"\n");
				for (int j=0;j<events.length;j++) {
					if (events[j].isAnti) {
	          // remember that the fired event does not occur in the final result
						//System.out.println("not firing anti-event "+events[j]);
					  setAnti = true;
						// break;
					}
					if (events[j].isHot) {
					  setHot = true;
					}
				}
				postConditions = bp.fire(events, info.enablingLocation.get(fireIndex));
			}
			
			// we did not fire this event
			if (postConditions == null)
				continue;

			// update co-relation for all new post-conditions
			for (DNode d : postConditions) {
				co.put(d, new HashSet<DNode>());
				coID.put(d, new HashSet<Short>());
			}
			for (DNode d : postConditions) {
				updateConcurrencyRelation(d);
			}

			if (postConditions.length > 0) {
			  
  			DNode newEvent = postConditions[0].pre[0];
  			
  			log.append("created new event "+newEvent+" at "+DNode.toString(info.enablingLocation.get(fireIndex))+"\n");
  			log.append("having post-conditions "+DNode.toString(postConditions)+"\n");
  			log.append("from "+toString(newEvent.causedBy)+"\n");
  			
  			setCurrentPrimeConfig(newEvent);
  			newEvent.isHot = setHot;   // remember temperature of event
  			
        // prevent that an anti-event gets successors in
        // the BP, mark each anti-event as a cutoff event
        if (setAnti) {
          newEvent.isAnti = true;
          if (newEvent.isHot) {
            // hot anti-events MUST NOT occur, hence, no successor of hot-anti events occurs
            newEvent.isCutOff = true;
          }
          // System.out.println("created anti-event "+newEvent);
          log.append("created anti-event "+toString(primeConfigurationString.get(newEvent))+"\n");
        } else {
          
          // for all other events, check whether it is a cutOff event
    			if (isCutOffEvent(newEvent)) {
    				newEvent.isCutOff = true;
    				log.append("is a cut-off event because of "+elementary_ccPair.get(newEvent)+"\n");
    				log.append(toString(primeConfigurationString.get(newEvent))+"\n");//" > "+toString(primeConfigurationString.get(elementary_ccPair.get(newEvent)))+"\n");
    			} else {
    			  log.append(toString(primeConfigurationString.get(newEvent))+"\n");
    			}
        }

  			// remember cutOff on post-conditions as well
  			if (newEvent.isCutOff == true)
  				for (DNode b : postConditions) {
  				  //System.out.println("setting cut-off condition "+b+" because of "+newEvent);
  					b.isCutOff = true;
  					b.isAnti = newEvent.isAnti && newEvent.isHot;	// copy the anti-flag
  				}
			} else {
			  currentPrimeCut = null;
			}
			
			firedEvents++;
			
			if (options_checkProperties)
				checkProperties();
		 }
		}
		return firedEvents;
	}
	
	/**
	 * @param e
	 * @param cut
	 * @return
	 *     how often the given event <code>e</code> is enabled in the given
	 *     cut (at different locations)
	 */
	private byte isEnabledIn(DNode e, DNode[] cut)
	{
		byte enabledCount = 0;
		
		DNodeSetElement cut2 = new DNodeSetElement();
		for (DNode b : cut) cut2.addLast(b);
		
		LinkedList<DNode[]> cuts = findEnablingCuts(dNodeAS.eventPreSetAbstraction.get(e), co, cut2, e.isAnti);
			
		for (DNode[] cutNodes : cuts) {

			//// TODO: optimization: do not check enabled events,
			////       but whether all fireable events are already
			////       present in the cut
			/*
			// see if this event is already present at the given cut
	
			int foundNum = 0;
			for (DNode b : cutNodes)
			{
				if (b.post != null)
					for (int j = 0; j < b.post.length; j++)
						if (b.post[j].id == e.id) {
							foundNum++;
							break;
						}
			}
			// we found a post-event with the same ID as the transition that
			// we want to fire at every node of the current location: the transition
			// was already fired here, so skip this one
			if (foundNum == cutNodes.length)
				continue;
			*/	
	
			DNode[] loc = new DNode[e.pre.length];
			  
			// see if each precondition of event e is
			// in max BP (current maximal conditions of the current BP)
			int i=0;
			for (; i<e.pre.length; i++) {
				loc[i] = null;
				
				// find a matching condition for e.pre[i] in max BP
				for (DNode b : cutNodes)
				{
					if (b.id == e.pre[i].id) {
						if (b.endsWith(e.pre[i])) {
							// same ID and "endsWith"
							loc[i] = b;	// found
							break;
						}
					}
				}
				if (loc[i] == null)	// no match, cancel
					break;
			} // finish searching for matching preconditions in max BP
			
			if ( i == e.pre.length ) {
				// all precondition of event i have been found in max BP
				// the event e is enabled here
				enabledCount++;
			}
		}
		return enabledCount;
	}
	
	/**
   * The current primeCut (after firing one event). It is stored longer than
   * the lifetime of the method {@link DNodeBP#isCutOffEvent(DNode)} which
   * sets its value. This way, <code>currentPrimeCut</code> can be read in
   * subsequent analysis steps. 
   */
  private DNode[] currentPrimeCut;
  
  /**
   * The current prime configuration after firing one event. It is stored
   * longer than the lifetime of the method {@link DNodeBP#isCutOffEvent(DNode)}
   * which sets its value. This way, <code>currentPrimeCut</code> can be read in
   * subsequent analysis steps. 
   */
  private HashSet<DNode> currentPrimeConfig;
  
  /**
   * Set fields {@link #currentPrimeCut} {@link #currentPrimeConfig} and update
   * fields {@link #primeConfigurationSize}, {@link #primeConfigurationHash}, and
   * {@link #primeConfigurationString} for 'event' that has just been fired.
   * 
   * @param event
   */
  private void setCurrentPrimeConfig(DNode event) {
    
    boolean computePredecessors = false;
    boolean includeEvent = false;
    
    if (options_searchStrat_predecessor || options_searchStrat_lexicographic) computePredecessors = true;
    if (options_searchStrat_size) includeEvent = true;
    
    currentPrimeCut = bp.getPrimeCut(event, computePredecessors, includeEvent);
    int currentConfigSize = bp.getPrimeConfiguration_size;
    // collected all predecessor events if using the lexicographic search strategy
    currentPrimeConfig = bp.getPrimeConfiguration;
    
    primeConfigurationSize.put(event, currentConfigSize);
    primeConfigurationHash.put(event, hashCut(currentPrimeCut));
    
//    DNode[] currentPrimeConfig_sorted = new DNode[currentPrimeConfig.size()];
//    currentPrimeConfig.toArray(currentPrimeConfig_sorted);
//    DNode.sortIDs(currentPrimeConfig_sorted);
    
    short[] currentPrimeConfigIDs = new short[currentPrimeConfig.size()];
    int i = 0;
    for (DNode e : currentPrimeConfig) {
      currentPrimeConfigIDs[i++] = e.id;
    }
    Arrays.sort(currentPrimeConfigIDs);
    
    //primeConfigurationString.put(event, getLexicographicString_acc(currentPrimeConfig));
    primeConfigurationString.put(event, currentPrimeConfigIDs);
  }
  
  /**
   * Check whether the given event (freshly added to the branching process)
   * is a cut off event according to the chosen search strategy and the
   * chosen equivalence criterion. Requires {@link #setCurrentPrimeConfig(DNode)}
   * to be called for 'event' previously.
   * 
   * @param event
   * @return
   *      true iff <code>event</code> is a cut off event
   */
  private boolean isCutOffEvent(DNode event) {

    // pick a search strategy for determing the cut off event
    if (options_searchStrat_predecessor) {
      
      // pick an equivalence criterion for determining equivalent cuts
      if (options_cutOffEquiv_history) {
        if (findEquivalentCut_history(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_marking) {
        if (findEquivalentCut_marking_predecessor(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_conditionHistory ) {
        int currentConfigSize = bp.getPrimeConfiguration_size;
        if (findEquivalentCut_conditionHistory_signature_size(
            currentConfigSize, event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_predecessor(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
      } else {
        assert false : "No valid search strategy given.";
      }
      
    } else if (options_searchStrat_size) {
      // search for cut off events in all previously constructed events

      // pick an equivalence criterion for determining equivalent cuts
      if (options_cutOffEquiv_conditionHistory ) {
        if (findEquivalentCut_conditionHistory_signature_size(
            primeConfigurationSize.get(event), event, currentPrimeCut, bp.getAllEvents())) {
          return true;
        }
        
      } else if (options_cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_size(
            primeConfigurationSize.get(event), event, currentPrimeCut, bp.getAllEvents())) {
          return true;
        }
        
      } else {
        assert false : "No valid search strategy given.";
      }
    }
    
    return false;
  }

  /**
   * Main algorithm. One step for extending the current branching process.
   *  
   * @return
   *    number of events that have been fired in this step, returns
   *    0 if construction of the branching process is complete.
   */
  public int step() {
    //System.out.println("----- next step -----");
    EnablingInfo info = getAllEnabledEvents();
    int fired = fireAllEnabledEvents(info);
    
    if (abort) return 0;
    
    if (propertyCheck == PROP_NONE || !options_stopIfPropertyFound)
      return fired;
    else
      return 0; // abort if properties violated
  }


	
	/**
	 * @param condition
	 * @param cut
	 * 
	 * @return
	 *     how often <code>cut</code> contains a condition that ends
	 *     with the given <code>condition</code> (and its history) according
	 *     to {@link DNode#endsWith(DNode)}.
	 */
	private byte endsIn(DNode condition, DNode[] cut) {
		byte satisfiedCount = 0;

		for (int i=0; i<cut.length;i++) {
			if (cut[i].id < condition.id)
				continue;
			else if (cut[i].id == condition.id) {
				if (cut[i].endsWith(condition))
					satisfiedCount++;
			} else
				// cut nodes are ordered by their IDs, this ID is too large
				break;
		}
		return satisfiedCount;
	}
	
  /* --------------------------------------------------------------------------
   * 
   *  The equivalence relation on the nodes of the branching process.
   *  
   * --------------------------------------------------------------------------
   */
	
	/**
   * Maps each cut-off event and each post-condition of a cut-off event to its
   * smallest equivalent event (condition) of the branching process.
   * 
   * Two nodes are equivalent iff their futures in the full (infinite)
   * unfolding are isomorphic. Cut-off events that are equivalent to the
   * initial cut have no entry in this map.
   * 
   * This mapping is updated by {@link #updateCCpair(DNode, DNode)} and
   * {@link #updateCCpair(DNode[], DNode[])} and is used for computing foldings
   * of the McMillan prefix ({@link #minimize()} and {@link #relax()}) and
   * for deciding behavioral properties ({@link #checkProperties()}).
   */
  private HashMap<DNode, DNode> elementary_ccPair = new HashMap<DNode, DNode>();

	/**
	 * Update the mapping that relates each new cutOff event to its equivalent
	 * old event. If the equivalent event is a cutOff event as well, follow the
	 * equivalence relation transitively to the first event.
	 * 
	 * @param newEvent
	 *     the new event that is a cutOff event
	 * @param equivalentEvent
	 *     the old event to which <code>newEvent</code> is equivalent
	 */
	private void updateCCpair(DNode newEvent, DNode equivalentEvent) {
	  DNode existing = elementary_ccPair.get(equivalentEvent);
	  if (existing == null)
	    elementary_ccPair.put(newEvent, equivalentEvent);
	  else
	    elementary_ccPair.put(newEvent, existing);
	}
	
	/**
   * Update the mapping that relates each new condition of a cutOff event to
   * its equivalent counterpart condition.
   * 
   * @param newEvent
   *     the new cut-off event
   * @param newConditions
   *     the new conditions
   * @param equivalentEvent
   *     the old conditions to which <code>newConditions</code> are equivalent
   */
  private void updateCCpair(DNode newEvent, DNode[] newConditions, DNode equivalentConditions[]) {
    for (int i=0; i<newConditions.length; i++) {
      
      DNode existing = elementary_ccPair.get(equivalentConditions[i]);
      if (existing == null)
        elementary_ccPair.put(newConditions[i], equivalentConditions[i]);
      else
        elementary_ccPair.put(newConditions[i], existing);
      
      if (newConditions[i] == equivalentConditions[i]) {
        //System.out.println("[DNodeBP:updateCCpair] WARNING: making "+newConditions[i]+" equivalent to itself");
      } else {
        
        // update mapping only for post-conditions of cut-off event
        if (newConditions[i].pre != null && newConditions[i].pre.length > 0
            && newConditions[i].pre[0] != newEvent) {
          continue;
        }
        
        //System.out.println("setting cut-off condition: "+newConditions[i]+" because of "+equivalentConditions[i]);
        newConditions[i].isCutOff = true; // remember it
      }
    }
  }
  
  /* ==========================================================================
   * 
   *  Search Strategies and Equivalence Criteria for finding cut off events.
   *  Each search strategy consists of two methods
   *  
   *    1. The findEquivalentCut_XXX method is called from isCutOffEvent, it
   *       iterates over all candidate events (according to its strategy) for
   *       checking whether the passed event is a cut off event wrt. this
   *       candidate event. This method calls the second method equivalenCuts_XXX.
   *       
   *    2. The equivalenCuts_XXX method gets two cuts as parameters and checks
   *       whether these cuts are equivalent, i.e. their futures would be
   *       isomorphic.
   *       
   *  The methods are written in reverse order as the equivalence criterion
   *  is the core of each search strategy.
   *  
   * ==========================================================================
   */
  
  /* --------------------------------------------------------------------------
   * 
   *  History Equivalence
   *  
   * --------------------------------------------------------------------------
   */
  
  /**
   * History equivalence is an equivalence notion for systems with history
   * dependent enabling conditions of events. Let HIST_MAX denote the length
   * of the longest history of a fireable event in the input system {@link #dNodeAS}.
   * 
   * Two cuts are 'history equivalent' iff their conditions have
   * pairwise isomorphic histories up to depth HIST_MAX
   * 
   * @param newCut
   * @param oldCut
   * @return <code>true</code> iff the cuts have isomorphic histories
   */
  private boolean equivalentCuts_history(DNode[] newCut, DNode oldCut[]) {
    
    if (newCut.length != oldCut.length)
      return false;

    for (int i=0; i < newCut.length; i++) {
      // Compare history of nodes in the cuts, assume that no node has
      // two equally labeled predecessors. Cuts are ordered by their IDs,
      // compare nodes with same index for equality
      if (!newCut[i].equals(oldCut[i], dNodeAS.maxHistoryDepth))
        return false;
    }
  
    return true;
  }

  /**
   * The search strategy for {@link #equivalentCuts_history(DNode[], DNode[])}
   * 
   * @param newEvent  newly added event
   * @param newCut    the prime cut of the newly added event
   * @param eventsToCompare
   *                  the set of candidate events against which we check for
   *                  newEvent being a cut off event
   * @return <code>true</code> iff <code>newEvent</code> is a cut off event
   */
	private boolean findEquivalentCut_history(DNode newEvent, DNode[] newCut, HashSet<DNode> eventsToCompare) {

		if (equivalentCuts_history(newCut, bp.initialCut)) {
		  updateCCpair(newEvent, newCut, bp.initialCut);
			return true;
		}

		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (equivalentCuts_history(newCut, oldCut)) {
			  updateCCpair(newEvent, e); // update the cutOff mapping
			  updateCCpair(newEvent, newCut, oldCut);
				return true;
			}
		}
		
		return false;
	}

  /* --------------------------------------------------------------------------
   * 
   *  Event Signature Equivalence, Predecessor Strategy
   *  
   * --------------------------------------------------------------------------
   */
	
	/**
	 * Two cuts are 'event equivalent' iff the cuts enable exactly the same set
	 * of events of the system {@link #dNodeAS}. We check for equivalence of
	 * sets of enabled events by a signature over the events.
	 * 
	 * An 'event signature' stores for each event how often it is enabled in the
	 * cut. We store this as an array of bytes. The array has length of number
	 * of events of {@link #dNodeAS}. Each array entry stores how often the
	 * respective event is enabled in the cut.
	 * 
	 * This method assumes that it is called within a search scheme for predecessor
	 * search ({@link #options_searchStrat_predecessor}. This way, we can assume
	 * that each old cut against which we check for equivalence has all its
	 * successor events built.
   * TODO: REALLY????
	 * 
	 * @param newCutSignature
	 *     the signature of the new cut, computed once before the comparison begins
	 * @param oldCut
	 *     the prime cut of an existing event against which we compare whether
	 *     the new event is a cut off event     
	 */
  private boolean equivalentCuts_eventSignature_predecessor(byte[] newCutSignature, DNode oldCut[]) {

    int sigIndex = 0;
    
    // first compare on equivalence of all fireable events
    for (DNode e : dNodeAS.fireableEvents) {

      // on predecessor search: "oldCut" has all successor events built
      // count on the old cut the number of occurrences of event 'e' in the
      // post-set of the cut, if it does not match the number of enabled events
      // in the new cut, the cuts are not equivalent
      if (countPostEvents_oldCut(oldCut, e) != newCutSignature[sigIndex])
        return false;
      sigIndex++;
    }
    
    // then check all precondition events for equivalence, this is necessary to
    // achieve completeness in the computed prefix
    for (DNode e : dNodeAS.preConEvents) {
      // events of the precondition are not added to the branching process, we
      // must compute how often each precondition event is enabled in the old cut
      if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
        return false;
      sigIndex++;
    }
    
    return true;
  }
	
  /**
   * The search strategy for {@link #equivalentCuts_eventSignature_predecessor(byte[], DNode[])}.
   * A predecessor based search strategy ({@link #options_searchStrat_predecessor}).
   * 
   * @param newEvent
   * @param newCut
   * @param eventsToCompare
   * @return
   */
	private boolean findEquivalentCut_eventSignature_predecessor(DNode newEvent, DNode[] newCut, HashSet<DNode> eventsToCompare) {
	  // create a new signature for the new cut (compute the numbers of all enabled events)
		byte[] newCutSignature = cutSignature_events_newCut(newCut);

		// check the initial cut for equivalence
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_eventSignature_predecessor(newCutSignature, bp.initialCut)) { 
			  updateCCpair(newEvent, newCut, bp.initialCut); // make new and old cut equivalent
				return true;
			}
		
		// check the prime cuts of all events against which we compare
		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (newCut.length == oldCut.length)
				if (equivalentCuts_eventSignature_predecessor(newCutSignature, oldCut)) {
	        updateCCpair(newEvent, e);     // make new and old event equivalent
	        updateCCpair(newEvent, newCut, oldCut);  // make new and old cut equivalent
					return true;
				}
		}
		
		return false;
	}

	/**
	 * Count the number of post-events of <code>oldCut</code> that
	 * originate in firing the <code>event</code> from {@link #dNodeAS}.
	 * Whether an event of the branching process originates in <code>event</code>
	 * is stored in {@link DNode#causedBy} and {@link DNode#globalId}.
	 * 
	 * @param oldCut
	 * @param event
	 * @return
	 */
	private byte countPostEvents_oldCut(DNode[] oldCut, DNode event) {
    HashSet<DNode> cut = new HashSet<DNode>(oldCut.length);
    for (int i=0; i<oldCut.length; i++) cut.add(oldCut[i]);
    
    // count all post-events of oldCut that originate in event, i.e.
    // check whether the events agree on id and globalId
    HashSet<DNode> postEvents = new HashSet<DNode>();
    for (int i=0; i<oldCut.length; i++) {
      if (oldCut[i].post == null) continue;
      for (int j=0; j<oldCut[i].post.length; j++)
        if (oldCut[i].post[j].id == event.id)
          for (int k=0; k<oldCut[i].post[j].causedBy.length; k++)
            if (oldCut[i].post[j].causedBy[k] == event.globalId)
              postEvents.add(oldCut[i].post[j]);
    }
    
    // check for each event if its entire pre-set lies in the cut, only then
    // the event is a proper post-event of the cut
    byte fullCount = 0;
    for (DNode e : postEvents) {
      boolean violated = false;
      for (DNode b : e.pre)
        if (!cut.contains(b)) {
          violated = true; break;
        }
      if (!violated)
        fullCount++;
    }
    
    return fullCount;
  }


	
  /* --------------------------------------------------------------------------
   * 
   *  Event Signature Equivalence, Lexicographic Strategy
   *  
   * --------------------------------------------------------------------------
   */

  /**
   * Two cuts are 'event equivalent' iff the cuts enable exactly the same set
   * of events of the system {@link #dNodeAS}. We check for equivalence of
   * sets of enabled events by a signature over the events.
   * 
   * An 'event signature' stores for each event how often it is enabled in the
   * cut. We store this as an array of bytes. The array has length of number
   * of events of {@link #dNodeAS}. Each array entry stores how often the
   * respective event is enabled in the cut.
   * 
   * This method assumes that it is called within a search scheme for lexicographic
   * search ({@link #options_searchStrat_size}. This strategy must
   * compute for each event how often it is enabled in the respective cut.
   * (in contrast to {@link #equivalentCuts_eventSignature_predecessor(byte[], DNode[])})
   * 
   * @param newCutSignature
   *     the signature of the new cut, computed once before the comparison begins
   * @param oldCut
   *     the prime cut of an existing event against which we compare whether
   *     the new event is a cut off event     
   */
	private boolean equivalentCuts_eventSignature_lexik(byte[] newCutSignature, DNode oldCut[]) {

    int sigIndex = 0;
    for (DNode e : dNodeAS.fireableEvents) {
      // on lexicographic search: check full enabling of events as
      // checked "oldCut" may not have all successor events built
      //int en_num = isEnabledIn(e, oldCut);
      //System.out.println(e+": "+ en_num + " , "+newCutSignature[sigIndex]);
      if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
        return false;
      sigIndex++;
    }
    
    for (DNode e : dNodeAS.preConEvents) {
      //int en_num = isEnabledIn(e, oldCut);
      //System.out.println(e+": "+ en_num + " , "+newCutSignature[sigIndex]);
      if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
        return false;
      sigIndex++;
    }
    
    return true;
  }
	  

  /**
   * The search strategy for {@link #equivalentCuts_eventSignature_predecessor(byte[], DNode[])}.
   * A size-based search strategy ({@link #options_searchStrat_size}).
   * 
   * @param newCutConfigSize
   *      number of events in the prime configuration of the new event
   * @param newEvent  newly added event
   * @param newCut    the prime cut of the newly added event
   * @param eventsToCompare
   *                  the set of candidate events against which we check for
   *                  newEvent being a cut off event
   * @return <code>true</code> iff <code>newEvent</code> is a cut off event
   */
	private boolean findEquivalentCut_eventSignature_size(
			int newCutConfigSize,
			DNode newEvent,
			DNode[] newCut,
			Iterable<DNode> eventsToCompare)
	{
		
		// TODO: do not compare wrt. enabling of events but wrt. embedding of all
		// preconditions of events, i.e. wrt. equality of the histories that matter
		
		byte[] newEventSignature = cutSignature_events_newCut(newCut);
		
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_eventSignature_lexik(newEventSignature, bp.initialCut)) {
				//System.out.println(toString(newCut)+ " equals initial");
			  updateCCpair(newEvent, newCut, bp.initialCut); // make new and old cut equivalent
				return true;
			}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			
			// do not check the event that has just been added
			// the cuts would be equal...
			if (e == newEvent) continue;
			// an anti-event does not represent a reachable state
			if (e.isAnti) continue;
			
			// newCut is only equivalent to oldCut if the configuration
			// of newCut is larger than the configuration of oldCut
			if (newCutConfigSize <= bp.getPrimeConfiguration_size)
				continue;	// the old one is larger, not equivalent
			
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (newCut.length == oldCut.length) {
				//System.out.println("prime cut of "+e);
				if (equivalentCuts_eventSignature_lexik(newEventSignature, oldCut)) {
					//System.out.println(toString(newCut)+ " equals " + toString(oldCut));
	        updateCCpair(newEvent, e);    // make new and old event equivalent
	        updateCCpair(newEvent, newCut, oldCut); // make new and old cut equivalent
					return true;
				}
			}
		}
		
		return false;
	}
	
	/**
	 * Create a new event signature for the given 'newCut' storing how often
	 * each event of {@link #dNodeAS} is enabled in 'newCut'.
	 * 
	 * @param newCut
	 * @return the new signature
	 */
  private byte[] cutSignature_events_newCut(DNode[] newCut) {
    
    byte signature[] = new byte[dNodeAS.fireableEvents.length()+dNodeAS.preConEvents.length()];

    int sigIndex = 0;
    for (DNode e : dNodeAS.fireableEvents) {
      signature[sigIndex++] = isEnabledIn(e, newCut);
    }
    
    for (DNode e : dNodeAS.preConEvents) {
      signature[sigIndex++] = isEnabledIn(e, newCut);
    }
    
    return signature;
  }
  
  /* --------------------------------------------------------------------------
   * 
   *  Marking Equivalence
   *  
   * --------------------------------------------------------------------------
   */
  
  /**
   * Two cuts are 'marking equivalent' iff their corresponding markings
   * are equal, i.e. the number of occurrences of each label in both cuts
   * is the same.
   * 
   *  @param newCut
   *  @param oldCut
   *  @return
   *      <code>true</code> iff <code>newCut</code> and <code>oldCut</code>
   *      are marking equivalent
   */
  private boolean equivalentCuts_marking(DNode[] newCut, DNode oldCut[]) {
    // cuts of different size cannot be equivalent
    if (newCut.length != oldCut.length) return false;

    // the cuts are ordered by their ids, they have equial numbers of occurrences
    // of each label if they are strictly equal
    for (int i=0; i < newCut.length; i++) {
      if (newCut[i].id != oldCut[i].id)
        return false;
    }
  
    return true;
  }
	
  /**
   * The search strategy for {@link #equivalentCuts_marking(DNode[], DNode[])}.
   * A predecessor based search strategy ({@link #options_searchStrat_predecessor}).
   * 
   * @param newEvent
   * @param newCut
   * @param eventsToCompare
   * @return
   */
	private boolean findEquivalentCut_marking_predecessor(DNode newEvent, DNode[] newCut, HashSet<DNode> eventsToCompare) {

		if (equivalentCuts_marking(newCut, bp.initialCut)) {
		  updateCCpair(newEvent, newCut, bp.initialCut);
			return true;
		}
		
		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (equivalentCuts_marking(newCut, oldCut)) {
	      updateCCpair(newEvent, e);    // make new and old event equivalent
	      updateCCpair(newEvent, newCut, oldCut); // make new and old cut equivalent
				return true;
			}
		}
		
		return false;
	}
	
  /* --------------------------------------------------------------------------
   * 
   *  Condition History Equivalence
   *  
   * --------------------------------------------------------------------------
   */

	/**
	 * Two cuts are 'condition history equivalent' iff the cuts end with the same
	 * set of conditions from {@link #dNodeAS}. This is an optimized equivalence
	 * criterion for equivalence of cuts.
	 * 
	 * Two cuts are generally equivalent iff their futures are isomorphic. For a
	 * given system {@link #dNodeAS}, the futures of two cuts are isomorphic iff
	 * we can extend either cut with exactly the same events from {@link #dNodeAS}.
	 * An event can be appended to a cut iff the cut ends with (see {@link DNode#endsWith(DNode))
	 * the pre-conditions of the event. Thus two cuts are equivalent if they end
	 * with exactly the same pre-conditions of fireable events of #dNodeAS.
	 * 
	 * We optimize the equivalence check by comparing condition signatures of cuts.
	 * A condition signature stores for each pre-condition of a fireable event of
	 * #dNodeAS how often the cut ends with this pre-condition.
	 * 
	 * This method compares the condition signatures of two cuts. 
	 * 
	 * The signatures of the prime cut of the new event is computed on the fly
	 * and partial results are stored in <code>newCutSignature</code>. The signature
	 * gets extended in consecutive calls of this method.
	 * 
	 * @param newCutSignature
	 * @param newCut
	 * @param oldCut 
	 */
	private boolean equivalentCuts_conditionSignature_history(byte[] newCutSignature, DNode newCut[], DNode oldCut[]) {

    //System.out.println(toString(newCutSignature));
	  
	  // Checking condition equivalence is costly, we want to avoid calling
	  // DNode#endsWith as much as possible.
	  
	  // First check do a shallow check whether the cuts can be equivalent
	  // at all by comparing their markings. If they are not marking equivalent,
	  // they are not condition equivalent. 
    for (int i = 0; i < newCut.length; i++) {
      if (newCut[i].id != oldCut[i].id) return false;
    }
    
    // the cuts are marking equivalent
    
    // Now compare the condition signature of the cut. The signature of the
    // new cut may not have been computed yet. Compute it on the fly.
    // Cancel comparison and computation of signature if we find a condition
    // for which the cuts are not equivalent.
    for (int i = 0; i < newCutSignature.length; i++) {
      // init signature entry for newCut if not already done
      if (newCutSignature[i] == (byte)255)
        newCutSignature[i] = endsIn(dNodeAS.preConditions[i], newCut);
      
      // compare signature entry for newCut with value for oldCut 
      if (newCutSignature[i] != endsIn(dNodeAS.preConditions[i], oldCut))
        return false;
    }
    
    return true;
  }

	/**
	 * Compare two prime configurations of equal size whether one is lexicographically
	 * smaller than the other.
	 * 
	 * @param oldConfig
	 * @param newConfig
	 * @return <code>true</code> iff oldConfig is smaller than newConfig
	 */
	private boolean isSmaller_lexicographic(DNode[] oldConfig, DNode[] newConfig) {
	  if (options_searchStrat_lexicographic == false) return false;
	  if (oldConfig.length != newConfig.length) {
	    System.err.println("Error: lexicographically comparing configurations of different size");
	    return false;
	  }

    // both configurations have same size by assumption
    // iterate over the ID-sorted configurations and compare configs lexicographically
    for (int i=0; i<oldConfig.length; i++) {
      // old is smaller than new at the first difference
      if (oldConfig[i].id < newConfig[i].id) { return true; }
      // old is larger than new at the first difference
      if (oldConfig[i].id > newConfig[i].id) { return false; }
    }
    // old and new are equal until the end
	  return false;
	}
	
	 /**
   * Compare two prime configurations of equal size whether one is lexicographically
   * smaller than the other.
   * 
   * @param oldConfigIDs
   * @param newConfigIDs
   * @return <code>true</code> iff oldConfig is smaller than newConfig
   */
  private boolean isSmaller_lexicographic(short[] oldConfigIDs, short[] newConfigIDs) {
    if (options_searchStrat_lexicographic == false) return false;
    if (oldConfigIDs.length != newConfigIDs.length) {
      System.err.println("Error: lexicographically comparing configurations of different size");
      return false;
    }

    // both configurations have same size by assumption
    // iterate over the ID-sorted configurations and compare configs lexicographically
    for (int i=0; i<oldConfigIDs.length; i++) {
      // old is smaller than new at the first difference
      if (oldConfigIDs[i] < newConfigIDs[i]) { return true; }
      // old is larger than new at the first difference
      if (oldConfigIDs[i] > newConfigIDs[i]) { return false; }
    }
    // old and new are equal until the end
    return false;
  }
	
	 /**
   * Compare two prime configurations of equal size whether one is lexicographically
   * smaller than the other.
   * 
   * @param oldConfig
   * @param newConfig
   * @return <code>true</code> iff oldConfig is smaller than newConfig
   */
  private boolean isSmaller_lexicographic_acc(byte[] oldConfig, byte[] newConfig) {
    if (options_searchStrat_lexicographic == false) return false;

    // both configurations have same size by assumption
    // iterate over the ID-sorted configurations and compare configs lexicographically
    for (int eventID=0; eventID<oldConfig.length; eventID++) {
      // old has more smaller events than new at the first difference, old is smaller
      if (oldConfig[eventID] > newConfig[eventID]) { return true; }
      // old has less smaller events than new at the first difference, old is larger
      if (oldConfig[eventID] < newConfig[eventID]) { return false; }
    }
    // old and new are equal until the end
    return false;
  }
  
  /**
   * @param configuration
   * @return an id-sorted array telling for each event-id how of the event
   * occurs in the given configuration
   */
  private byte[] getLexicographicString_acc(HashSet<DNode> configuration) {
    byte[] lstring = new byte[dNodeAS.fireableEventIndex.size()];
    for (DNode e : configuration) {
      lstring[dNodeAS.fireableEventIndex.get(e.id)]++;
    }
    return lstring;
  }
	
	/**
	 * @param cut
	 * @return hash value of the given cut
	 */
	private int hashCut(DNode[] cut) {
	  int result = 0;
	  int exp = 1;
	  for (int i=0;i<cut.length;i++) {
	    result += cut[i].id*exp;
	    exp *= 64;
	  }
	  return result;
	}
	
	HashMap<DNode, Integer> primeConfigurationSize = new HashMap<DNode, Integer>();
	
	HashMap<DNode, short[]> primeConfigurationString = new HashMap<DNode, short[]>();
	
	HashMap<DNode, Integer> primeConfigurationHash = new HashMap<DNode, Integer>();
	
  /**
   * The search strategy for {@link #equivalentCuts_conditionSignature_history(byte[], DNode[], DNode[])}.
   * A size-based search strategy ({@link #options_searchStrat_size}).
   * 
   * @param newEvent
   * @param newCut
   * @param eventsToCompare
   * @return
   */
	private boolean findEquivalentCut_conditionHistory_signature_size (
			int newCutConfigSize,
			DNode newEvent,
			DNode[] newCut,
			Iterable<DNode> eventsToCompare)
	{

		byte[] newCutSignature = cutSignature_conditions_init255();
		
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, bp.initialCut)) {
			  updateCCpair(newEvent, newCut, bp.initialCut);
				return true;
			}
		
		// compute the lexikographic string that represents the prime configuration
		// of 'newEvent'
//    DNode[] currentPrimeConfig_sorted = null;
    int newEventHash = primeConfigurationHash.get(newEvent);
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			
			// do not check the event that has just been added
			// the cuts would be equal...
			if (e == newEvent) continue;
			  
      // newCut is only equivalent to oldCut if the configuration of newCut
      // is (lexicographically) larger than the configuration of oldCut
      if (!primeConfigurationSize.containsKey(e)) {
        continue;
      }
      if (newCutConfigSize < primeConfigurationSize.get(e)) {
        // the old one is larger, not equivalent
        continue;
      }
      
      // compare reached states by their hash values
      // only if hash values are equal, 'newEvent' and 'e' could be equivalent 
      if (primeConfigurationHash.get(e) != newEventHash)
        continue;

			DNode[] oldCut = bp.getPrimeCut(e, options_searchStrat_lexicographic, options_searchStrat_lexicographic);
			
			if (newCutConfigSize == bp.getPrimeConfiguration_size) {
        // both configurations have the same size:
			  
        // if not lexicographic, cannot be cut-off
			  if (!options_searchStrat_lexicographic) continue;
			  
//			  // compute sorted prime config only when really needed
//			  if (currentPrimeConfig_sorted == null) {
//			    currentPrimeConfig_sorted = new DNode[currentPrimeConfig.size()];
//			    currentPrimeConfig.toArray(currentPrimeConfig_sorted);
//			    DNode.sortIDs(currentPrimeConfig_sorted);
//			  }
//			  
// 	      HashSet<DNode> oldPrimeConfig = bp.getPrimeConfiguration;
// 	      DNode[] oldPrimeConfig_sorted = new DNode[oldPrimeConfig.size()];
// 	      oldPrimeConfig.toArray(oldPrimeConfig_sorted);
// 	      DNode.sortIDs(oldPrimeConfig_sorted);
        
        if (!isSmaller_lexicographic(primeConfigurationString.get(e), primeConfigurationString.get(newEvent)))
          continue;
			}
			
			if (newCut.length == oldCut.length)
				if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, oldCut)) {
	        updateCCpair(newEvent, e); // update the cutOff mapping
	        updateCCpair(newEvent, newCut, oldCut);
					return true;
				}
		}
		
		return false;
	}
	
	 
  public static String toString(int[] array) {
    String result = "[";
    for (int i=0;i<array.length;i++) {
      if (i > 0) result += ",";
      result+=array[i];
    }
    return result+"]";
  }
	
	public static String toString(short[] array) {
	  String result = "[";
	  for (int i=0;i<array.length;i++) {
	    if (i > 0) result += ",";
	    result+=array[i];
	  }
	  return result+"]";
	}

	/**
	 * @return
	 *   a fresh condition signature stating that the number of occurrences
	 *   of conditions in a cut is unknown (value 255)
	 */
	private byte[] cutSignature_conditions_init255() {
		byte signature[] = new byte[dNodeAS.preConditions.length];

		for (int i=0; i<signature.length; i++)
			signature[i] = (byte) 255;
		return signature;
	}
	
  /* ==========================================================================
   * 
   *  Operations on the completely constructed McMillan prefix.
   * 
   * ==========================================================================
   */
   	
	
	/**
	 * Remove all nodes from the branching process that are not part of
	 * its minimal complete finite prefix. 
	 */
	public void minimize() {

	  System.out.println("minimize()");
	  
	  // run a breadth-first search from the maximal nodes of the branching
	  // process to its initial nodes, for each node that has no equivalent
	  // smaller node yet, seek an equivalent image node,  
	  LinkedList<DNode> queue = new LinkedList<DNode>();
	  queue.addAll(bp.maxNodes);
	  
	  while (!queue.isEmpty()) {
	    DNode d = queue.removeFirst();
	    DNode e = elementary_ccPair.get(d);
	    
	    
	    if (e == null) {   // d has no equivalent partner yet, see if there is one

	      if (d.post == null || d.post.length == 0) {
	        // d has no successor, search in all maximal nodes for an equivalent node
	        for (DNode d_img : bp.maxNodes) {
	          if (d_img == d) continue;
	          if (d_img.id != d.id) continue;
	          // do not map normal nodes to non-existing nodes
	          if (d_img.isAnti != d.isAnti) continue; 
	          
	          // d and d0 are nodes with the same ID and without successor
	          if (elementary_ccPair.get(d_img) != null) continue;
	          // d0 also has no equivalent node, e.g. is not a cut-off node
	          // then d and d0 are equivalent
	          
	          System.out.println(d+" --> "+d_img+" (max nodes)");
	          elementary_ccPair.put(d, d_img);
	          d.isCutOff = true;
	        }
	        continue;
	        // TODO: shouldn't we add 'd's successors to the queue?
	      }
	      
	      // 'd' has successor nodes, an equivalent node 'd_img' must have the
	      // corresponding equivalent successors, we find 'd_img' by searching in
	      // the equivalent nodes of 'd's successors for a node with the same id
	      // as the node 'd'
        DNode d_img = null;
	      
	      // see if all successors of 'd0' have an equivalent node
	      // and whether the equivalent nodes have all the same predecessor
	      
	      // for each successor 'd.post' of 'd'
	      // find the equivalent image node 'dPost_img'
	      DNode[] dPost_img = new DNode[d.post.length];
	      for (int i=0; i<d.post.length; i++) {
	        dPost_img[i] = elementary_ccPair.get(d.post[i]);
	      }
	      
	      // then see if all images of the successors of 'd' have a common
	      // predecessor
	      
	      // there is no image of the first successor of 'd': we cannot minimize
	      if (dPost_img[0] == null)
	        continue;  // no equivalent partner of the first successor

	      // find the predecessor of the image of 'd's first successor  that has
	      // same id (name) as DNode 'd', this predecessor should be 'd_img'
	      System.out.print("checking 0: "+dPost_img[0]+"...");
	      for (int j=0; j<dPost_img[0].pre.length; j++)
	        if (dPost_img[0].pre[j].id == d.id) {
	          d_img = dPost_img[0].pre[j];
	          break;
	        }
	      System.out.println(" found "+d_img);
	      
	      // the image of the first successor of 'd' has no predecessor with
	      // the same id as 'd', there is no node we can make equivalent to 'd'
	      if (d_img == null) continue;  // no match :(
	      
	      // the found image has a different number of successors,
	      // they cannot be equivalent
	      if (d_img.post.length != d.post.length) continue;
	      
	      // now check whether each image of all successors of 'd' also have
	      // 'd_img' as their predecessor
	      boolean notFound = false;
	      for (int i=1; i<dPost_img.length; i++) {
          boolean foundPreFor_i = false;
          System.out.print("checking "+i+": "+dPost_img[i]+"...");
          if (dPost_img[i] != null)
  	        for (int j=0; j<dPost_img[i].pre.length; j++) {
  	          if (dPost_img[i].pre[j] == d_img) {
  	            // the i-th successor has 'd_img' as predecessor
  	            System.out.println(" found again");
  	            foundPreFor_i = true; break; 
  	          }
  	        }
          if (!foundPreFor_i) {
            System.out.println(" found not");
            // if the ith successor does not have 'd_img' as predecessor,
            // then we have no node we can make equivalent to 'd' 
            notFound = true;
            break;
          }
	      }
	      
	      if (notFound)
	        continue;  // no match :(
	      
	      System.out.println(d+" --> "+d_img+" (successor)");
	      elementary_ccPair.put(d, d_img);
	      d.isCutOff = true;
	      // extended equivalence relation by 'd' ~ 'd_img' 
	    }
      // and continue with all predecessors of 'd'
      for (DNode dPre : d.pre)
        if (!queue.contains(dPre)) queue.addLast(dPre);
	  } // END: search all nodes of the branching process 
	  
	  HashSet<DNode> seen = new HashSet<DNode>();
	  queue.addAll(bp.initialConditions);
	  while (!queue.isEmpty()) {
	    DNode d = queue.removeFirst();
	    seen.add(d);
	    
	    if (!d.isCutOff && (d.pre.length > 0)) {
	      boolean allPreAreCutOff = true;
	      for (DNode dPre : d.pre) {
	        if (!dPre.isCutOff) { allPreAreCutOff = false; break; }
	      }
	      if (allPreAreCutOff) {
	        // all predecessors are marked as cutOff, but this one not
	        // this node actually resides beyond the minimal prefix (there
	        // is some other configuration that is a cut-off configuration and
	        // this event extends the other configuration)
	        
	        // so map this node to the corresponding node
	        // take any predecessor of d, get its corresponding equivalent 
	        // node 'preEquiv' and map 'd' to the corresponding successor
	        // of 'preEquiv'
	        DNode preEquiv = elementary_ccPair.get(d.pre[0]);
	        for (DNode d0 : preEquiv.post) {
	          if (d0.id == d.id) {
	            System.out.println(d+" --> "+d0+" (initial)");
	            d.isCutOff = true;
	            updateCCpair(d, d0);
	            break;
	          }
	        }
	        // done, continue with d's successors iteratively
	      }
	    }
	  
	    // add all successors of d of which all predecessors have already
	    // been visited
	    if (d.post != null)
  	    for (DNode dPost : d.post) {
  	      boolean seenAll = true;
  	      for (DNode dPostPre : dPost.pre)
  	        if (!seen.contains(dPostPre)) { seenAll = false; break; }
  	      if (seenAll) {
  	        queue.addLast(dPost);
  	      }
  	    }
	  }
  }
	
	/**
	 * The equivalence relation on events and conditions {@link #elementary_ccPair}
	 * is generated based on local knowledge and may be too coarse, e.g. by the
	 * {@link #minimize()} function. We occasionally set two events as equivalent
	 * while their pre-conditions are not pairwise equivalent. For these events
	 * we have to drop the equivalence relation. This is done by this method.
	 */
	public void relax() {
	  
	  for (DNode e : bp.allEvents) {
	    DNode e0 = elementary_ccPair.get(e);
	    if (e0 == null) continue;
	    
	    int i=0;
	    for (; i<e.pre.length; i++) {
	      if (elementary_ccPair.get(e.pre[i]) != e0.pre[i])
	        break;
	    }
	    if (i < e.pre.length) {
	      //System.out.println("events "+e+" and "+e0+" are not fully compatible by "+e.pre[i]+" and "+e0.pre[i]);
	      elementary_ccPair.remove(e);
	      e.isCutOff = false;
	    }
	  }
	}
	
	/* --------------------------------------------------------------------------
	 * 
	 *  Public interface of the branching process, various getters, setters,
	 *  statistics, and output functions.
	 *  
	 * -------------------------------------------------------------------------- */
  
  public int statistic_condNum = 0;
  public int statistic_eventNum = 0;
  public int statistic_cutOffNum = 0;
  public int statistic_arcNum = 0;
	
  /**
   * Compute statistics information for the branching process and store this
   * information in the fields
   *    {@link #statistic_arcNum}
   *    {@link #statistic_condNum}
   *    {@link #statistic_cutOffNum}
   *    {@link #statistic_eventNum}
   * @return a string representation of the statistics
   */
	public String getStatistics() {

		for (DNode n : bp.getAllNodes()) {
			if (n.isEvent) {
			  statistic_eventNum++;
  
  			if (n.isCutOff && !n.isAnti)
  			  statistic_cutOffNum++;
				
				//System.out.println(n+" : "+toString(bp.getPrimeCut(n, null)));
			}
			else statistic_condNum ++;
			
			statistic_arcNum += n.pre.length; 
		}
		return "|B|="+statistic_condNum +" |E|="+statistic_eventNum+" |E_cutOff|="+statistic_cutOffNum+" |F|="+statistic_arcNum;
	}
	
	/**
	 * @return the system that creates the branching process
	 */
	public DNodeSys getSystem() {
	  return dNodeAS;
	}
	
	/**
	 * @return the current branching process
	 */
  public DNodeSet getBranchingProcess () {
    // FIXME: return only after computation finished?
    return bp;
  }

  /**
   * @return the mapping of cutOff events to their equivalent counter parts
   */
  public HashMap<DNode, DNode> getElementary_ccPair() {
    // FIXME: return only after computation finished?
    return elementary_ccPair;
  }
	
	public String toDot() {
		return bp.toDot();
	}
	
	public void printEquivalenceRelation() {
	  System.out.println("--- equivalence relation ---");
	  System.out.println("EVENTS:");
    for (DNode e : bp.getAllEvents()) {
      DNode e0 = elementary_ccPair.get(e);
      System.out.println(e+" -> "+e0);
    }
    System.out.println("CONDITIONS:");
    for (DNode b : bp.getAllConditions()) {
      DNode b0 = elementary_ccPair.get(b);
      System.out.println(b+" -> "+b0);
    }
    System.out.println("--- /equivalence relation ---");
	}
	
/* --------------------------------------------------------------------------
 * 
 *  Analysis routines for checking various temporal properties of the system.
 *  
 * -------------------------------------------------------------------------- */
	
	public static final int PROP_NONE = 0;
	public static final int PROP_UNSAFE = 1;
	public static final int PROP_DEADCONDITION = 2;

	private int propertyCheck = PROP_NONE;

	/**
	 * Entry for checking properties online, while constructing the branching
	 * process. This method is to be called after every firing of an event. It
	 * reads the fields {@link #currentPrimeCut}
	 * 
	 * @return
	 *     <code>false</code> iff an error was detected and the construction
	 *     of the branching process shall be interrupted
	 */
  private boolean checkProperties() {
    
    if (options_checkProperty_Unsafe) {
      
      if (currentPrimeCut == null)
        return true;
      
      // check for unsafe markings
      for (int i=0; i<currentPrimeCut.length-1; i++) {
        // the conditions in the primeCut are ordered by their IDs
        // if two neighboring conditions have equal IDs -> unsafe cut
        if (currentPrimeCut[i].id == currentPrimeCut[i+1].id) {
          propertyCheck |= PROP_UNSAFE;
          System.out.println("Found an unsafe prime cut: "+DNode.toString(currentPrimeCut));
        }
      }
    }
    
    return true;
  }
	
  /**
   * stores all dead conditions that were found in {@link #findDeadConditions(boolean)}
   */
  private LinkedList<DNode> deadConditions;
  
	/**
	 * Check the constructed branching process for containment of dead conditions.
	 * A dead condition is a condition which has no successor event, i.e., where
	 * a token that gets there cannot be consumed. If the source of the system
	 * is a Petri net, the check will exclude all conditions that correspond to
	 * places that have no post-transition, i.e. that are structurally dead.
	 * 
	 * @param first
	 *     quit procedure after first dead condition was found
	 * 
	 * @return
	 * 		<code>true</code> iff the branching process contains a dead condition
	 */
	public boolean findDeadConditions (boolean first) {
	  
	  // do not check for dead conditions unless needed
    if (!options_checkProperty_DeadCondition)
      return false;
		
		// do not check if the BP is unsafe, in this case, the construction
		// was interrupted and the BP is incomplete
		if ((propertyCheck & PROP_UNSAFE) > 0)
			return false;
		
		deadConditions = new LinkedList<DNode>();
		
		// find a non-cut-off condition in the maximal nodes of the BP
		int numDeadConditions = 0;
		for (DNode b : bp.maxNodes) {
			
			if (!b.isCutOff) {
				// this is a non-cut-off condition, see if it corresponds to a
				// terminal place of the source system
				if (!dNodeAS.isTerminal(b)) {
				  // no: this condition is dynamically dead
					System.out.println("Found dead condition: "+b);
	        propertyCheck |= PROP_DEADCONDITION;
					numDeadConditions++;
          deadConditions.add(b);
	        if (first) return true;
				} else {
				  // yes: this condition is structurally dead, no problem due to 'b'
				  
				  // But it may have dead concurrent conditions.
				  // A concurrent condition 'c' of this condition 'b' is dead
				  // wrt. 'b' if 'c' has no successor that is concurrent to 'b'
				  for (DNode c: co.get(b)) {
				    // 'c' is concurrent to 'b', 
				    boolean hasEvent_co_to_c = false;
				    // now see if 'c' has a post-event 'e' that is concurrent to 'b'
				    if (c.post != null)
  				    for (DNode e : c.post) {
  				      boolean not_co_to_b = false;
  				      // event 'e' is concurrent to 'b' iff all pre-conditions 'c2' are
  				      // concurrent to 'b'
  				      for (DNode c2 : e.pre) {
  				        if (!co.get(c2).contains(b)) {
  				          not_co_to_b = true;
  				          break;
  				        }
  				      }
  				      
  				      if (!not_co_to_b) {
  				        // event 'e' is concurrent to 'b', so 'b' is not dead wrt. 'c'
  				        hasEvent_co_to_c = true;
  				        break;
  				      }
  				    }
				    
				    if (!hasEvent_co_to_c) {
				      // Condition 'c' has no event that is concurrent to 'b'
				      // because 'b' has no successor, we have that 'c' has no successor
				      // whenever 'b' is marked. This means 'c' is dead.
				      if (!dNodeAS.isTerminal(c)) {
			          System.out.println("Found dead condition: "+c+" wrt. "+b);
			          propertyCheck |= PROP_DEADCONDITION;
			          numDeadConditions++;
			          deadConditions.add(c);
			          if (first) return true;
				      }
				    }
				  }
				}
			}
		}
		System.out.println("Number of dead conditions: "+numDeadConditions);
		return (numDeadConditions > 0);
	}
	
	/**
	 * @return list of dead conditions found in {@link #findDeadConditions(boolean)}
	 */
	public LinkedList<DNode> getDeadConditions() {
	  return deadConditions;
	}
	

	
	/**
	 * @param d
	 * @return
	 *   all maximal conditions of the counter-example trace that
	 *   reaches the {@link DNode} d, the trace is then all nodes that
	 *   precede the returned conditions
	 */
	public DNode[] getCounterExample(DNode d) {
	  DNode event = null;
	  if (d.isEvent) {
	    event = d;
	  } else {
	    if (d.pre != null && d.pre.length > 0) {
	      event = d.pre[0];
	    } else {
	      return bp.initialCut;
	    }
	  }
	  
	  return bp.getPrimeCut(event, false, false);
	}
	
	/**
	 * Check the constructed branching process for containment of unsafe markings.
	 * The method will iterate over the constructed concurrency relation and check
	 * whether there are two concurrent places with the same ID
	 * 
	 * @return
	 * 		<code>true</code> iff the branching process contains no unsafe marking
	 */
	public boolean isGloballySafe() {
		
		if (!options_checkProperty_Unsafe)
			return true;
		
		// compare all concurrent conditions
		for (Entry<DNode, Set<DNode>> coPairs : co.entrySet()) {
			// and see if two of them have the same ID
			short id = coPairs.getKey().id;
			for (DNode d : coPairs.getValue())
				// yes: then there is a reachable marking putting two tokens
				// on the same place, the system is unsafe
				if (d.id == id) {
				  propertyCheck |= PROP_UNSAFE;
					System.out.println("Found two concurrent conditions with the same name: "+coPairs.getKey()+", "+d);
					return false;
				}
		}
		
		return true;
	}
	
	/**
	 * @return true iff the process is safe
	 */
	public boolean isSafe() {
	  if (!options_checkProperty_Unsafe) return true;
	  return (propertyCheck & PROP_UNSAFE) == 0;
	}
	
	 /**
   * @return true iff the process has a deadlock, call
   * {@link #findDeadConditions(boolean)} first, to find the dead conditions
   */
  public boolean hasDeadCondition() {
    if (!options_checkProperty_DeadCondition) return true;
    return (propertyCheck & PROP_DEADCONDITION) > 0;
  }

  /**
   * @param arr
   * @return a string representation of the given <code>byte</code> array
   */
//  private static String toString(byte[] arr) {
//    String result = "[";
//    for (byte d : arr) {
//      result += d+", ";
//    }
//    return result + "]";
//  }
  
}
