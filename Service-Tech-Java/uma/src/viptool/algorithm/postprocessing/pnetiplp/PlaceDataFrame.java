package viptool.algorithm.postprocessing.pnetiplp;

/**
 * This class stores all necessary information to build the linear inequality
 * system which is used to identify the implicit given places
 * 
 * @author thomas
 */
public class PlaceDataFrame implements Comparable<PlaceDataFrame> {
	/**
	 * This variable store the unique id string of the place from the pnml file
	 */
	private String	placeId_;

	/**
	 * The result of this algorithm depends of the order of the places. This
	 * variable is used to give the places a useful order.
	 */
	private int		score_	= 0;

	/**
	 * This variable stores the index of this place into the places structure.
	 */
	private int		index_;

	/**
	 * This variable stores the initial marking of this place
	 */
	private int		initialMarking_;

	/**
	 * Array for incoming arc weights
	 */
	private int[]	incomingArcWeights_;

	/**
	 * Array for outgoing arc weights
	 */
	private int[]	outgoingArcWeights_;
	
	/**
	 * visible name of the place for debug
	 */
	private String name_ = "";

	/**
	 * This constructor build a new instance of PlaceDataFrame with the fixed
	 * attributes
	 * 
	 * @param placeId
	 *            id string of the place from the pnml file
	 * @param index
	 *            index of the place into the places structure
	 * @param initialMarking
	 *            initial marking of the place
	 * @param transitioncount
	 *            transition count of the net
	 */
	PlaceDataFrame(String placeId, int index, int initialMarking,
			int transitioncount) {
		placeId_ = placeId;
		index_ = index;
		initialMarking_ = initialMarking;
		incomingArcWeights_ = new int[transitioncount];
		outgoingArcWeights_ = new int[transitioncount];
	}

	/**
	 * This method returns the transition count
	 * 
	 * @return transition count
	 */
	public int getTransitionCount() {
		return incomingArcWeights_.length;
	}

	/**
	 * This method set the incoming arc weight from a transition to the place
	 * 
	 * @param transition
	 *            index of the transition
	 * @param weight
	 *            arc weight or 0 for no arc
	 */
	public void setIncomingArcWeight(int transition, int weight) {
		incomingArcWeights_[transition] = weight;
	}

	/**
	 * This method returns the arc weight for the given transition.
	 * 
	 * @param transition
	 *            index of the transition
	 * @return arc weight or 0 if there is no arc
	 */
	public int getIncomingArcWeight(int transition) {
		return incomingArcWeights_[transition];
	}

	/**
	 * This method sets the outgoing arc weight form the place to a transition
	 * 
	 * @param transition
	 *            index of the transition
	 * @param weight
	 *            arc weight
	 */
	public void setOutgoingArcWeight(int transition, int weight) {
		outgoingArcWeights_[transition] = weight;
	}

	/**
	 * This method returns the outgoing arc weight for the given transition
	 * 
	 * @param transition
	 *            index of the transition
	 * @return arc weight or 0 if there is no arc
	 */
	public int getOutgoingArcWeight(int transition) {
		return outgoingArcWeights_[transition];
	}

	/**
	 * This method returns the initial marking of this place.
	 * 
	 * @return initial marking
	 */
	public int getInitialMarking() {
		return initialMarking_;
	}

	/**
	 * This method returns the internal index of this place.
	 * 
	 * @return index of this place
	 */
	public int getIndex() {
		return index_;
	}

	/**
	 * This method returns the place id string for the pnml file of this place.
	 * 
	 * @return id string
	 */
	public String getPlaceId() {
		return placeId_;
	}

	/**
	 * This method sets the score of this place.
	 * 
	 * @param score
	 *            score of this place
	 */
	public void setScore(int score) {
		score_ = score;
	}

	/**
	 * This method returns the score of this place.
	 * 
	 * @return score of this place
	 */
	public int getScore() {
		return score_;
	}

	/**
	 * Implementation of comparable. This method returns score of other place -
	 * score of this place.
	 */
	public int compareTo(PlaceDataFrame o) {
		return o.getScore() - this.getScore();
	}

	/**
	 * This method returns the visible name of the place.
	 * 
	 * @return
	 * 		visible name
	 */
	public String getName(){
		return name_;
	}
	
	/**
	 * This method sets the name of the place.
	 * 
	 * @param name
	 * 		name of the place
	 */
	public void setName(String name){
		name_ = name;
	}
}
