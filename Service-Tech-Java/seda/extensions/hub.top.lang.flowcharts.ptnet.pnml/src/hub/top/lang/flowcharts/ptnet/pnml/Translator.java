package hub.top.lang.flowcharts.ptnet.pnml;

import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PageHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;

public abstract class Translator {
	DiagramNode node;
	PageHLAPI page;
	NodePair pair;
	Hashtable<DiagramNode, NodePair> table;	

	// ID - count for places, transitions and arcs
	int placeCount;
	int transCount;
	int arcCount;
	
	// set indices of the last inserted place, transition and arc
	public void setCounts (int placeCt, int transCt, int arcCt){
		this.placeCount = placeCt;
		this.transCount = transCt;
		this.arcCount = arcCt;
	}
	
	// returns the index of the last inserted Place
	public int getPlaceCount() {
		return placeCount;
	}

	// set index of the last inserted Place
	public void setPlaceCount(int placeCount) {
		this.placeCount = placeCount;
	}

	// returns the index of the last inserted Transition
	public int getTransCount() {
		return transCount;
	}

	// set index of the last inserted Transition
	public void setTransCount(int transCount) {
		this.transCount = transCount;
	}

	// returns the index of the last inserted Arc
	public int getArcCount() {
		return arcCount;
	}

	// set index of the last inserted Arc
	public void setArcCount(int arcCount) {
		this.arcCount = arcCount;
	}

	public PageHLAPI getPage() {
		return page;
	}

	public void setPage(PageHLAPI page) {
		this.page = page;
	}

	public NodePair getPair() {
		return pair;
	}

	public void setPair(NodePair pair) {
		this.pair = pair;
	}

	public Hashtable<DiagramNode, NodePair> getTable() {
		return table;
	}

	public void setTable(Hashtable<DiagramNode, NodePair> table) {
		this.table = table;
	}

	public void setNode(DiagramNode node){
		this.node= node;
	}
	
	public DiagramNode getNode(){
		return(this.node);
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{	}
	
	/**
	 * creates an XML-conform label from a String 
	 * replaces therefore special characters by their HTML notation	 
	 * if label is empty there will be created an alternative name
	 * @param label - the diagramNode's label, x - 't' for transitions, 'p' for places
	 * @return the PNML Label
	 */
	protected String createPNMLLabel (String label, Character x){
		if (label == null || label.length() == 0){
			
			// Counts werden nicht hoch gezaehlt, weil sie noch gleich dem Label des aktuellen Knotens sind
			if (x == 'p') {
				label = "p"+this.getPlaceCount();
			}
			else if (x == 't') {
				label = "t" + this.getTransCount();
			}
			return label;
		}
		
		label = label.replaceAll("&", "&amp;");
		label = label.replaceAll("<", "&lt;");
		label = label.replaceAll(">", "&gt;");
		label = label.replaceAll("'", "&apos;");
		label = label.replaceAll("\"", "&quot;");
		label = label.replaceAll("ß", "ss");
		label = label.replaceAll("Ä", "Ae");
		label = label.replaceAll("ä", "ae");
		label = label.replaceAll("Ö", "Oe");
		label = label.replaceAll("ö", "oe");
		label = label.replaceAll("Ü", "Ue");
		label = label.replaceAll("ü", "ue");
		return label;
	}
	
	/**
	 * nodes that have more than one incoming arc need an InterfaceIn
	 * @param p - the actual startPlace
	 */
	protected void addInterfaceIn(PlaceHLAPI pStart){
		try
		{
			this.transCount++;
			TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount());
			t.setContainerPageHLAPI(this.getPage());
			
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t, pStart, this.getPage());
			
			for (DiagramArc arc: this.node.getIncoming()){
				this.placeCount++;
				PlaceHLAPI pIn = new PlaceHLAPI("p"+this.getPlaceCount()); pIn.setNameHLAPI(new NameHLAPI(this.createPNMLLabel(node.getLabel()+"_In_"+arc.getSrc().getLabel(), 'p')));
				pIn.setContainerPageHLAPI(this.getPage());				
			
				this.arcCount++;
				new ArcHLAPI("a"+this.getArcCount(), pIn, t, page);
				this.pair.addStart(pIn);				
				this.pair.setOtherEnd(pIn, arc.getSrc());
			}
		}
		catch (InvalidIDException e){
				System.err.println("Invalid Id generated while adding InterfaceIn to node " + node.getLabel());
				e.printStackTrace();
		}
		catch (VoidRepositoryException e){
				e.printStackTrace();
		}
	}
	
	/**
	 * nodes that have more than one outgoing arc need an InterfaceOut
	 * @param p - the actual endPlace
	 */
	protected void addInterfaceOut(PlaceHLAPI pEnd){
		try
		{
			this.transCount++;
			TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount());
			t.setContainerPageHLAPI(this.getPage());
			
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pEnd, t, this.getPage());
			
			for (DiagramArc arc: this.node.getOutgoing()){
				this.placeCount++;
				PlaceHLAPI pOut = new PlaceHLAPI("p"+this.getPlaceCount()); pOut.setNameHLAPI(new NameHLAPI(this.createPNMLLabel(node.getLabel()+"_Out_"+arc.getTarget().getLabel(), 'p')));
				pOut.setContainerPageHLAPI(this.getPage());				
			
				this.arcCount++;
				new ArcHLAPI("a"+this.getArcCount(), t, pOut, page);
				this.pair.addEnd(pOut);
				this.pair.setOtherEnd(pOut, arc.getTarget());
			}
		}
		catch (InvalidIDException e){
				System.err.println("Invalid Id generated while adding InterfaceIn to node " + node.getLabel());
				e.printStackTrace();
		}
		catch (VoidRepositoryException e){
				e.printStackTrace();
		}
	}
	
	protected class NodePair{
		List<PlaceHLAPI> startPlaces;
		List<PlaceHLAPI> endPlaces;
		// for each Node the other end of the according arc
		Hashtable<PlaceHLAPI, DiagramNode> otherEnds;
		
		/**
		 * 
		 * @return the Hashtable including all Places and the DiagramNodes at the other ends of the according arcs 
		 */
		public Hashtable<PlaceHLAPI, DiagramNode> getOtherEnds() {
			return otherEnds;
		}

		public void setOtherEnds(Hashtable<PlaceHLAPI, DiagramNode> otherEnds) {
			this.otherEnds = otherEnds;
		}

		/**
		 * set the other end of an arc according to a place in the Pair
		 * @param place - the place
		 * @param end - the other end of the according arc
		 */
		public void setOtherEnd (PlaceHLAPI place, DiagramNode end){
			if (!(this.startPlaces.contains(place) || this.endPlaces.contains(place))){
				throw new NullPointerException("Error in NodePair! Trying to add a place which doesn't exist.");
			}
			else 
				if (this.otherEnds == null) {
					this.otherEnds = new Hashtable<PlaceHLAPI, DiagramNode>();
				}
				this.otherEnds.put(place, end);
		}
		
		/**
		 * @param end - the source or target of the arc
		 * @return the place according to the arc which source or target is given
		 */
		public PlaceHLAPI getOtherEndPlace (DiagramNode end){
			if (this.getOtherEnds() == null){
				return null;
			}
			if (!this.getOtherEnds().containsValue(end)){
				System.out.println("Error in NodePair.getOtherEndPlace() ! I do not know the " + end.getClass().getSimpleName() + " " + end.getLabel());
				return null;
			}
			else 
			{
				for (PlaceHLAPI p : this.getOtherEnds().keySet()){
					if (this.getOtherEnds().get(p) == end) return p;
				}
			}
			return null;
		}
		
		public NodePair(PlaceHLAPI start, PlaceHLAPI end){
			this.startPlaces = new Vector<PlaceHLAPI>();
			this.startPlaces.add(start);
			this.endPlaces = new Vector<PlaceHLAPI>();			
			this.endPlaces.add(end);
		}
				
		public NodePair (List<PlaceHLAPI> start, List<PlaceHLAPI> end){
			this.startPlaces = start;
			this.endPlaces = end;
		}
		

		public boolean removeStart (PlaceHLAPI node){
			return(this.startPlaces.remove(node));
		}
		
		public boolean removeEnd (PlaceHLAPI node){
			return(this.endPlaces.remove(node));
		}
		
		public List<PlaceHLAPI> getStart(){
			return this.startPlaces;
		}
		
		public List<PlaceHLAPI> getEnd(){
			return this.endPlaces;
		}
		
		public void addStart(PlaceHLAPI start){
			this.startPlaces.add(start);
		}
		
		public void addEnd (PlaceHLAPI end){
			this.endPlaces.add(end);
		}

		public void setStartPlaces(List<PlaceHLAPI> startPlaces) {
			this.startPlaces = startPlaces;
		}

		public void setEndPlaces(List<PlaceHLAPI> endPlaces) {
			this.endPlaces = endPlaces;
		}
	}
}
