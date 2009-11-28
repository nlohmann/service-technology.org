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

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.editor.ptnetLoLA.PtNet;

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
  public static boolean options_searchStrat_predecessor = false;
	/**
	 *  check for cut-off events using the lexicographic order on events:
	 *  compare each candidate event only with events that have been added
	 *  previously and which have a strictly smaller prime configuration
	 */
	public static boolean options_searchStrat_lexicographic = false;
	
  //// --- equivalence notions for detecting whether two events are equivalent
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  enabling the same sets of events
   */
  public static boolean options_cutOffEquiv_eventSignature = false;
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  isomorphism to the maximal depth of the histories in the input system
   */
  public static boolean options_cutOffEquiv_history = false;
	/**
	 * check for equivalence of cuts by comparing the reached markings only
	 */
	public static boolean options_cutOffEquiv_marking = false;
	/**
	 *  check for equivalence of cuts by comparing the histories of conditions
	 *  with respect to the conditions of the given system
	 */
	public static boolean options_cutOffEquiv_conditionHistory = false;

	//// --- analysis flags
	/**
	 * whether properties shall be checked on the fly at all
	 */
	public static boolean options_checkProperties = false;
	/**
	 * check whether the system is unsafe
	 */
	public static boolean options_checkProperty_Unsafe = false;
	/**
	 * check whether the system has dead conditions (which have no post-event
	 */
	public static boolean options_checkProperty_DeadCondition = false;
	
	/**
	 * A {@link DNode}-compatible representation of the original system for
	 * which the McMillan unfolding shall be constructed.
	 */
	private DNodeSys			dNodeAS;
	
	/**
	 * The branching process that is extended by this algorithm. 
	 */
	private DNodeSet		bp;
	
	/**
	 * The explicit concurrency relation of this branching process assigns each
	 * condition of the branching process all its concurrent conditions.
	 */
	private HashMap<DNode, Set<DNode>> co = new HashMap<DNode, Set<DNode> >();
	

	/**
	 * Initialize branching process construction for an {@link AdaptiveSystem}.
	 * This construction uses lexicographic search order and uses history of
	 * conditions as equivalence notion.
	 * 
	 * @param as
	 */
	public DNodeBP(AdaptiveSystem as) {

		// instantiate DNode representation of the adaptive system
		dNodeAS = new DNodeSys_AdaptiveSystem(as);
		
		initialize();
		
    options_searchStrat_lexicographic = true;
		options_cutOffEquiv_conditionHistory = true;
		
    options_checkProperties = true;
    options_checkProperty_Unsafe = true;
    options_checkProperty_DeadCondition = true;
	}

	/**
	 * Initialize branching process construction for a Petri net ({@link PtNet}).
	 * This construction uses the marking equivalence for finding cut off events
	 * and the predecessor search strategy.
	 * 
	 * @param net
	 * @throws InvalidModelException
	 */
	public DNodeBP(PtNet net) throws InvalidModelException {
		// instantiate DNode representation of the petri net
		dNodeAS = new DNodeSys_PtNet(net);
		
		initialize();

		options_searchStrat_predecessor = true;
		options_cutOffEquiv_marking = true;
		
	  options_checkProperties = true;
	  options_checkProperty_Unsafe = true;
	  options_checkProperty_DeadCondition = true;
	}
	
	/**
	 * Initialize all members for the branching process construction.
	 * Sets the initial branching process based on the given system and
	 * initializes the explicit concurrency relation for the algorithm.
	 */
	private void initialize() {
	
		// get initial BP
		bp = dNodeAS.initialRun;
		bp.setInitialConditions();
		bp.addInitialNodesToAllNodes();
		
		// initialize explicit concurrency relation
		for (DNode d : bp.maxNodes) {
			co.put(d, new HashSet<DNode>());
		}
		for (DNode d : bp.maxNodes) {
			updateConcurrencyRelation(d);
		}

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
		
		//System.out.println("newNode = "+newNode+", °newNode = "+preNew);
		
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
	 * Find all cuts that match the given <code>preConditions</code>.
	 * 
	 * @param preConditions
	 * @param coRelation
	 * @param conditionsToSearch
	 * @return
	 */
	public LinkedList<DNode[]> findEnablingCuts(DNode[] preConditions, Map<DNode, Set<DNode>> coRelation, DNode[] conditionsToSearch)
	{
		DNode.SortedLinearList possibleMatches = new DNode.SortedLinearList();

		if (conditionsToSearch != null) {

			// limited set of nodes to search for cuts
			for (DNode max : conditionsToSearch)
				for (int i=0; i<preConditions.length; i++)
					if (max.id == preConditions[i].id
					    // consider only conditions which are NOT successors of a cutOff event
					    && (max.pre == null || max.pre.length == 0 || !max.pre[0].isCutOff))
					{
						possibleMatches = possibleMatches.add(max);
					}
		} else {
			
			// all nodes to search for cuts
			for (DNode max : bp.getAllConditions())
				for (int i=0; i<preConditions.length; i++)
					if (max.id == preConditions[i].id
              // consider only conditions which are NOT successors of a cutOff event
              && (max.pre == null || max.pre.length == 0 || !max.pre[0].isCutOff))
					{
						possibleMatches = possibleMatches.add(max);
					}
		}
		// possibleMatches contains all nodes of max BP that have the same
		// ID as one of the preconditions, but which are not marked as cutOff 
		// generate cuts from these nodes
		
		
		// the mapping possibleMatches now assigns each node of patternToFind
		// possible candidate nodes for matching in the candidate set and uses
		// at least one node from the partial cut
		DNodeCutGenerator cgen = new DNodeCutGenerator(preConditions, possibleMatches,
				coRelation);
		LinkedList<DNode[]> result = new LinkedList<DNode[]>();
		while (cgen.hasNext())
			result.add(cgen.next());
		
		return result;
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

		short lastEnabledID = -1;
		int beginIDs = 0;
		for (DNode e : dNodeAS.fireableEvents)
		{
		
			//System.out.println(e+" "+toString(e.pre)+":");
			LinkedList<DNode[]> cuts = findEnablingCuts(e.pre, co, null);

			//Iterable<DNode> cutNodes = bp.maxNodes;
			
		  for (DNode[] cutNodes : cuts) {
		    
        //System.out.println(toString(cutNodes));
  
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
  			if (foundNum == cutNodes.length) {
  			  //System.out.println("foundNum == cutNodes.length");
  				continue;	
  			}
  
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
  				
  				assert loc[e.pre.length] != null : "Error, adding invalid enabling location";
  			
  
  				if (lastEnabledID != e.id) {
  
  					// this is the first event with "name"
  					lastEnabledID = e.id;
  					info.enablingLocation.addLast(loc);
  					info.enabledEvents.addLast(e);
  
  					info.synchronizedEvents.addLast(null);
  					
  					// remember first entry of events with this name
  					beginIDs = info.enabledEvents.size()-1;
  				} else {
  					
  					int syncEntry = -1;
  					for (int j=beginIDs; j<info.enablingLocation.size(); j++) {
  						if (Arrays.equals(loc, info.enablingLocation.get(j))) {
  							syncEntry = j;
  							break;
  						}
  					}
  					
  					if (syncEntry == -1) {
  						// this is the first event with "name" at this location
  						info.enablingLocation.addLast(loc);
  						info.enabledEvents.addLast(e);
  
  						info.synchronizedEvents.addLast(null);
  						
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
  			  //System.out.println("incomplete match");
  			}
		  } // for all cuts
		}
		
		return info;
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
		
		int firedEvents = 0;
		
		for (int i=0; i<info.enabledEvents.size(); i++) {

			// flag, set to true iff this event corresponds to an event of an anti-oclet,
			// in this case the event will be added to the branching process (BP) but no
			// subsequent event is fired. In the final result, such anti-events will
			// be removed from the BP. By adding anti-events to the BP, we prevent that
			// an anti-event is "not added" to the BP in every round.
			boolean setAnti = false;
			
			// array containing the post-conditions that are newly created in the BP by
			// firing this event
			DNode[] postConditions = null;
			
			if (info.synchronizedEvents.get(i) == null) {

				//System.out.println("fire "+info.enabledEvents.get(i)+ " at "+toString(info.enablingLocation.get(i)));
				DNode e = info.enabledEvents.get(i);

				if (e.isAnti) {
					// remember that this event was an anti-event
					setAnti = true;
					// System.out.println("not firing "+e+" (is anti)");
				}
				postConditions = bp.fire(e, info.enablingLocation.get(i));

			} else {
				DNode events[] = new DNode[info.synchronizedEvents.get(i).size()];
				events = info.synchronizedEvents.get(i).toArray(events);

				//System.out.println("fire "+toString(events)+ " at "+toString(info.enablingLocation.get(i)));
				boolean isAnti = false;
				for (int j=0;j<events.length;j++)
					if (events[j].isAnti) {
						//System.out.println("not firing anti-event "+events[j]);
						isAnti = true;
						break;
					}
						
				postConditions = bp.fire(events, info.enablingLocation.get(i));
				// remember that this event does not occur in the final result
				if (isAnti) setAnti = true;
			}
			
			// we did not fire this event
			if (postConditions == null)
				continue;

			// update co-relation for all new post-conditions
			for (DNode d : postConditions) {
				co.put(d, new HashSet<DNode>());
			}
			for (DNode d : postConditions) {
				updateConcurrencyRelation(d);
			}

			if (postConditions.length > 0) {
			  
  			DNode newEvent = postConditions[0].pre[0];
  			
        // prevent that an anti-event gets successors in
        // the BP, mark each anti-event as a cutoff event
        if (setAnti) {
          newEvent.isAnti = true;
          newEvent.isCutOff = true;
          // System.out.println("created anti-event "+newEvent);
        } else {
          
          // for all other events, check whether it is a cutOff event
    			if (isCutOffEvent(newEvent)) {
    				newEvent.isCutOff = true;
    			}
    			
        }

  			// remember cutOff on post-conditions as well
  			if (newEvent.isCutOff == true)
  				for (DNode b : postConditions) {
  					b.isCutOff = true;
  					b.isAnti = newEvent.isAnti;	// copy the anti-flag
  				}
			} else {
			  currentPrimeCut = null;
			}
			
			firedEvents++;
			
			if (options_checkProperties)
				checkProperties();
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
		
		LinkedList<DNode[]> cuts = findEnablingCuts(e.pre, co, cut);
			
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
   * Check whether the given event (freshly added to the branching process)
   * is a cut off event according to the chosen search strategy and the
   * chosen equivalence criterion. 
   * 
   * @param event
   * @return
   *      true iff <code>event</code> is a cut off event
   */
  private boolean isCutOffEvent(DNode event) {

    // pick a search strategy for determing the cut off event
    if (options_searchStrat_predecessor) {
      // search for cut off events in the predecessors of the new event
      DNode.SortedLinearList predecessors = new DNode.SortedLinearList();
      currentPrimeCut = bp.getPrimeCut(event, predecessors);
      
      // pick an equivalence criterion for determining equivalent cuts
      if (options_cutOffEquiv_history) {
        if (findEquivalentCut_history(event, currentPrimeCut, predecessors)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_marking) {
        if (findEquivalentCut_marking_predecessor(event, currentPrimeCut, predecessors)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_conditionHistory ) {
        int currentConfigSize = bp.getPrimeCut_configSize;
        if (findEquivalentCut_conditionHistory_signature_lexic(
            currentConfigSize, event, currentPrimeCut, predecessors)) {
          return true;
        }
        
      } else if (options_cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_predecessor(event, currentPrimeCut, predecessors)) {
          return true;
        }
      } else {
        assert false : "No valid search strategy given.";
      }
      
    } else if (options_searchStrat_lexicographic) {
      // search for cut off events in all previously constructed events
      currentPrimeCut = bp.getPrimeCut(event, null);
      int currentConfigSize = bp.getPrimeCut_configSize;

      // pick an equivalence criterion for determining equivalent cuts
      if (options_cutOffEquiv_conditionHistory ) {
        if (findEquivalentCut_conditionHistory_signature_lexic(
            currentConfigSize, event, currentPrimeCut, bp.getAllEvents())) {
          return true;
        }
        
      } else if (options_cutOffEquiv_eventSignature) {
        if (findEquivalentCut_eventSignature_lexik(
            currentConfigSize, event, currentPrimeCut, bp.getAllEvents())) {
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
    // System.out.println("----- next step -----");
    EnablingInfo info = getAllEnabledEvents();
    int fired = fireAllEnabledEvents(info);
    
    if (propertyCheck == PROP_NONE)
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
   * This mapping is update by {@link #updateCCpair(DNode, DNode)} and
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
   * @param newConditions
   *     the new conditions
   * @param equivalentEvent
   *     the old conditions to which <code>newConditions</code> are equivalent
   */
  private void updateCCpair(DNode[] newConditions, DNode equivalentConditions[]) {
    for (int i=0; i<newConditions.length; i++) {
      
      DNode existing = elementary_ccPair.get(equivalentConditions[i]);
      if (existing == null)
        elementary_ccPair.put(newConditions[i], equivalentConditions[i]);
      else
        elementary_ccPair.put(newConditions[i], existing);
      
      if (newConditions[i] == equivalentConditions[i])
        System.out.println("[DNodeBP:updateCCpair] WARNING: making "+newConditions[i]+" equivalent to itself");
      else
        newConditions[i].isCutOff = true; // remember it
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
	private boolean findEquivalentCut_history(DNode newEvent, DNode[] newCut, Iterable<DNode> eventsToCompare) {

		if (equivalentCuts_history(newCut, bp.initialCut)) {
		  updateCCpair(newCut, bp.initialCut);
			return true;
		}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (equivalentCuts_history(newCut, oldCut)) {
			  updateCCpair(newEvent, e); // update the cutOff mapping
			  updateCCpair(newCut, oldCut);
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
	private boolean findEquivalentCut_eventSignature_predecessor(DNode newEvent, DNode[] newCut, Iterable<DNode> eventsToCompare) {
	  // create a new signature for the new cut (compute the numbers of all enabled events)
		byte[] newCutSignature = cutSignature_events_newCut(newCut);

		// check the initial cut for equivalence
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_eventSignature_predecessor(newCutSignature, bp.initialCut)) { 
			  updateCCpair(newCut, bp.initialCut); // make new and old cut equivalent
				return true;
			}
		
		// check the prime cuts of all events against which we compare
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (newCut.length == oldCut.length)
				if (equivalentCuts_eventSignature_predecessor(newCutSignature, oldCut)) {
	        updateCCpair(newEvent, e);     // make new and old event equivalent
	        updateCCpair(newCut, oldCut);  // make new and old cut equivalent
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
   * search ({@link #options_searchStrat_lexicographic}. This strategy must
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
   * A lexicographic search strategy ({@link #options_searchStrat_lexicographic}).
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
	private boolean findEquivalentCut_eventSignature_lexik(
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
			  updateCCpair(newCut, bp.initialCut); // make new and old cut equivalent
				return true;
			}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			
			// do not check the event that has just been added
			// the cuts would be equal...
			if (e == newEvent) continue;
			
			// newCut is only equivalent to oldCut if the configuration
			// of newCut is larger than the configuration of oldCut
			if (newCutConfigSize <= bp.getPrimeCut_configSize)
				continue;	// the old one is larger, not equivalent
			
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (newCut.length == oldCut.length) {
				//System.out.println("prime cut of "+e);
				if (equivalentCuts_eventSignature_lexik(newEventSignature, oldCut)) {
					//System.out.println(toString(newCut)+ " equals " + toString(oldCut));
	        updateCCpair(newEvent, e);    // make new and old event equivalent
	        updateCCpair(newCut, oldCut); // make new and old cut equivalent
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
	private boolean findEquivalentCut_marking_predecessor(DNode newEvent, DNode[] newCut, Iterable<DNode> eventsToCompare) {

		if (equivalentCuts_marking(newCut, bp.initialCut)) {
		  updateCCpair(newCut, bp.initialCut);
			return true;
		}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (equivalentCuts_marking(newCut, oldCut)) {
	      updateCCpair(newEvent, e);    // make new and old event equivalent
	      updateCCpair(newCut, oldCut); // make new and old cut equivalent
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
   * The search strategy for {@link #equivalentCuts_conditionSignature_history(byte[], DNode[], DNode[])}.
   * A lexicographic search strategy ({@link #options_searchStrat_lexicographic}).
   * 
   * @param newEvent
   * @param newCut
   * @param eventsToCompare
   * @return
   */
	private boolean findEquivalentCut_conditionHistory_signature_lexic (
			int newCutConfigSize,
			DNode newEvent,
			DNode[] newCut,
			Iterable<DNode> eventsToCompare)
	{

		byte[] newCutSignature = cutSignature_conditions_init255();
		
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, bp.initialCut)) {
			  updateCCpair(newCut, bp.initialCut);
				return true;
			}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			
			// do not check the event that has just been added
			// the cuts would be equal...
			if (e == newEvent) continue;

			DNode[] oldCut = bp.getPrimeCut(e, null);
			
			// newCut is only equivalent to oldCut if the configuration
			// of newCut is larger than the configuration of oldCut
			if (newCutConfigSize <= bp.getPrimeCut_configSize)
				continue;	// the old one is larger, not equivalent
			
			if (newCut.length == oldCut.length)
				if (equivalentCuts_conditionSignature_history(newCutSignature, newCut, oldCut)) {
	        updateCCpair(newEvent, e); // update the cutOff mapping
	        updateCCpair(newCut, oldCut);
					return true;
				}
		}
		
		return false;
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
	      System.out.println("events "+e+" and "+e0+" are not fully compatible by "+e.pre[i]+" and "+e0.pre[i]);
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
  
	public String getStatistics() {
		int condNum = 0;
		int eventNum = 0;
		int cutOffNum = 0;
		for (DNode n : bp.getAllNodes()) {
			if (n.isEvent) {
				eventNum++;

				if (n.isCutOff && !n.isAnti)
					cutOffNum++;
				
				//System.out.println(n+" : "+toString(bp.getPrimeCut(n, null)));
			}
			else condNum ++;
		}
		return "|B|="+condNum +" |E|="+eventNum+" |E_cutOff|="+cutOffNum;
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
	
	private static final int PROP_NONE = 0;
	private static final int PROP_UNSAFE = 1;
	private static final int PROP_DEADCONDITION = 2;

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
          System.out.println("Found an unsafe prime cut: "+toString(currentPrimeCut));
        }
      }
    }
    
    return true;
  }
	
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
		
		// find a non-cut-off condition in the maximal nodes of the BP
		int numDeadConditions = 0;
		for (DNode b : bp.maxNodes) {
			
			if (!b.isCutOff) {
				// this is a non-cut-off condition, see if corresponds to a
				// terminal place of the source system
				if (!dNodeAS.isTerminal(b)) {
				  // no: this condition is dynamically dead
					System.out.println("Found dead condition: "+b);
	        propertyCheck |= PROP_DEADCONDITION;
					numDeadConditions++;
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
   * @return a string representation of the given {@link DNode} array
   */
  private static String toString(DNode[] arr) {
    String result = "[";
    for (DNode d : arr) {
      result += d+", ";
    }
    return result + "]";
  }

  /**
   * @param arr
   * @return a string representation of the given <code>byte</code> array
   */
  private static String toString(byte[] arr) {
    String result = "[";
    for (byte d : arr) {
      result += d+", ";
    }
    return result + "]";
  }
  
}
