package hub.top.mia;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

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
	private HashSet<Marking> successors;
	private HashSet<Marking> cone;

	public Marking() {
		markedPlaces = new HashSet<Place>();
		successors = new HashSet<Marking>();
		cone = new HashSet<Marking>();
	}

	public Marking(PetriNet net) {
		markedPlaces = new HashSet<Place>();
		successors = new HashSet<Marking>();
		cone = new HashSet<Marking>();
		this.net = net;
	}

	public Marking(SafeMarking marking) {
		// TODO: convert from SafeMarking back to marking
	}

	public void setNet(PetriNet net) {
		this.net = net;
	}

	public PetriNet getNet() {
		return net;
	}

	public boolean isEnabled(Transition transition) {
		List<Place> preset = transition.getPreSet();

		for (Place place : preset) {
			if (!markedPlaces.contains(place)) {
				return false;
			}
		}

		return true;
	}

	public Marking fire(Transition transition) {
		Marking result = new Marking();

		HashSet<Place> newPlaces = (HashSet<Place>) markedPlaces.clone();

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

		return result;

	}

	public void generateSuccessors() {
		for (Place place : markedPlaces) {
			LinkedList<Transition> transitions = (LinkedList<Transition>) place
					.getPostSet();

			for (Transition transition : transitions) {
				if (isEnabled(transition)) {
					getSuccessors().add(fire(transition));
				}
			}
		}
	}

	public void setSuccessors(HashSet<Marking> successors) {
		this.successors = successors;
	}

	public HashSet<Marking> getSuccessors() {
		return successors;
	}
	
	/**
	 * compute cone and check that all elements of subcones are part of the cone
	 * @param current
	 */
	public void computeCone(Marking current, HashSet<Marking> subcone) {
		// TODO: compute cone from current submarking, check if subcone elements
		// are contained in cone
		// start from current marking, recursively generate successors until
		// nothing new is added

		// initially add this to cone

		boolean expanded = false;
		cone.add(this);
		HashSet<Marking> newSuccessors = new HashSet<Marking>();
		
		do {
			expanded  = false;
			newSuccessors.clear();
			
			for (Marking successor : cone) {
				successor.generateSuccessors();
				HashSet<Marking> successors = successor.getSuccessors();

				for (Marking newSuccessor : successors) {
					if (!cone.contains(newSuccessor)) {
						newSuccessors.add(newSuccessor);
					}
				}
			}
			
			// add new elements
			cone.addAll(newSuccessors);
		} while (expanded);

	}

	public void addPlace(Place place) {
		markedPlaces.add(place);
	}

}
