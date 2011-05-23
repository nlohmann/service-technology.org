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
	private HashSet<Marking> cone;

	/** is true if marking is part of sub-cone and also of the cone**/
	private boolean hasCone;
	
	/** reference to places of a marking sorted in alphabetical order**/
	private Place[] sortedPlaces;
	
	/** TSCCs related fields **/
	private int index, lowlink, scc;
	private static int nextIndex, nextSCC;
	
	private Stack<Marking> stack;
	private boolean isInTSCC;
	
	public Marking() {
		markedPlaces = new HashSet<Place>();
		setForwardSuccessor(new HashSet<Marking>());
		setBackSuccesors(new HashSet<Marking>());
		cone = new HashSet<Marking>();
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
					forwardSuccessor.add(fireForward(transition));
				}
			}
			
			// generate backward successors
			LinkedList<Transition> preSet = (LinkedList<Transition>) place.getPreSet();
			for (Transition transition : preSet) {
				if (isBackwardEnabled(transition)) {
					backSuccesors.add(fireBackward(transition));
				}
			}
		}
	}

	
	/**
	 * check if element is part of the cone
	 * @param element
	 * @return
	 */
	public boolean isInCone(Marking element) {
		for (Marking node : cone) {
			if (element.equals(node)) {
				return true;
			}
		}
		
		return false;
	}
	
	/**
	 * compute cone having as a source current marking 
	 * check that all elements of subcones are part of the cone
	 * @param subcone
	 */
	public boolean computeCone(HashSet<Marking> subcone) {
		// start from current marking, recursively generate successors until
		// nothing new is added
		// initially add this to cone

		boolean expanded = false;
		cone.add(this);
		
		// check if current node is part of cone
		for (Marking element : subcone) {
			if (element.equals(this)) {
				element.setHasCone(true);
			}
		}
		
		// set with intermediate successors
		HashSet<Marking> successors = new HashSet<Marking>();
		
		// set with newly generated successors
		ArrayList<Marking> currentSuccessors = new ArrayList<Marking>();
		currentSuccessors.add(this);
		
		ArrayList<Marking> newSuccessors = new ArrayList<Marking>();
		do {
			expanded  = false;
			newSuccessors.clear();
			
			for (Marking node : currentSuccessors) {
				node.generateSuccessors();
				
				// feed intermediate successors set with all forward and backward successors of node successor 
				successors.clear();
				successors.addAll(node.getForwardSuccessor());
				successors.addAll(node.getBackSuccesors());
				
				for (Marking newSuccessor : successors) {
					// add element to cone if it is not in cone and not generated previously
					if (!isInCone(newSuccessor) && !newSuccessors.contains(newSuccessor)) {
						// check if some element of the sub-cone is equal to successor
						for (Marking element : subcone) {
							if (element.equals(newSuccessor)) {
								element.setHasCone(true);
							}
						}
						
						// add successor to newly generated successors list
						newSuccessors.add(newSuccessor);
						
						// set expanded to true
						expanded = true;
					}
				}
			}
			
			// add new elements
			cone.addAll(newSuccessors);
			
			// add current successors to the list for the next iteration
			currentSuccessors.clear();
			currentSuccessors.addAll(newSuccessors);
		} while (expanded);
		
		// check if all sub-cone elements are part of the cone
		for (Marking element : subcone) {
			if (!element.isHasCone()) {
				return false;
			}
		}
		
		return true;
	}

	/**
	 * print cone with source this node
	 */
	public void printCone() {
		System.out.println("Cone generated by: " + toString());
		for (Marking element : cone) {
			System.out.println(element);
		}
		
		System.out.println();
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
	
	/**
	 * generate TCSSs of cone with source this
	 * use Tarjan's algorithm
	 */
	public void generateTCSSs() {
		stack = new Stack<Marking>();
		
		for (Marking element : cone) {
			if (element.indexUndefined()) {
				strongConnect(element);
			}
		}
	}
	
	private void strongConnect(Marking current) {
		int index = Marking.getNextIndex();
		current.setIndex(index);
		current.setLowlink(index);
		
		// push current node into the stack
		stack.push(current);
		
		HashSet<Marking> successors = current.getForwardSuccessor();
		
		for (Marking successor : successors) {			
			if (successor.indexUndefined()) {
				strongConnect(successor);
				
				int temp = successor.getIndex();
				if (temp < current.getIndex()) {
					current.setIndex(temp);
				}
			} else if (stack.contains(successor)){
				int temp = successor.getIndex();
				if (temp < current.getIndex()) {
					current.setIndex(temp);
				}
			}
		}
		
		// if current is a root node, pop the stack and generate TSCC
		if (current.getLowlink() == current.getIndex()) {
			Marking node; 
			
			// get next scc number
			int scc = Marking.getNextSCC();
			ArrayList<Marking> sccNodes = new ArrayList<Marking>();
			do {
				// mark node as part of current scc
				node = stack.pop();
				node.setScc(scc);
				
				sccNodes.add(node);
			} while(!node.equals(current));
			
			// check if all successors of all scc members are in same scc - found tscc
			boolean isTSCC = true;
			for (Marking sccNode : sccNodes) {
				HashSet<Marking> sccSuccessors = sccNode.getForwardSuccessor();
				
				for (Marking sccSuccessor : sccSuccessors) {
					if (sccSuccessor.getScc() != sccNode.getScc()) {
						isTSCC = false;
					}
				}
			}
			
			// if we found a tscc
			if (isTSCC) {
				System.out.println("Found TSCC....");
				for (Marking sccNode : sccNodes) {
					System.out.println(sccNode);
					sccNode.setInTSCC(true);
				}
			}
		}
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
}
