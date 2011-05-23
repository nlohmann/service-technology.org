package hub.top.mia;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Stack;

/**
 * Marking of a given PetriNet
 * 
 * @author darsinte
 * 
 */
public class Marking implements Cloneable {
	private HashSet<Place> markedPlaces;
	private PetriNet net;

	/** direct successors **/
	private HashSet<Marking> forwardSuccessor, backSuccesors;
	//private HashSet<Marking> cone;

	/** is true if marking is part of sub-cone and also of the cone**/
	private boolean hasCone;
	
	/** reference to places of a marking sorted in alphabetical order**/
	private Place[] sortedPlaces;
	
	/** TSCCs related fields **/
	private int index, lowlink, scc;
	private static int nextIndex, nextSCC;

	private boolean isInTSCC;
	private Cone cone;
	
	public Marking() {
		markedPlaces = new HashSet<Place>();
		setForwardSuccessor(new HashSet<Marking>());
		setBackSuccesors(new HashSet<Marking>());
		//cone = new HashSet<Marking>();
		index = lowlink = setScc(0);
		isInTSCC = false;
	}

	public Marking(PetriNet net) {
		this();
		this.net = net;
	}

	/**
	 * convert from BitSet to places representation
	 * need places of net in lexicographical order
	 * @param safeMarking
	 * @param sortedPlaces
	 */
	public Marking(BitSet safeMarking, Place[] sortedPlaces) {		
		this();
		
		// compute marked places
		for (int i = 0; i<sortedPlaces.length; i++) {
			if (safeMarking.get(i) == true) {
				markedPlaces.add(sortedPlaces[i]);
			}
		}
	}
	
	public Marking(Cone cone) {
		this();
		this.setCone(cone);
	}
	
	public void setNet(PetriNet net) {
		this.net = net;
	}

	public PetriNet getNet() {
		return net;
	}

	/**
	 * check if transition is forward enabled
	 * @param transition
	 * @return
	 */
	public boolean isForwardEnabled(Transition transition) {
		List<Place> preSet = transition.getPreSet();

		for (Place place : preSet) {
			if (!getMarkedPlaces().contains(place)) {
				return false;
			}
		}

		return true;
	}
	
	/**
	 * check if transition is backward enabled
	 * @param transition
	 * @return
	 */
	public boolean isBackwardEnabled(Transition transition) {
		List<Place> postSet = transition.getPostSet();

		for (Place place : postSet) {
			if (!getMarkedPlaces().contains(place)) {
				return false;
			}
		}

		return true;
	}
	
	/**
	 * fire transition backward
	 * @param transition
	 * @return
	 */
	public Marking fireBackward(Transition transition){
		Marking result = new Marking();

		HashSet<Place> newPlaces = (HashSet<Place>) getMarkedPlaces().clone();

		List<Place> preSet = transition.getPreSet();
		List<Place> postSet = transition.getPostSet();

		// delete places in postset
		for (Place place : postSet) {
			newPlaces.remove(place);
		}

		// add places from preset
		for (Place place : preSet) {
			newPlaces.add(place);
		}

		result.setMarkedPlaces(newPlaces);
		
		return result;
	}
	
	/**
	 * fire transition forward
	 * @param transition
	 * @return
	 */
	public Marking fireForward(Transition transition) {
		Marking result = new Marking();

		HashSet<Place> newPlaces = (HashSet<Place>) getMarkedPlaces().clone();

		List<Place> preset = transition.getPreSet();
		List<Place> postSet = transition.getPostSet();

		// delete places in preset
		for (Place place : preset) {
			newPlaces.remove(place);
		}

		// add places from postset
		for (Place place : postSet) {
			newPlaces.add(place);
		}

		result.setMarkedPlaces(newPlaces);
		
		return result;

	}

	/**
	 * generate backward and forward successors
	 */
	public void generateSuccessors() {
		for (Place place : markedPlaces) {
			// generate forward successors
			LinkedList<Transition> postSet = (LinkedList<Transition>) place
					.getPostSet();
			for (Transition transition : postSet) {
				if (isForwardEnabled(transition)) {
					Marking successor = fireForward(transition);
					
					// test whether successor is already in cone
					Marking temp = cone.getElement(successor);
					
					if (temp != null) {
						forwardSuccessor.add(temp);
					} else {
						successor.setCone(cone);
						forwardSuccessor.add(successor);
					}
				}
			}
			
			// generate backward successors
			LinkedList<Transition> preSet = (LinkedList<Transition>) place.getPreSet();
			for (Transition transition : preSet) {
				if (isBackwardEnabled(transition)) {
					Marking successor = fireBackward(transition);
					
					// test whether successor is already in cone
					Marking temp = cone.getElement(successor);
					
					if (temp != null) {
						backSuccesors.add(temp);
					} else {
						successor.setCone(cone);
						backSuccesors.add(successor);
					}
				}
			}
		}
	}
	
	@Override
	public String toString() {
		String result = "[";
		
		for (Place place : markedPlaces) {
			result += place.getName();
		}
		
		result += "]";
		return result;
	}
	
	public void addPlace(Place place) {
		getMarkedPlaces().add(place);
	}

	public void setHasCone(boolean hasCone) {
		this.hasCone = hasCone;
	}

	public boolean isHasCone() {
		return hasCone;
	}

	@Override
	public boolean equals(Object other) {
		if (! (other instanceof Marking)) {
			return false;
		}
		
		Marking markingOther = (Marking) other;
		if (size() != markingOther.size()) {
			return false;
		}
		
		for (Place place : markedPlaces) {
			if (!markingOther.isMarked(place)) {
				return false;
			}
		}
		
		return true;
	}

	public void setMarkedPlaces(HashSet<Place> markedPlaces) {
		this.markedPlaces = markedPlaces;
	}

	public HashSet<Place> getMarkedPlaces() {
		return markedPlaces;
	}
	
	/**
	 * test whether a certain place is marked
	 * @param place
	 * @return
	 */
	public boolean isMarked(Place place) {
		for (Place marked : markedPlaces) {
			if (marked.getUniqueIdentifier().equals(place.getUniqueIdentifier())) {
				return true;
			}
		}
		
		return false;
	}	
	
	/**
	 * size of the marking
	 * @return
	 */
	public int size() {
		return markedPlaces.size();
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public int getIndex() {
		return index;
	}

	public int setLowlink(int lowlink) {
		this.lowlink = lowlink;
		return lowlink;
	}

	public int getLowlink() {
		return lowlink;
	}
	
	/**
	 * generate next index
	 * @return
	 */
	public static int getNextIndex() {
		nextIndex++;
		return nextIndex;
	}
	
	/**
	 * generate next scc number
	 * @return
	 */
	public static int getNextSCC() {
		nextSCC++;
		return nextSCC;
	}
	
	public boolean indexUndefined() {
		if (index == 0) {
			return true;
		}
		
		return false;
	}

	public void setInTSCC(boolean isInTSCC) {
		this.isInTSCC = isInTSCC;
	}

	public boolean isInTSCC() {
		return isInTSCC;
	}

	public int setScc(int scc) {
		this.scc = scc;
		return scc;
	}

	public int getScc() {
		return scc;
	}

	public void setForwardSuccessor(HashSet<Marking> forwardSuccessor) {
		this.forwardSuccessor = forwardSuccessor;
	}

	public HashSet<Marking> getForwardSuccessor() {
		return forwardSuccessor;
	}

	public void setBackSuccesors(HashSet<Marking> backSuccesors) {
		this.backSuccesors = backSuccesors;
	}

	public HashSet<Marking> getBackSuccesors() {
		return backSuccesors;
	}

	public void setCone(Cone cone) {
		this.cone = cone;
	}

	public Cone getCone() {
		return cone;
	}
}
