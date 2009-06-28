package hub.top.greta.oclets.canonical;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

import hub.top.adaptiveSystem.AdaptiveSystem;

public class DNodeBP {
	
	public static boolean options_cutOffStrat_history = false;
	public static boolean options_cutOffStrat_lexikographic = false;
	
	private AdaptiveSystem 	as;
	private DNodeAS			dNodeAS;
	
	private DNodeSet		bp;
	
	private HashMap<DNode, Set<DNode>> co = new HashMap<DNode, Set<DNode> >();

	
	public DNodeBP(AdaptiveSystem as) {
		this.as = as;
		initialize();
		
		options_cutOffStrat_history = false;
		options_cutOffStrat_lexikographic = false;
	}
	
	public void initialize() {
		// instantiate DNode representation of the adaptive system
		dNodeAS = new DNodeAS(as);	
		// get initial BP
		bp = dNodeAS.apEncoding;
		bp.setInitialConditions();
		
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
			if (preNew == preExisting ) {
				// both are concurrent
				coset.add(existing);
				cosetEx.add(newNode);
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
			}
		}
		
		co.put(newNode, coset);
	}
	
	public LinkedList<DNode[]> findEnablingCuts(DNode[] preConditions, Map<DNode, Set<DNode>> coRelation, DNode[] conditionsToSearch)
	{
		DNode.SortedLinearList possibleMatches = new DNode.SortedLinearList();

		
		if (conditionsToSearch != null) {

			// limited set of nodes to search for cuts
			for (DNode max : conditionsToSearch)
				for (int i=0; i<preConditions.length; i++)
					if (max.id == preConditions[i].id && !max.isCutOff) {
						possibleMatches = possibleMatches.add(max);
					}
		} else {
			
			// all nodes to search for cuts
			for (DNode max : bp.getAllConditions())
				for (int i=0; i<preConditions.length; i++)
					if (max.id == preConditions[i].id && !max.isCutOff) {
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
	
	class EnablingInfo {
		public LinkedList< DNode > enabledEvents = new LinkedList<DNode>();
		public LinkedList< DNode[] > enablingLocation = new LinkedList<DNode[]>();
		
		public LinkedList< LinkedList<DNode> >
						synchronizedEvents = new LinkedList< LinkedList<DNode> >();
	}
	
	private static String toString(DNode[] arr) {
		String result = "[";
		for (DNode d : arr) {
			result += d+", ";
		}
		return result + "]";
	}
	
	private EnablingInfo getAllEnabledEvents() {
		EnablingInfo info = new EnablingInfo();

		short lastEnabledID = -1;
		int beginIDs = 0;
		for (DNode e : dNodeAS.fireableEvents)
		{
		
			//System.out.println(e+" "+toString(e.pre)+":");
			LinkedList<DNode[]> cuts = findEnablingCuts(e.pre, co, null);
			/*
			for (DNode[] cut : cuts) {
				System.out.println(toString(cut));
			}
			*/

			//Iterable<DNode> cutNodes = bp.maxNodes;
			
		  for (DNode[] cutNodes : cuts) {

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
				
				assert loc[e.pre.length] != null : "Error, adding invalid enabling location";
			

				if (lastEnabledID != e.id) {

					// this is the first event with "id"
					lastEnabledID = e.id;
					info.enablingLocation.addLast(loc);
					info.enabledEvents.addLast(e);

					info.synchronizedEvents.addLast(null);
					
					// remember first entry of events with this id
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
						// this is the first event with "id" at this location
						info.enablingLocation.addLast(loc);
						info.enabledEvents.addLast(e);

						info.synchronizedEvents.addLast(null);
						
					} else {
						// this is the second/n-th event with "id" at the same
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
			}
		  }
		}
		
		return info;
	}
	
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

			DNode newEvent = postConditions[0].pre[0];
			if (isCutOffEvent(newEvent)) {
				newEvent.isCutOff = true;
			}
			
			// to prevent that an anti-event that is not fired gets successors in
			// the BP, mark each anti-event as a cutoff event
			if (setAnti) {
				newEvent.isAnti = true;
				newEvent.isCutOff = true;
				// System.out.println("created anti-event "+newEvent);
			}
			
			// remember cutOff on post-conditions as well
			if (newEvent.isCutOff == true)
				for (DNode b : postConditions) {
					b.isCutOff = true;
					b.isAnti = newEvent.isAnti;	// copy the anti-flag
				}
			
			firedEvents++;
		}
		return firedEvents;
	}
	
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
	
	private boolean findEquivalentCut_history(DNode[] newCut, Iterable<DNode> eventsToCompare) {

		if (equivalentCuts_history(newCut, bp.initialCut))
			return true;
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (equivalentCuts_history(newCut, oldCut))
				return true;
		}
		
		return false;
	}
	
	private boolean findEquivalentCut_signature(DNode[] newCut, Iterable<DNode> eventsToCompare) {
		byte[] newCutSignature = cutSignature_newCut(newCut);
		
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_signature(newCutSignature, bp.initialCut))
				return true;
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (newCut.length == oldCut.length)
				if (equivalentCuts_signature(newCutSignature, oldCut))
					return true;
		}
		
		return false;
	}
	
	private boolean findEquivalentCut_signature_lexik(DNode eventToIgnore, DNode[] newCut, Iterable<DNode> eventsToCompare) {
		
		// TODO: do not compare wrt. enabling of events but wrt. embedding of all
		// preconditions of events, i.e. wrt. equality of the histories that matter
		
		byte[] newCutSignature = cutSignature_newCut(newCut);
		
		if (newCut.length == bp.initialCut.length)
			if (equivalentCuts_signature(newCutSignature, bp.initialCut)) {
				System.out.println(toString(newCut)+ " equals initial");
				return true;
			}
		
		Iterator<DNode> it = eventsToCompare.iterator();
		while (it.hasNext()) {
			DNode e = it.next();
			
			// do not check the event that has just been added
			// the cuts would be equal...
			if (e == eventToIgnore) continue;
			
			DNode[] oldCut = bp.getPrimeCut(e, null);
			if (newCut.length == oldCut.length) {
				System.out.println("prime cut of "+e);
				if (equivalentCuts_signature_lexik(newCutSignature, oldCut)) {
					System.out.println(toString(newCut)+ " equals " + toString(oldCut));
					return true;
				}
			}
		}
		
		return false;
	}
	
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
	
	private boolean equivalentCuts_signature(byte[] newCutSignature, DNode oldCut[]) {

		int sigIndex = 0;
		for (DNode e : dNodeAS.fireableEvents) {

			// on non-lexikograhic search: "oldCut" has all successor events
			// built, only count those
			if (countSuccEvents_oldCut(oldCut, e) != newCutSignature[sigIndex])
				return false;
			sigIndex++;
		}
		
		for (DNode e : dNodeAS.preConEvents) {
			if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
				return false;
			sigIndex++;
		}
		
		return true;
	}
	
	private boolean equivalentCuts_signature_lexik(byte[] newCutSignature, DNode oldCut[]) {

		int sigIndex = 0;
		for (DNode e : dNodeAS.fireableEvents) {
			// on lexikographic search: check full enabling of events as
			// checked "oldCut" may not have all successor events built
			int en_num = isEnabledIn(e, oldCut);
			System.out.println(e+": "+ en_num + " , "+newCutSignature[sigIndex]);
			if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
				return false;
			sigIndex++;
		}
		
		for (DNode e : dNodeAS.preConEvents) {
			int en_num = isEnabledIn(e, oldCut);
			System.out.println(e+": "+ en_num + " , "+newCutSignature[sigIndex]);
			if (isEnabledIn(e, oldCut) != newCutSignature[sigIndex])
				return false;
			sigIndex++;
		}
		
		return true;
	}
	
	private byte[] cutSignature_newCut(DNode[] newCut) {
		
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
	
	
	private byte countSuccEvents_oldCut(DNode[] oldCut, DNode event) {
		HashSet<DNode> cut = new HashSet<DNode>(oldCut.length);
		for (int i=0; i<oldCut.length; i++) cut.add(oldCut[i]);
		
		HashSet<DNode> succEvents = new HashSet<DNode>();
		for (int i=0; i<oldCut.length; i++) {
			if (oldCut[i].post == null) continue;
			for (int j=0; j<oldCut[i].post.length; j++)
				if (oldCut[i].post[j].id == event.id)
					for (int k=0; k<oldCut[i].post[j].causedBy.length; k++)
						if (oldCut[i].post[j].causedBy[k] == event.localId)
							succEvents.add(oldCut[i].post[j]);
		}
		
		byte fullCount = 0;
		for (DNode e : succEvents) {
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

	private boolean isCutOffEvent(DNode event) {

		if (options_cutOffStrat_history) {
			
			DNode.SortedLinearList predecessors = new DNode.SortedLinearList();
			DNode[] primeCut = bp.getPrimeCut(event, predecessors);

			if (findEquivalentCut_history(primeCut, predecessors)) {
				return true;
			}
			
		} else if (options_cutOffStrat_lexikographic) {

			DNode[] primeCut = bp.getPrimeCut(event, null);
	
			if (findEquivalentCut_signature_lexik(event, primeCut, bp.getAllEvents())) {
				return true;
			}
			
		} else {
			
			DNode.SortedLinearList predecessors = new DNode.SortedLinearList();
			DNode[] primeCut = bp.getPrimeCut(event, predecessors);
	
			if (findEquivalentCut_signature(primeCut, predecessors)) {
				return true;
			}
		}
		
		return false;
	}
	
	public int step() {
		// System.out.println("----- next step -----");
		EnablingInfo info = getAllEnabledEvents();
		return fireAllEnabledEvents(info);
	}
	
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
	
	public String toDot() {
		return bp.toDot();
	}
}
