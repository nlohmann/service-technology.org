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
import hub.top.uma.synthesis.EquivalenceRefineSuccessor;
import hub.top.uma.synthesis.IEquivalentNodesRefine;
import hub.top.uma.synthesis.EquivalenceRefineLabel;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
import java.util.Iterator;
import java.util.LinkedHashSet;
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
  
  /**
   * all configuration options used in the construction of the branching process 
   */
  private final Options options;

  ////--- fields and members of the branching process construction algorithm
  
  /**
   * set to true to abort construction after this step
   */
  private boolean abort = false; 
  
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
	 * @param options set to <code>null</code> for default options
	 */
	public DNodeBP(DNodeSys system, Options options) {

		// instantiate DNode representation of the adaptive system
		dNodeAS = system;
		initialize();
		
		if (options == null)
		  this.options = new Options(system);
		else 
		  this.options = options;
	}
	
  
  /* ==========================================================================
   * 
   *  Main branching process construction algorithm.
   *  
   * ==========================================================================
   */

	/**
	 * Initialize all members for the branching process construction.
	 * Sets the initial branching process based on the given system and
	 * initializes the explicit concurrency relation for the algorithm.
	 */
	private void initialize() {
	  
	  // for computing cuts
	  //_debug_executionTimeProfile.append("time;numConditions;numConditions_red;numConditions_red2;maxDim;Tfind;numCuts;Tcuts\n");
	  // for computing event firings
    _debug_executionTimeProfile.append(
        "time;"
       +"fire event;"+"s/m;"
       +"co;"+"co-checks;"
       +"primeConfig;"
       +"cutOff\n");

		// get initial BP
		bp = dNodeAS.initialRun;
		bp.setInitialConditions();
		bp.addInitialNodesToAllNodes();
		
		//System.out.println("max nodes: "+bp.maxNodes);
		
		// initialize explicit concurrency relation
		//for (DNode d : bp.allConditions) {
		for (DNode d : bp.initialConditions) {
			co.put(d, new HashSet<DNode>());
			coID.put(d, new HashSet<Short>());
		}
		for (DNode d : bp.initialConditions) {
		  for (DNode d2 : bp.initialConditions) {
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
	
	/**
	 * @param b
	 * @return pre-event of condition 'b'
	 */
	private static DNode getPreEvent(DNode b) {
		return (b.pre.length > 0) ? b.pre[0] : null;
	}
	
	private void setConcurrent(DNode existing, Set<DNode> cosetEx, DNode newConditions[]) {
	  
	  Set<Short> cosetIdEx = coID.get(existing);

	  for (DNode dSymm : newConditions) {
      
      if (dSymm == existing) continue;
      
      // both are concurrent
      co.get(dSymm).add(existing);
      cosetEx.add(dSymm);
      
      coID.get(dSymm).add(existing.id);
      cosetIdEx.add(dSymm.id);
      
      // check for unsafe behavior
      if (getOptions().boundToCheck == 1) {
        if (existing.id == dSymm.id) {
          propertyCheck |= PROP_UNSAFE;
          System.out.println("found two concurrent conditions with the same name: "+existing+", "+dSymm);
        }
      }
    }

	}
	
	/**
	 * Set the explicit concurrency relation for <code>newNode</code> that
	 * has just been added to the branching process.
	 * Assumption: <code>newNode.pre != null</code>
	 * 
	 * @param newNode
	 * @param symmetric the same co-relation holds for all nodes in symmetric
	 */
	private void updateConcurrencyRelation (DNode newNode, DNode symmetric[]) {
		if (newNode.isEvent) return;
		
		// the pre-event of newNode has just been added to the branching process
		DNode preNew = (newNode.pre.length > 0) ? newNode.pre[0] : null;
		
		if (preNew == null) {
	    for (DNode existing : bp.getAllConditions()) {
	      if (existing == newNode)
	        continue;
	      
	      // this applies to all nodes in symmetric (including newNode)
	      setConcurrent(existing, co.get(existing), symmetric);
	    }		  
		}
		
		/*
		for (DNode existing : bp.getAllConditions()) {
			if (existing == newNode)
				continue;
			
			_debug_coCheckCount++;
			
			DNode preExisting = (existing.pre.length > 0) ? existing.pre[0] : null;
			
			// both have the same predecessor (which is an event)
			if (preNew == preExisting) {
			  
			  System.out.println("set co "+existing+" || "+DNode.toString(symmetric));
			  
        // both are concurrent
        // this applies to all nodes in symmetric (including newNode)
        setConcurrent(existing, co.get(existing), symmetric);
				continue;
			}
		}
		 */
		
		// collect all nodes that are concurrent to every predecessor node of
		// the pre-event of 'newNode'
		HashSet<DNode> preNewPreCo = new HashSet<DNode>();
		preNewPreCo.addAll(co.get(preNew.pre[0]));
		for (int i=1; i<preNew.pre.length; i++) {
		  preNewPreCo.retainAll(co.get(preNew.pre[i]));
		  
      _debug_coCheckCount += co.get(preNew.pre[i]).size();
		}
 
		for (DNode existing : preNewPreCo) {
		  
		  _debug_coCheckCount++;
			
			// DNode 'existing' is concurrent to every predecessor of
			// DNode newNode's pre-event: 'existing' and 'newNode' are concurrent
        
      // this applies to all nodes in symmetric (including newNode)
      setConcurrent(existing, co.get(existing), symmetric);
		}
		//co.put(newNode, coset);
		
    // check for violation of k-boundedness
    if (getOptions().boundToCheck > 1) {
      int coNum = 0;
      for (DNode other : co.get(newNode)) {
        if (other.id == newNode.id) coNum++;
      }
      
      if (coNum > getOptions().boundToCheck) {
        // found k other concurrent conditions with the same id
        // bound violated
        propertyCheck |= PROP_UNSAFE;
        System.out.println("conditions "+newNode+" has "+coNum+" concurrent conditions with the same name");
      }
    }
	}
	
	/*
	private void updateConcurrencyRelation (DNode newNode, DNode symmetric[]) {
	    if (newNode.isEvent) return;
	    
	    // the pre-event of newNode has just been added to the branching process
	    DNode preNew = (newNode.pre.length > 0) ? newNode.pre[0] : null;
	    
	    //System.out.println("newNode = "+newNode+", "newNode = "+preNew);
	    
	    //HashSet<CNode> coset = new HashSet<CNode>();
	    //Set<DNode> coset = co.get(newNode);
	    
	    for (DNode existing : bp.getAllConditions()) {
	      if (existing == newNode)
	        continue;
	      
	      _debug_coCheckCount++;
	      
	      DNode preExisting = (existing.pre.length > 0) ? existing.pre[0] : null;
	      Set<DNode> cosetEx = co.get(existing);
	      
	      // both have the same predecessor (which is an event)
	      if (preNew == preExisting || preNew == null ) {
	        // both are concurrent
	        
	        // this applies to all nodes in symmetric (including newNode)
	        for (DNode dSymm : symmetric) {
	          
	          if (dSymm == existing) continue;
	          
	          // both are concurrent
	          co.get(dSymm).add(existing);
	          cosetEx.add(dSymm);
	          
	          coID.get(dSymm).add(existing.id);
	          coID.get(existing).add(dSymm.id);
	          
	          // check for unsafe behavior
	          if (options.boundToCheck == 1) {
	            if (existing.id == dSymm.id) {
	              propertyCheck |= PROP_UNSAFE;
	              System.out.println("found two concurrent conditions with the same name: "+existing+", "+dSymm);
	            }
	          }
	        }
	        
	        continue;
	      }
	      
	      boolean notCo = false;
	      for (DNode p : preNew.pre) {
	        _debug_coCheckCount++;
	        if (!cosetEx.contains(p)) {
	          notCo = true;
	          //System.out.println("~("+existing+" co "+ p+") --> ~("+existing+" co "+newNode+")");
	          break;
	        }
	      }
	      // DNode existing is concurrent to every predecessor of
	      // DNode newNode's pre-event: both are concurrent
	      if (!notCo) {
	        // both are concurrent
	        
	        // this applies to all nodes in symmetric (including newNode)
	        for (DNode dSymm : symmetric) {
	          
	          if (dSymm == existing) continue;
	          
	          co.get(dSymm).add(existing);
	          cosetEx.add(dSymm);
	          
	          coID.get(dSymm).add(existing.id);
	          coID.get(existing).add(dSymm.id);
	          
	          // check for unsafe behavior
	          if (options.boundToCheck == 1) {
	            if (existing.id == dSymm.id) {
	              propertyCheck |= PROP_UNSAFE;
	              System.out.println("found two concurrent conditions with the same name: "+existing+", "+dSymm);
	            }
	          }
	        }

	      }
	    }
	    //co.put(newNode, coset);
	    
	    // check for violation of k-boundedness
	    if (options.boundToCheck > 1) {
	      int coNum = 0;
	      for (DNode other : co.get(newNode)) {
	        if (other.id == newNode.id) coNum++;
	      }
	      
	      if (coNum >= options.boundToCheck) {
	        // found k other concurrent conditions with the same id
	        // bound violated
	        propertyCheck |= PROP_UNSAFE;
	        System.out.println("conditions "+newNode+" has "+coNum+" concurrent conditions with the same name");
	      }
	    }
	  }
	*/
	
	/**
	 * This map stores for each local history of an event in the system all
	 * cuts that have been found in the current {@link #step()} of the
	 * branching process construction. {@link #findEnablingCuts(EventPreSet, Map, DNode[], boolean)
	 * fills this map and re-uses found cuts for events with the same local
	 * history. This improves performance in finding all cuts where an event
	 * is enabled.
	 */
	private HashMap<EventPreSet, LinkedList<DNode[]>> cachedCuts = new HashMap<EventPreSet, LinkedList<DNode[]>>();
	
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
    if (cachedCuts.containsKey(preConditions)) {
      // yes, so return the previously found cuts
      return cachedCuts.get(preConditions);
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
      if (getOptions().cutOffTermination_reachability
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
    if (min > 0) {
      if (synchronizes)
        result = DNodeCutGenerator.generateCuts(canCompleteToNewCut, co);
      else
        result = DNodeCutGenerator.generateCuts_noOld(canCompleteToNewCut, co);
    } else
      result = new LinkedList<DNode[]>();
		
//		long _debug_t3 = System.currentTimeMillis();
//    System.out.println(" found "+result.size()+" in "+(_debug_t3-_debug_t2)+"ms");
//    _debug_executionTimeProfile.append((_debug_t3-_debug_t0)+";"+_debug_size+";"+_debug_size2+";"+_debug_size3+";"+_debug_max+";"+(_debug_t2-_debug_t1)+";"+result.size()+";"+(_debug_t3-_debug_t2)+"\n");
		
    cachedCuts.put(preConditions, result); // cache the found cuts for later use
		return result;
	}
  

  public static class SyncInfo {
    public DNode[] events = null;
    public DNode[] loc = null;
    
    public SyncInfo(DNode e, DNode[] loc) {
      events = new DNode[1];
      events[0] = e;
      this.loc = loc;
    }
    
    public boolean synchronizesWith(DNode e, DNode[] loc) {
      if (events[0].id != e.id) return false;
      if (   ( e.isAnti && DNode.containedIn(loc, this.loc)
          || (!e.isAnti && Arrays.equals(loc, this.loc))))
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    
    public void append(DNode e) {
      // event e synchronizes with other events at an existing location, add e to 'events[]'
      int freeLoc = events.length / 2;
      boolean alreadyExists = false;
      for (; freeLoc < events.length; freeLoc++) {
        if (events[freeLoc] == null) break;
        // event 'e' already synchronizes at this location. we are done
        if (events[freeLoc] == e) {
          alreadyExists = true; break;
        }
      }
      
      // skip to next location
      if (alreadyExists) return;
      
      // event 'e' not yet synchronized here, put 'e' into events[otherLoc]
      // extend array if too small
      if (freeLoc == events.length) {
        DNode[] newSyncEvents = new DNode[events.length*2];
        for (int i=0;i<events.length;i++) {
          newSyncEvents[i] = events[i];
        }
        events = newSyncEvents;
      }
      
      // store event 'e'
      events[freeLoc] = e;
    }
    
    public void reduce() {
      // cut off all null entries in the events array 
      int trueSize = 0;
      // find the first entry with null
      for ( ; trueSize < this.events.length; trueSize++)
        if (this.events[trueSize] == null) break;
      // create an array of that size, and copy the events
      DNode[] reduced = new DNode[trueSize];
      for (int i=0; i<trueSize; i++)
        reduced[i] = this.events[i];
      events = reduced;
    }
    
    @Override
    public String toString() {
      return DNode.toString(events)+" @ "+DNode.toString(loc);
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
	public static class EnablingInfo {
		public HashMap< Short, SyncInfo[] > locations;

		
		public EnablingInfo() {
		  locations = new HashMap<Short, SyncInfo[]>();
		}
		
		public void putEnabledEvent(DNode e, DNode[] loc) {
		  
		  //System.out.println("put "+e+"@"+DNode.toString(loc));
		  
		  if (!locations.containsKey(e.id)) {
		    locations.put(e.id, new SyncInfo[1]);
		  }
		  SyncInfo[] locs = locations.get(e.id);
		  
		  boolean addedEvent = false;
		  for (int otherLoc = 0; otherLoc < locs.length; otherLoc++) {
		    if (locs[otherLoc] == null) break;

        // event e synchronizes with other events at an existing location,
		    // add e to this location
		    if (locs[otherLoc].synchronizesWith(e, loc)) {
		      locs[otherLoc].append(e);
		      addedEvent = true;
		    } // event 'e' stored
		  }
		  
		  if (addedEvent) return;
		  
		  // event not added, append its occurrence location to locs
		  if (e.isAnti) {
		    // a single anti-event must not occur (it's redundant)
		    //System.out.println("not adding first anti-event");
		    return;
		  }
		  
		  // find a free spot
		  int freeLoc = locs.length / 2;
		  for (; freeLoc < locs.length; freeLoc++) {
		    if (locs[freeLoc] == null) break;
		  }
		  // no free spot, extend arrays
		  if (freeLoc == locs.length) {
		    SyncInfo[] newLocs = new SyncInfo[locs.length*2];
		    for (int i=0;i<locs.length;i++) {
		      newLocs[i] = locs[i];
		    }
        locations.put(e.id, newLocs);
		    locs = newLocs;
		  }
		  locs[freeLoc] = new SyncInfo(e, loc);
		}
		
		@Override
		public String toString() {
		  short[] enabledIDs = new short[locations.size()];
		  int i = 0;
		  for (Short s : locations.keySet()) {
		    enabledIDs[i++] = s;
		  }
		  Arrays.sort(enabledIDs);
		  
		  StringBuilder b = new StringBuilder();
		  
		  for (i = 0; i<enabledIDs.length; i++) {
		    for (int j=0; j<locations.get(enabledIDs[i]).length; j++) {
		      if (locations.get(enabledIDs[i])[j] == null) continue;
		      b.append(locations.get(enabledIDs[i])[j]+"\n");
		    }
		  }
		  return b.toString();
		}
	}
	
	public class EnablingQueue {
	  
	  public HashMap<Integer, LinkedList<SyncInfo> > queue;
	  private int maxConfigSize;
	  private int minConfigSize;
	  
	  private int queueSize = 0;
	  
	  public EnablingQueue () {
	    queue = new HashMap<Integer, LinkedList<SyncInfo>>();
	    maxConfigSize = 0;
	    minConfigSize = Integer.MAX_VALUE;
	  }
	  
	  public void insertAll(EnablingInfo info) {
	    for (SyncInfo[] sInfos : info.locations.values()) {
	      for (SyncInfo sInfo : sInfos) {
	        if (sInfo != null)
	          insert(sInfo);
	      }
	    }
	  }
	  
	  public void insert(SyncInfo info) {
      int configSize = bp.getConfigSize(info.loc);
      minConfigSize = (minConfigSize < configSize ? minConfigSize : configSize);
      maxConfigSize = (maxConfigSize > configSize ? maxConfigSize : configSize);
      if (!queue.containsKey(configSize)) {
        queue.put(configSize, new LinkedList<SyncInfo>() );
      }
      info.reduce();
      queue.get(configSize).addLast(info);
      queueSize++;
	  }
	  
	  public SyncInfo removeFirst() {
	    LinkedList<SyncInfo> minQueue = queue.get(minConfigSize);
	    SyncInfo min = minQueue.removeFirst();
	    if (minQueue.isEmpty()) {
	      queue.remove(minConfigSize);
	      minConfigSize = Integer.MAX_VALUE;
	      for (Integer configSize : queue.keySet()) {
	        if (configSize < minConfigSize) minConfigSize = configSize;
	      }
	    }
	    queueSize--;
	    return min;
	  }
	  
	  public int size() {
	    return queueSize;
	  }
	  
	  @Override
	  public String toString() {
	    StringBuilder b = new StringBuilder();
	    for (int i=0;i<=maxConfigSize;i++) {
	      if (queue.containsKey(i)) {
	        b.append(i+": "+queue.get(i)+"\n");
	      }
	    }
	    return b.toString();
	  }
	}
	
	/**
   * Priority queue containing all enabled events that have not been added to
   * the branching process yet.
   */
  protected EnablingQueue enablingQueue = new EnablingQueue();

	
	/**
	 * Find all enabled events and their locations of enabling in the
	 * current branching process and them to the {@link #enablingQueue}.
	 */
	protected EnablingInfo getAllEnabledEvents(DNodeSetElement fireableEvents) {
		EnablingInfo info = new EnablingInfo();
		
    //System.out.println(co);

		for (DNode e : fireableEvents)
		{
		
			// System.out.println("ENABLED "+e+" "+DNode.toString(e.pre)+":");
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
  			
  			//_debug_log.append(e+" has valid cut at "+DNode.toString(cutNodes)+"\n");
  
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
  			  
  			  //System.out.println(e+" is enabled at "+DNode.toString(cutNodes));
  	      //_debug_log.append(e+" is enabled at "+DNode.toString(cutNodes)+"\n");
  				
  				assert loc[e.pre.length-1] != null : "Error, adding invalid enabling location";

  				// search all known enabled events whether there is already an event with
  				// the same ID at the same location. If so: synchronize both events.
				  info.putEnabledEvent(e, loc);

  			} else {
  			  //System.out.println("  incomplete match");
  			}
		  } // for all cuts
		}
		
		for (Entry<EventPreSet, LinkedList<DNode[]>> cuts : cachedCuts.entrySet()) cuts.getValue().clear();
		cachedCuts.clear();
		
		// set all existing conditions to old
		for (DNode b : bp.allConditions) {
		  b._isNew = false;
		}
		
    return info;
	}

	/**
	 * Fire all enabled events from the queue.
	 * @return number of events added to the branching process
	 */
	private int fireAllEnabledEvents() {
	  
	  int fired = 0;
	  while (enablingQueue.size() > 0) {
	    DNode[] postCond = fireEnabledEvent(enablingQueue.removeFirst());
	    if (postCond != null) fired++;
	  }
	  return fired;
	}

	/**
	 * Fire the least enabled event from the queue.
	 * @return number of events added to the branching process
	 */
	private int fireLeastEnabledEvent() {
	  int fired = 0;
    if (enablingQueue.size() > 0)
      do {
        DNode[] postCond = fireEnabledEvent(enablingQueue.removeFirst());
        if (postCond != null) fired++;
      } while (enablingQueue.size() > 0 && fired == 0);
    return fired;
	}
	
	/**
	 * Fire an enabled event.
	 * 
	 * @param fireEvents
	 * @return the number of created events in the branching process
	 */
	protected DNode[] fireEnabledEvent(SyncInfo fireEvents) {
	  
		//for (int fireIndex=0; fireIndex<info.enabledEvents.size(); fireIndex++) {
		   
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
			
		  long _debug_t2a = System.currentTimeMillis();
		  boolean _debug_t2a_multi = false;
			
			if (fireEvents.events.length == 1) {

				//System.out.println("fire "+info.enabledEvents.get(fireIndex)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex)));
			  //_debug_log.append("fire "+info.enabledEvents.get(fireIndex)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex))+"\n");
				DNode e = fireEvents.events[0];

				if (e.isAnti) {
					// remember that this event was an anti-event
					setAnti = true;
					//System.out.println("not firing "+e+" (is anti)");
				}
				if (e.isHot) {
				  setHot = true;
				}
				postConditions = bp.fire(e, fireEvents.loc, false);

			} else {
				DNode events[] = fireEvents.events;
				
				//System.out.println("fire "+DNode.toString(events)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex)));
				//_debug_log.append("fire "+DNode.toString(events)+ " at "+DNode.toString(info.enablingLocation.get(fireIndex))+"\n");
				for (int j=0;j<events.length;j++) {
					if (events[j].isAnti) {
	          // remember that the fired event does not occur in the final result
					  setAnti = true;
            //System.out.println("not firing anti-event "+events[j]);
						// break;
					}
					if (events[j].isHot) {
					  setHot = true;
					}
				}
				postConditions = bp.fire(events, fireEvents.loc, false);
				
				_debug_t2a_multi = true;
			}
			
			// we did not fire this event
			if (postConditions == null)
				return null;
			
      long _debug_t2b_t3a_fireEvent = System.currentTimeMillis();

			// update co-relation for all new post-conditions
			updateConcurrencyRelation(postConditions);
			
			long _debug_t3b_t4a_co = System.currentTimeMillis();
			long _debug_t4b_t5a_primeConfig = 0;
			long _debug_t5b_cutOff = 0;
			boolean _debug_t5_computed_cutOff = false;
			
			if (postConditions.length > 0) {
			  
  			DNode newEvent = postConditions[0].pre[0];
  			
        setCurrentPrimeConfig(newEvent, true);
        newEvent.isHot = setHot;   // remember temperature of event
  			
  			_debug_log.append("created new event "+newEvent+" at "+DNode.toString(fireEvents.loc)+"\n");
  			_debug_log.append("having post-conditions "+DNode.toString(postConditions)+"\n");
  			_debug_log.append("from "+toString(newEvent.causedBy)+"\n");
  			
        _debug_t4b_t5a_primeConfig = System.currentTimeMillis();
        
        // prevent that an anti-event gets successors in
        // the BP, mark each anti-event as a cutoff event
        if (setAnti) {
          newEvent.isAnti = true;
          if (newEvent.isHot) {
            // hot anti-events MUST NOT occur, hence, no successor of hot-anti events occurs
            newEvent.isCutOff = true;
          }
          // System.out.println("created anti-event "+newEvent);
          _debug_log.append("created anti-event "+toString(primeConfigurationString.get(newEvent))+"\n");
        } else {
          _debug_t5_computed_cutOff = true;
          
          // for all other events, check whether it is a cutOff event
    			if (isCutOffEvent(newEvent)) {
    				newEvent.isCutOff = true;
    				_debug_log.append("is a cut-off event because of "+futureEquivalence.getElementary_ccPair().get(newEvent)+"\n");
    				_debug_log.append(toString(primeConfigurationString.get(newEvent))+"\n");//" > "+toString(primeConfigurationString.get(elementary_ccPair.get(newEvent)))+"\n");
    			} else {
    			  _debug_log.append(toString(primeConfigurationString.get(newEvent))+"\n");
    			}
        }
        
        _debug_t5b_cutOff = System.currentTimeMillis();

  			// remember cutOff on post-conditions as well
  			if (newEvent.isCutOff == true)
  			  setCutOff(newEvent);
  			
        if (getOptions().checkProperties)
          checkProperties(newEvent);
        
			} else {
			  currentPrimeCut = null;
			}
			
			if (getOptions().checkProperties)
				checkProperties();
			
			long _debug_cutOfftime = (_debug_t5_computed_cutOff ? (_debug_t5b_cutOff-_debug_t4b_t5a_primeConfig) : 0);
			String _debug_multi = (_debug_t2a_multi ? "m" : "s");
			
			_debug_executionTimeProfile.append(
			    (_debug_t5b_cutOff-_debug_t0)+";"
			   +(_debug_t2b_t3a_fireEvent-_debug_t2a)+";"+_debug_multi+";"
			   +(_debug_t3b_t4a_co-_debug_t2b_t3a_fireEvent)+";"+_debug_coCheckCount+";"
         +(_debug_t4b_t5a_primeConfig-_debug_t3b_t4a_co)+";"
         +(_debug_cutOfftime)+"\n"
			);
		return postConditions;
	}
	
	int _debug_coCheckCount = 0;
	
	/**
	 * Set concurrency relation for post conditions of a new event
	 * of the branching process.
	 * 
	 * @param postConditions
	 */
	public void updateConcurrencyRelation(DNode[] postConditions) {
	  
	  if (postConditions.length == 0) return;
	  
	  _debug_coCheckCount = 0;
	  
    for (DNode d : postConditions) {
      co.put(d, new HashSet<DNode>());
      coID.put(d, new HashSet<Short>());
    }
    
    // all post-conditions are mutually concurrent, set the corresponding maps
    for (int i=0;i<postConditions.length;i++) {
      Set<DNode> co_i = co.get(postConditions[i]);
      Set<Short> coID_i = coID.get(postConditions[i]);
      for (int j=i+1;j<postConditions.length;j++) {
        co_i.add(postConditions[j]);
        co.get(postConditions[j]).add(postConditions[i]);
        coID_i.add(postConditions[j].id);
        coID.get(postConditions[j]).add(postConditions[i].id);
      }
    }
    
    //for (DNode d : postConditions) {
    //  updateConcurrencyRelation(d, new DNode[]{d});
    //}
    updateConcurrencyRelation(postConditions[0], postConditions);
	}

  /**
   * Determines whether 'd' and 'd2' are concurrent using a structural search on
   * the branching process instead of the explicit concurrency relation
   * {@link #co}.
   * 
   * @param d
   * @param d2
   * @return <code>true</code> iff the nodes 'd' and 'd2' are concurrent
   */
	public boolean areConcurrent_struct(DNode d, DNode d2) {
	  
	  HashSet<DNode> dPre = bp.getAllPredecessors(d);
	  if (dPre.contains(d2)) return false;
	  HashSet<DNode> dPre2 = bp.getAllPredecessors(d2);
	  if (dPre2.contains(d)) return false;
	  
	  for (DNode b : dPre) {
      if (b.isEvent) continue;
	    if (!dPre2.contains(b)) continue;
	    if (b.post == null) continue;
	    
	    boolean e_in_dPre_notIn_dPre2 = false;
	    boolean e_in_dPre2_notIn_dPre = false;
	    // condition 'b' is in the intersection of dPre and dPre2
	    for (DNode e : b.post) {
	      // one post-event 'e' of 'b' that is predecessor of 'd' but not of 'd2'
	      if (dPre.contains(e) && !dPre2.contains(e)) e_in_dPre_notIn_dPre2 = true;
        // one post-event 'e' of 'b' that is predecessor of 'd2' but not of 'd'
	      if (dPre2.contains(e) && !dPre.contains(e)) e_in_dPre2_notIn_dPre = true;
	    }
	    if (e_in_dPre_notIn_dPre2 && e_in_dPre2_notIn_dPre) // 'd' and 'd2' conflict on b
	      return false;
	  }
	  
	  return true;
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
  protected DNode[] currentPrimeCut;
  
  /**
   * The current prime configuration after firing one event. It is stored
   * longer than the lifetime of the method {@link DNodeBP#isCutOffEvent(DNode)}
   * which sets its value. This way, <code>currentPrimeCut</code> can be read in
   * subsequent analysis steps. 
   */
  protected HashSet<DNode> currentPrimeConfig;
  
  /**
   * Set fields {@link #currentPrimeCut} {@link #currentPrimeConfig} and update
   * fields {@link #primeConfiguration_Size}, {@link #primeConfiguration_CutHash}, and
   * {@link #primeConfigurationString} for 'event' that has just been fired.
   * 
   * @param event
   * @param storeValues  set to <code>true</code> when calling this
   * method for the first time for 'event', otherwise set to <code>false</code>
   */
  public void setCurrentPrimeConfig(DNode event, boolean storeValues) {
    
    boolean computePredecessors = false;
    boolean includeEvent = false;

    // collected all predecessor events if using the lexicographic search strategy
    if (getOptions().searchStrat_predecessor || getOptions().searchStrat_lexicographic) computePredecessors = true;
    if (getOptions().searchStrat_size) includeEvent = true;
    
    currentPrimeCut = bp.getPrimeCut(event, computePredecessors, includeEvent);
    currentPrimeConfig = bp.getPrimeConfiguration;
    
    if (storeValues) {
      int currentConfigSize = bp.getPrimeConfiguration_size;

      primeConfiguration_Size.put(event, currentConfigSize);
      primeConfiguration_CutHash.put(event, hashCut(currentPrimeCut));
      
      // LEXIK: optimization for size-lexicographic search strategy
      primeConfigurationString.put(event, getLexicographicString(currentPrimeConfig));
      //primeConfigurationString.put(event, getLexicographicString_acc(currentPrimeConfig));
    }
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
  public boolean isCutOffEvent(DNode event) {
    
    // pick a search strategy for determing the cut off event
    if (getOptions().searchStrat_predecessor) {
      
      // pick an equivalence criterion for determining equivalent cuts
      if (getOptions().cutOffEquiv_history) {
        if (findEquivalentCut_history(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (getOptions().cutOffEquiv_marking) {
        if (findEquivalentCut_marking_predecessor(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (getOptions().cutOffEquiv_conditionHistory ) {
        int currentConfigSize = bp.getPrimeConfiguration_size;
        if (findEquivalentCut_conditionHistory_signature_size(
            currentConfigSize, event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
        
      } else if (getOptions().cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_predecessor(event, currentPrimeCut, currentPrimeConfig)) {
          return true;
        }
      } else {
        assert false : "No valid search strategy given.";
      }
      
    } else if (getOptions().searchStrat_size) {
      // search for cut off events in all previously constructed events

      // pick an equivalence criterion for determining equivalent cuts
      if (getOptions().cutOffEquiv_conditionHistory ) {
        if (findEquivalentCut_conditionHistory_signature_size(
            primeConfiguration_Size.get(event), event, currentPrimeCut, bp.getAllEvents())) {
          return true;
        }
        
      } else if (getOptions().cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_size(
            primeConfiguration_Size.get(event), event, currentPrimeCut, bp.getAllEvents())) {
          return true;
        }
        
      } else {
        assert false : "No valid search strategy given.";
      }
    }
    
    return false;
  }
  
  /**
   * Mark event e as cut-off event. Also sets {@link DNode#isCutOff} for
   * the post-conditions of e
   * 
   * @param e
   */
  private void setCutOff(DNode e) {
    e.isCutOff = true;
    for (DNode b : e.post) {
      //System.out.println("setting cut-off condition "+b+" because of "+newEvent);
      b.isCutOff = true;
      b.isAnti = e.isAnti && e.isHot; // copy the anti-flag
    }
  }

  /**
   * Guard to allow events that have already been added to the branching
   * process to be checked again for being cut-off events. The field is
   * controlled by {@link #balanceCutOffEvents()}.
   * 
   * ----------------------------------------------------------------
   *  Balancing Cut-Off Events
   *     
   *  The correct computation of a minimal unfolding requires to pick
   *  a least event from all enabled events wrt. the adequate order.
   *  Here, the adequate order uses lexicographic comparison of
   *  prime configurations of equal size. However, the
   *  {@link DNodeBP#enablingQueue} sorts enabled events only by the
   *  size of their prime configurations which may lead to wrong
   *  ordering of enabled events with the same size. In consequence,
   *  some cut-off events would not be detected: the smaller
   *  event X2 is added after the larger event X1, so neither is
   *  declared cut-off. If X2 was added first, then X1 could be
   *  detected as cut-off.
   *  
   *  The following procedures efficiently implement detecting these
   *  wrong orderings once they occurred and allows to re-check whether
   *  an event is a cut-off event.
   *  
   *  We detect whether events were added in the wrong order in
   *  {@link #findEquivalentCut_conditionHistory_signature_size(int, DNode, DNode[], Iterable)}
   *  when comparing two events with prime configurations of the
   *  same size. If the newly added event X2 has a smaller configuration
   *  than the compared event X1, and X1 has just been added in the
   *  same iteration (if {@link DNode#_isNew} is <code>true</code>),
   *  then X1 and X2 should have been added in a different order. Thus,
   *  we may check again for X1 whether it is a cut-off event.
   *  
   *  We do so by adding X1 to {@link #balanceCutOffEvents_list}. Each
   *  event in this list is check at the end of the iteration for
   *  being a cut-off event in {@link #balanceCutOffEvents()}.
   *  
   * ----------------------------------------------------------------
   */
  private boolean balanceCutOffEvents_allow = true;
  
  /**
   * A guarded linked list collecting all events that shall be checked again
   * for being cut-off events. Events must be added via {@link #checkForCutOff_again(DNode)} only.
   */
  private LinkedList<DNode> balanceCutOffEvents_list = new LinkedList<DNode>();

  /**
   * Add event 'e' to the list of events that shall be checked again for being
   * cut-off events. See {@link #balanceCutOffEvents_allow}.
   * 
   * Events are only checked if {@link #balanceCutOffEvents_allow} is set to <code>true</code>.
   * The value of {@link #balanceCutOffEvents_allow} is controlled by the
   * algorithm in {@link #balanceCutOffEvents()}.
   *  
   * @param e
   */
  protected void checkForCutOff_again(DNode e) {
    if (balanceCutOffEvents_allow == false) return;
    balanceCutOffEvents_list.addLast(e);
  }
  
  /**
   * Checks each event in {@link #balanceCutOffEvents()} again for being
   * a cut-off event.
   */
  private void balanceCutOffEvents() {
    // Do not add further events to balanceCutOffEvents_list because
    // we already checked all events. The events we are checking now
    // are the only candidates that still might become cut-offs.
    balanceCutOffEvents_allow = false;
    for (DNode e : balanceCutOffEvents_list) {
      setCurrentPrimeConfig(e, false);
      if (isCutOffEvent(e)) {
        //System.out.println("post-hoc setting cut-off for "+e);
        setCutOff(e);
      }
    }
    balanceCutOffEvents_list.clear();
    // Allow checking cut-off events again (for the ntext iteration)
    balanceCutOffEvents_allow = true;
  }

  /**
   * Main algorithm. One step for extending the current branching process.
   *  
   * @return
   *    number of events that have been fired in this step, returns
   *    0 if construction of the branching process is complete.
   */
  public int step() {
    // System.out.println("----- next step -----");
    EnablingInfo info = getAllEnabledEvents(dNodeAS.fireableEvents);
    enablingQueue.insertAll(info);
    
    /*
     * for checking quasi liveness
     * 
    Uma.out.println("found "+enablingQueue.size()+" enabled events");
    
    for (LinkedList<SyncInfo> enabled : enablingQueue.queue.values()) {
      for (SyncInfo s : enabled) {
        if (s.events[0].id == getOptions().checkProperty_ExecutableEventID) {
          propertyCheck |= PROP_EXECUTEDEVENT;
          Uma.out.println("found event early");
          return 0;
        }
      }
    }
    */
    
    int fired = fireAllEnabledEvents();
    
    // some cut-off events may not have been detected because
    // we are lazy when constructing the enablingQueue
    balanceCutOffEvents();
    
    if (abort) return 0;
    
    if (propertyCheck == PROP_NONE || !getOptions().stopIfPropertyFound)
      return fired;
    else
      return 0; // abort if properties violated/abortion condition holds
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
	
	private final FutureEquivalence futureEquivalence = new FutureEquivalence();
	
	/**
	 * maps each cut-off event to its equivalent event 
	 */
	private final Map<DNode, DNode> cutOffEquivalentEvent = new HashMap<DNode, DNode>();
	
	/**
	 * @return the mapping that assigns each cut-off event its equivalent event
	 */
	public Map<DNode, DNode> getCutOffEquivalentEvent () {
	  return cutOffEquivalentEvent;
	}
	
	/**
	 * Update the mapping that relates each new cutOff event to its equivalent
	 * old event. If the equivalent event is a cutOff event as well, follow the
	 * equivalence relation transitively to the first event.
	 * 
	 * @param cutOffEvent
	 *     the new event that is a cutOff event
	 * @param equivalentEvent
	 *     the old event to which <code>newEvent</code> is equivalent
	 *     
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
	 */
	protected void setCutOffEvent(DNode cutOffEvent, DNode equivalentEvent) {
	  //System.out.println("setting cut-off event: "+cutOffEvent+" --> "+equivalentEvent);
	  cutOffEquivalentEvent.put(cutOffEvent, equivalentEvent);
	  futureEquivalence.putElementaryEquivalent(cutOffEvent, equivalentEvent);
	}
	
	/**
   * Update the mapping that relates each new condition of a cutOff event to
   * its equivalent counterpart condition.
   * 
   * @param cutOffEvent
   *     the new cut-off event
   * @param cutOffConditions
   *     the new conditions
   * @param equivalentConditions
   *     the old conditions to which <code>cutOffConditions</code> are equivalent
   *     
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   */
  protected void setCutOffConditions(DNode cutOffEvent, DNode[] cutOffConditions, DNode equivalentConditions[]) {
    for (int i=0; i<cutOffConditions.length; i++) {
      
      futureEquivalence.putElementaryEquivalent(cutOffConditions[i], equivalentConditions[i]);
      
      if (cutOffConditions[i] == equivalentConditions[i]) {
        //System.out.println("[DNodeBP:updateCCpair] WARNING: making "+newConditions[i]+" equivalent to itself");
      } else {
        
        // update mapping only for post-conditions of cut-off event
        if (cutOffConditions[i].pre != null && cutOffConditions[i].pre.length > 0
            && cutOffConditions[i].pre[0] != cutOffEvent) {
          continue;
        }
        
        //System.out.println("setting cut-off condition: "+newConditions[i]+" because of "+equivalentConditions[i]);
        cutOffConditions[i].isCutOff = true; // remember it
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
		  setCutOffConditions(newEvent, newCut, bp.initialCut);
			return true;
		}

		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (equivalentCuts_history(newCut, oldCut)) {
			  setCutOffEvent(newEvent, e); // update the cutOff mapping
			  setCutOffConditions(newEvent, newCut, oldCut);
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
	 * search ({@link Options#searchStrat_predecessor}. This way, we can assume
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
   * A predecessor based search strategy ({@link Options#searchStrat_predecessor}).
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
			  setCutOffConditions(newEvent, newCut, bp.initialCut); // make new and old cut equivalent
				return true;
			}
		
		// check the prime cuts of all events against which we compare
		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (newCut.length == oldCut.length)
				if (equivalentCuts_eventSignature_predecessor(newCutSignature, oldCut)) {
	        setCutOffEvent(newEvent, e);     // make new and old event equivalent
	        setCutOffConditions(newEvent, newCut, oldCut);  // make new and old cut equivalent
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
    HashSet<DNode> cut = new HashSet<DNode>();
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
   * search ({@link Options#searchStrat_size}. This strategy must
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
   * A size-based search strategy ({@link Options#searchStrat_size}).
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
			  setCutOffConditions(newEvent, newCut, bp.initialCut); // make new and old cut equivalent
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
	        setCutOffEvent(newEvent, e);    // make new and old event equivalent
	        setCutOffConditions(newEvent, newCut, oldCut); // make new and old cut equivalent
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
    
    byte signature[] = new byte[dNodeAS.fireableEvents.size()+dNodeAS.preConEvents.size()];

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
   * A predecessor based search strategy ({@link Options#searchStrat_predecessor}).
   * 
   * @param newEvent
   * @param newCut
   * @param eventsToCompare
   * @return
   */
	private boolean findEquivalentCut_marking_predecessor(DNode newEvent, DNode[] newCut, HashSet<DNode> eventsToCompare) {

		if (equivalentCuts_marking(newCut, bp.initialCut)) {
		  setCutOffConditions(newEvent, newCut, bp.initialCut);
			return true;
		}
		
		for (DNode e : eventsToCompare) {
			DNode[] oldCut = bp.getPrimeCut(e, false, false);
			if (equivalentCuts_marking(newCut, oldCut)) {
	      setCutOffEvent(newEvent, e);    // make new and old event equivalent
	      setCutOffConditions(newEvent, newCut, oldCut); // make new and old cut equivalent
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
	 * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
	 * 
	 * @param newCutSignature
	 * @param newCut
	 * @param oldCut 
	 */
	protected boolean equivalentCuts_conditionSignature_history(byte[] newCutSignature, DNode newCut[], DNode oldCut[]) {

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
   * The search strategy for {@link #equivalentCuts_conditionSignature_history(byte[], DNode[], DNode[])}.
   * A size-based search strategy ({@link Options#searchStrat_size}). 
   * 
   * The method has be extended to determine cut-off events by a lexicographic
   * search strategy. 
   * 
   * @param newEvent  event that has been added to the branching process
   * @param newCut    the cut reached by 'newEvent'
   * @param eventsToCompare
   * @return <code>true</code> iff <code>newEvent</code> is a cut-off event
   * because of some equivalent event in <code>eventsToCompare</code>.
   */
  private boolean findEquivalentCut_conditionHistory_signature_size (
      int newCutConfigSize,
      DNode newEvent,
      DNode[] newCut,
      Iterable<DNode> eventsToCompare)
  {
    // all extensions to support the size-lexicographic search-strategy
    // or marked with LEXIK
    
    // optimization: determine equivalence of reached cuts by the
    // help of a 'condition signature, initialize 'empty' signature
    // for 'newEvent', see #equivalentCuts_conditionSignature_history
    byte[] newCutSignature = cutSignature_conditions_init255();
    
    // compare the cut reached by 'newEvent' to the initial cut
    if (newCut.length == bp.initialCut.length)
      if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, bp.initialCut)) {
        // yes, newEvent reaches the initial cut again
        setCutOffConditions(newEvent, newCut, bp.initialCut);
        return true; // 'newEvent' is a cut-off event 
      }
    
    // Check whether 'eventsToCompare' contains a "smaller" event that reaches
    // the same cut as 'newEvent'. 
    
    // Optimization: to quickly avoid comparing configurations of events that
    // do not reach the same cut as 'newEvent', compare and store hash values
    // of the reached configurations. 
    
    // get hash value of the cut reached by 'newEvent'
    int newEventHash = primeConfiguration_CutHash.get(newEvent);
    
    // check whether 'newEvent' is a cut-off event by comparing to all other
    // given events
    Iterator<DNode> it = eventsToCompare.iterator();
    while (it.hasNext()) {
      DNode e = it.next();
      
      // do not check the event that has just been added, the cuts would be equal...
      if (e == newEvent) continue;
        
      // newCut is only equivalent to oldCut if the configuration of newCut
      // is (lexicographically) larger than the configuration of oldCut
      if (!primeConfiguration_Size.containsKey(e)) {
        // the old event 'e' has incomplete information about its prime
        // configuration, cannot be used to check for cutoff
        continue;
      }
      // optimization: compared hashed values of the sizes of the prime configurations 
      if (newCutConfigSize < primeConfiguration_Size.get(e)) {
        // the old one is larger, not equivalent
        continue;
      }
      
      // optimization: compare reached states by their hash values
      // only if hash values are equal, 'newEvent' and 'e' could be equivalent 
      if (primeConfiguration_CutHash.get(e) != newEventHash)
        continue;

      // retrieve the cut reached by the old event 'e'
      DNode[] oldCut = bp.getPrimeCut(e, getOptions().searchStrat_lexicographic, getOptions().searchStrat_lexicographic);
      
      // cuts of different lenghts cannot reach the same state, skip
      if (newCut.length != oldCut.length)
        continue;

      // if both configurations have the same size:
      if (newCutConfigSize == bp.getPrimeConfiguration_size) {
        
        // and if not lexicographic, then the new event cannot be cut-off event
        if (!getOptions().searchStrat_lexicographic) continue;
        // LEXIK: otherwise compare whether the old event's configuration is
        // lexicographically smaller than the new event's configuration
        if (!isSmaller_lexicographic(primeConfigurationString.get(e), primeConfigurationString.get(newEvent))) {

          // Check whether 'e' was just added. If this is the case, then 'e' and 'newEvent'
          // were added in the wrong order and we check again whether 'e' is a cut-off event
          if (e.post != null && e.post.length > 0 && e.post[0]._isNew) {
            //System.out.println("smaller event added later, should switch cut-off");
            checkForCutOff_again(e);
          }
          continue;
        }
      }
      
      // The prime configuration of 'e' is either smaller or lexicographically
      // smaller than the prime configuration of 'newEvent'. Further, both events
      // reach cuts of the same size. Check whether both cuts reach the same histories
      // by comparing their condition signatures
      if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, oldCut)) {
        // yes, equivalent cuts, make events and conditions equivalent
        setCutOffEvent(newEvent, e);
        setCutOffConditions(newEvent, newCut, oldCut);
        // and yes, 'newEvent' is a cut-off event
        return true;
      }
    }
    // no smaller equivalent has been found
    return false;
  }
  
  /**
   * @return
   *   a fresh condition signature stating that the number of occurrences
   *   of conditions in a cut is unknown (value 255)
   *   
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   */
  protected byte[] cutSignature_conditions_init255() {
    byte signature[] = new byte[dNodeAS.preConditions.length];

    for (int i=0; i<signature.length; i++)
      signature[i] = (byte) 255;
    return signature;
  }
  
  /**
   * Optimizes finding a cut-off event.
   * 
   * Stores the size of the prime configuration for each event of the branching process.
   * used in {@link #findEquivalentCut_conditionHistory_signature_size(int, DNode, DNode[], Iterable)}
   */
  private HashMap<DNode, Integer> primeConfiguration_Size = new HashMap<DNode, Integer>();
  
  /**
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   * 
   * @return size of the prime configuration for each event of the branching process,
   *         stored in {@link #primeConfiguration_Size}
   */
  protected HashMap<DNode, Integer> getPrimeConfiguration_Size () {
    return primeConfiguration_Size;
  }

  /**
   * Optimizes finding a cut-off event.
   * 
   * Stores a hash-value of the cut reached by the prime configuration of an event. 
   * Two events with different hash-values cannot reach the same state. Used in
   * {@link #findEquivalentCut_conditionHistory_signature_size(int, DNode, DNode[], Iterable)}
   * 
   * Hash-values of the cut is computed by {@link #hashCut(DNode[])}.
   */
  private HashMap<DNode, Integer> primeConfiguration_CutHash = new HashMap<DNode, Integer>();
  
  /**
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   * 
   * @return hashvalues of the reached cut stored in {@link #primeConfiguration_CutHash}
   */
  protected HashMap<DNode, Integer> getPrimeConfiguration_CutHash () {
    return primeConfiguration_CutHash;
  }
  
   /**
   * @param cut
   * @return hash value of the given cut, can be stored in and compared to values
   * in {@link #primeConfiguration_CutHash}
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
	
  
  /* --------------------------------------------------------------------------
   * 
   *  Size-Lexicographic Search Strategy, optimizes the size-based search
   *  strategy for the case when two prime configurations have the same size. 
   *  
   *  Let s1 = a1 a2 a3 ... an and
   *      s2 = b1 b2 b3 ... bn
   *      
   *  be two strings of equal size and let < be a total order over the
   *  characters of the alphabet of s1 and s2. s1 is lexicographically smaller
   *  than s2 if there exists an index i s.t. aj = bj for all j < i and
   *  ai < bi. 
   * --------------------------------------------------------------------------
   */
  
  /*
   * 
   * There are different ways to represent the prime configuration of an event
   * as a lexicographically ordered string. Basically, the alphabet are the IDs
   * of the events (DNode#id). Their order is the order < on the shorts by which
   * IDs are encoded.
   * 
   * Programmatically, the string that represents an event's prime configuration
   * can be encoded in several ways. 
   * 
   * (1) The prime configuration of an event 'e' can be turned into a string by
   *     storing all events of the prime configuration of 'e' including 'e' in an
   *     array of DNodes sorted by their IDs.
   *     
   *     Whether one such configuration is smaller than another configuration 
   *     follows from the order of the ids of the DNodes in that array. This
   *     approach is not implemented.
   *     
   * (2) Alternatively, the IDs can be stored directly in an array of sorted
   *     shorts, i.e. directly representing the string of IDs.
   *     This approach is implemented in this class and used.
   *     
   * (3) Alternatively, an array stores for each ID (of all IDs used in the system)
   *     the number of events having this ID.
   *     This approach is implemented in this class but not used.
   *     
   * While approach (3) is more memory intensive in case of small configurations but
   * many IDs, it gets more efficient compared to (2) in systems where configurations
   * contain many IDs.
   */

  /**
   * @param configuration
   * @return lexicographic string of IDs (sorted in ascending order) of the
   * events in the given <code>configuration</code>
   */
  private short[] getLexicographicString (HashSet<DNode> configuration) {
    // implements approach (2) to represent lexicographic strings
    short[] currentPrimeConfigIDs = new short[configuration.size()];
    int i = 0;
    for (DNode e : configuration) {
      currentPrimeConfigIDs[i++] = e.id;
    }
    Arrays.sort(currentPrimeConfigIDs);
    return currentPrimeConfigIDs;
  }
  
  
  /**
   * @param configuration
   * @return an id-sorted array telling for each event-ID how often the ID
   * occurs in the given configuration
   */
  @SuppressWarnings("unused")
  private byte[] getLexicographicString_acc(HashSet<DNode> configuration) {
    // implements approach (3) to represent lexicographic strings
    byte[] lstring = new byte[dNodeAS.fireableEventIndex.size()];
    for (DNode e : configuration) {
      lstring[dNodeAS.fireableEventIndex.get(e.id)]++;
    }
    return lstring;
  }

  /**
   * Optimizes finding a cut-off event.
   * 
   * Stores a lexicographic string representation of the prime configuration reached
   * by an event. The string is computed using {@link #getLexiographicString(HashSet)}.
   * This field is used in
   * {@link #findEquivalentCut_conditionHistory_signature_size(int, DNode, DNode[], Iterable)}
   */
  private HashMap<DNode, short[]> primeConfigurationString = new HashMap<DNode, short[]>();
  
  /**
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   * 
   * @return lexicographic string representation of the prime configuration reached
   *         by an event, stored in {@link #primeConfigurationString}
   */
  protected HashMap<DNode, short[]> getPrimeConfigurationString () {
    return primeConfigurationString;
  }
	
  
  /**
   * Compare two prime configurations of equal size whether one is lexicographically
   * smaller than the other. Use this method to compare strings computed by
   * {@link #getLexicographicString(HashSet)}.
   * 
   * @param oldConfigIDs
   * @param newConfigIDs
   * @return <code>true</code> iff oldConfig is smaller than newConfig
   */
  protected boolean isSmaller_lexicographic(short[] oldConfigIDs, short[] newConfigIDs) {
    //System.out.println(toString(oldConfigIDs)+" <? "+toString(newConfigIDs));
    
    if (getOptions().searchStrat_lexicographic == false) return false;
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
   * smaller than the other. Use this method to compare strings computed by
   * {@link #getLexicographicString_acc(HashSet)}.
   * 
   * @param oldConfig
   * @param newConfig
   * @return <code>true</code> iff oldConfig is smaller than newConfig
   */
  @SuppressWarnings("unused")
  protected boolean isSmaller_lexicographic_acc(byte[] oldConfig, byte[] newConfig) {
    if (getOptions().searchStrat_lexicographic == false) return false;

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

  


	
  /* ==========================================================================
   * 
   *  Operations on the completely constructed McMillan prefix.
   * 
   * ==========================================================================
   */
   	
  private boolean futureEquivalenceInitialized = false;

  /**
   * @return the mapping of cutOff events to their equivalent counter parts
   */
  public Map<DNode, DNode> equivalentNode() {
    if (!futureEquivalenceInitialized) {
      // first compute the folding equivalence, which also sets elementary_ccPair
      initializeFutureEquivalence();
      futureEquivalenceInitialized = true;
    }
    return futureEquivalence.getElementary_ccPair();
  }

  /**
   * Initialize the {@link #futureEquivalence} which defines which nodes of the
   * branching process have equivalent futures. The default implementation takes
   * the basic equivalence mappings from events to their cut-off events and
   * extends it to equivalence classes with canonical representatives.
   * 
   * Extensions of this class may refine this method to provide additional
   * intialization. Any refinement must first call this method with
   * <code>super.initializeFutureEquivalence()</code> before extending the
   * future equivalence.
   */
  protected void initializeFutureEquivalence() {
    futureEquivalence.completeElementaryEquivalence(bp.getAllNodes());
    futureEquivalence.initFutureEquivalenceClasses();
    // remember here so that refinements of this method do not invoke
    // this method again when calling futureEquivalence()
    futureEquivalenceInitialized = true;  
  }

  /**
   * @return the future equivalence of this branching process, if called for the
   *         first time, the future equivalence is initialized from the
   *         identified cut-off events (see {@link #initialize()}), the future
   *         equivalence may be modified by extensionds of this class
   */
  public final FutureEquivalence futureEquivalence() {
    if (!futureEquivalenceInitialized) {
      // also set futureEquivalenceInitialized to true
      initializeFutureEquivalence();
    }
    return futureEquivalence;
  }
  
	/* --------------------------------------------------------------------------
	 * 
	 *  Public interface of the branching process, various getters, setters,
	 *  statistics, and output functions.
	 *  
	 * -------------------------------------------------------------------------- */
  
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


  // remember all statistics values in fields of the class
  // for later retrieval
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
   * @param csvFormat print statistics in CSV file format for machine readable use in the
   *                  order: "number of conditions; number of events; number of arcs"
   * @return a string representation of the statistics
   */
  public String getStatistics(boolean csvFormat) {
    
    statistic_condNum = 0;
    statistic_eventNum = 0;
    statistic_cutOffNum = 0;
    statistic_arcNum = 0;

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
    if (csvFormat)
      return statistic_condNum +";"+statistic_eventNum+";"+statistic_arcNum;
    else
      return "|B|= "+statistic_condNum +" |E|= "+statistic_eventNum+" |E_cutOff|= "+statistic_cutOffNum+" |F|= "+statistic_arcNum;
  }
  
  /**
   * @return GraphViz Dot representation of the computed branching process,
   * calls {@link DNodeSet#toDot()}
   */
	public String toDot() {
		return bp.toDot(dNodeAS.properNames);
	}
	
  /**
   * @param coloring information for the different nodes
   * @return GraphViz Dot representation of the computed branching process,
   * calls {@link DNodeSet#toDot()}
   */
  public String toDot(HashMap<DNode, String> coloring) {
    return bp.toDot(dNodeAS.properNames, coloring);
  }
	
/* --------------------------------------------------------------------------
 * 
 *  Analysis routines for checking various temporal properties of the system.
 *  
 * -------------------------------------------------------------------------- */
	
	public static final int PROP_NONE = 0;
	public static final int PROP_UNSAFE = 1;
	public static final int PROP_DEADCONDITION = 2;
	public static final int PROP_EXECUTEDEVENT = 4;

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
    
    if (getOptions().boundToCheck == 1) {
      // TODO: generalize to check k-boundedness
      
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
   * Entry for checking properties wrt. the most recently fired event, while
   * constructing the branching process. This method is to be called after
   * every firing of an event.
   * 
   * @param firedEvent
   * @return
   *     <code>false</code> iff the construction
   *     of the branching process shall be interrupted
   */
  private boolean checkProperties(DNode firedEvent) {

    if (!firedEvent.isAnti
        && getOptions().checkProperty_ExecutableEventID == firedEvent.id)
    {
      // event with the configured labeled was fired
      propertyCheck |= PROP_EXECUTEDEVENT;
      Uma.out.println("event found.");
      return false;
    }
    
    if (!firedEvent.isAnti && getOptions().checkProperty_ExecutableEvent != -1) {
      for (int i=0; i<firedEvent.causedBy.length; i++) {
        if (getOptions().checkProperty_ExecutableEvent == firedEvent.causedBy[i]) {
          // event with the configured ID was fired
          propertyCheck |= PROP_EXECUTEDEVENT;
          Uma.out.println("event found.");
          return false;
        }
      }
    }
    return true;
  }
  
  /**
   * Reset all internal fields regarding whether the property to check for holds.
   * This reset can be used for checking multiple properties on the same branching
   * process.
   */
  public void resetPropertyCheck() {
    propertyCheck = PROP_NONE;
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
    if (!getOptions().checkProperty_DeadCondition)
      return false;
		
		// do not check if the BP is unsafe, in this case, the construction
		// was interrupted and the BP is incomplete
		if ((propertyCheck & PROP_UNSAFE) > 0)
			return false;
		
		deadConditions = new LinkedList<DNode>();
		
		// find a non-cut-off condition in the maximal nodes of the BP
		int numDeadConditions = 0;
		for (DNode b : bp.getCurrentMaxNodes()) {
			
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
		
		if (getOptions().boundToCheck == 0)
			return true;
		
    // compare all concurrent conditions
    for (Entry<DNode, Set<DNode>> coPairs : co.entrySet()) {
      // and see if two of them have the same ID
      short id = coPairs.getKey().id;
      
      int coNum = 0;
      for (DNode d : coPairs.getValue()) {
        if (d.id == id) coNum++;
      }
      
      if (coNum > getOptions().boundToCheck) {
        // found k other concurrent conditions with the same id
        // bound violated
        propertyCheck |= PROP_UNSAFE;
        System.out.println("conditions "+coPairs.getKey()+" has "+coNum+" concurrent conditions with the same name");
        return false;
      }
    }
		
		return true;
	}
	
	/**
	 * @return true iff the process is safe
	 */
	public boolean isSafe() {
	  if (getOptions().boundToCheck == 0) return true;
	  return (propertyCheck & PROP_UNSAFE) == 0;
	}
	
	/**
	 * @return <code>true</code> iff the event specified by {@link #configure_checkExecutable(short)}
	 * is executable in the system
	 */
	public boolean canExecuteEvent() {
	  if (getOptions().checkProperty_ExecutableEventID != -1)
	    return (propertyCheck & PROP_EXECUTEDEVENT) == PROP_EXECUTEDEVENT;
	  if (getOptions().checkProperty_ExecutableEvent != -1)
	    return (propertyCheck & PROP_EXECUTEDEVENT) == PROP_EXECUTEDEVENT;
	  return true;
	}
	
	 /**
   * @return true iff the process has a deadlock, call
   * {@link #findDeadConditions(boolean)} first, to find the dead conditions
   */
  public boolean hasDeadCondition() {
    if (!getOptions().checkProperty_DeadCondition) return true;
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
  
  // use to log procedure calls and intermediate results in a separate file
  public StringBuilder _debug_log = new StringBuilder();

  // use to log execution times and statistical data to profile performance of
  // the algorithm and its optimizations
  public StringBuilder _debug_executionTimeProfile = new StringBuilder();
  
  // remember when we started with the branching process construction for
  // the exeuction time profile
  long _debug_t0 = System.currentTimeMillis();

  /**
   * @param array
   * @return string representation of the int array
   */
  public static String toString(int[] array) {
    String result = "[";
    for (int i=0;i<array.length;i++) {
      if (i > 0) result += ",";
      result+=array[i];
    }
    return result+"]";
  }

  /**
   * @param array
   * @return string representation of the short array
   */
  public static String toString(short[] array) {
    String result = "[";
    for (int i=0;i<array.length;i++) {
      if (i > 0) result += ",";
      result+=array[i];
    }
    return result+"]";
  }


  /**
   * @return options for constructing the branching process
   */
  public Options getOptions() {
    return options;
  }

}
