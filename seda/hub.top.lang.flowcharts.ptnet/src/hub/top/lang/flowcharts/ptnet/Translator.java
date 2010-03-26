package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;

import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

public abstract class Translator {
	DiagramNode node;
	NodePair pair;
	PtNet net;
	Hashtable<DiagramNode, NodePair> table;
	Integer pct;
	Integer tct;
	
	public Integer getPct() {
		return pct;
	}

	public void setPct(Integer pct) {
		this.pct = pct;
	}

	public Integer getTct() {
		return tct;
	}

	public void setTct(Integer tct) {
		this.tct = tct;
	}

	public NodePair getPair() {
		return pair;
	}

	public PtNet getNet() {
		return net;
	}

	public void setNet(PtNet net) {
		this.net = net;
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
	
	public void translate() {	}
	
	/**
	 * creates an XML-conform label from a String 
	 * replaces therefore special characters by their HTML notation
	 * if label is empty there will be created an alternative name
	 * @param label - the diagramNode's label, x - 't' for transitions, 'p' for places
	 * @return the PNML Label
	 */
	protected String createLabel (String label, Character x){
		if (label == null || label.length() == 0){
			if (x == 'p') {
				label = "p"+this.getPct();
				this.setPct(this.getPct()+1);
			}
			else if (x == 't') {
				label = "t" + this.getTct();
				this.setTct(this.getTct()+1);
			}
			return label;
		}
		
		label = label.replaceAll("&", "und");
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
	protected void addInterfaceIn(PlaceExt pStart){
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();	t.setName("InterfaceIn_" + createLabel(null, 't'));
		this.net.getTransitions().add(t);
		
		ArcToPlaceExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a1.setSource(t); a1.setTarget(pStart);
		this.net.getArcs().add(a1);
		
		for (DiagramArc arc: this.node.getIncoming()){
			PlaceExt pIn = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getSrc().getLabel() != null)
				pIn.setName(node.getLabel()+"_In_"+arc.getSrc().getLabel());
			else pIn.setName(createLabel(null, 'p'));
			this.net.getPlaces().add(pIn);	
			
			ArcToTransitionExt a = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a.setSource(pIn); a.setTarget(t);
			this.net.getArcs().add(a);
			
			this.pair.addStart(pIn);				
			this.pair.setOtherEnd(pIn, arc.getSrc());
		}
	}
	
	/**
	 * nodes that have more than one outgoing arc need an InterfaceOut
	 * @param p - the actual endPlace
	 */
	protected void addInterfaceOut(PlaceExt pEnd){
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();	t.setName("InterfaceOut_" + createLabel(null, 't'));
		this.net.getTransitions().add(t);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pEnd); a1.setTarget(t);
		this.net.getArcs().add(a1);
			
		for (DiagramArc arc: this.node.getOutgoing()){
			PlaceExt pOut = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getTarget().getLabel() != null)
				pOut.setName(this.createLabel(node.getLabel()+"_Out_"+arc.getTarget().getLabel(), 'p'));
			else pOut.setName(createLabel(null, 'p'));
			this.net.getPlaces().add(pOut);
			
			ArcToPlaceExt a = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a.setSource(t); a.setTarget(pOut);
			this.net.getArcs().add(a);
			
			this.pair.addEnd(pOut);
			this.pair.setOtherEnd(pOut, arc.getTarget());
		}
	}
	
	protected class NodePair{
		List<PlaceExt> startPlaces;
		List<PlaceExt> endPlaces;
		// for each Node the other end of the according arc
		Hashtable<PlaceExt, DiagramNode> otherEnds;
		
		/**
		 * 
		 * @return the Hashtable including all Places and the DiagramNodes at the other ends of the according arcs 
		 */
		public Hashtable<PlaceExt, DiagramNode> getOtherEnds() {
			return otherEnds;
		}

		public void setOtherEnds(Hashtable<PlaceExt, DiagramNode> otherEnds) {
			this.otherEnds = otherEnds;
		}

		/**
		 * set the other end of an arc according to a place in the Pair
		 * @param place - the place
		 * @param end - the other end of the according arc
		 */
		public void setOtherEnd (PlaceExt place, DiagramNode end){
			if (!(this.startPlaces.contains(place) || this.endPlaces.contains(place))){
				throw new NullPointerException("Error in NodePair.setOtherEnd()! Trying to add a place which is not in lists (startPlaces, endPlaces).");
			}
			else 
				if (this.otherEnds == null) {
					this.otherEnds = new Hashtable<PlaceExt, DiagramNode>();
				}
				this.otherEnds.put(place, end);
		}
		
		/**
		 * @param end - the source or target of the arc
		 * @param start - true, if searching for a startPlace, false, if searching for an endPlace
		 * @return the place according to the arc which source or target is given
		 */
		public PlaceExt getOtherEndPlace (DiagramNode end, Boolean start){
			if (this.getOtherEnds() == null){
				return null;
			}
			if (!this.getOtherEnds().containsValue(end)){
				System.out.println("NodePair.getOtherEndPlace()! The " + end.getClass().getSimpleName() + " " + end.getLabel() + " is not in the List.");

				return null;
			}
			else 
			{
				for (PlaceExt p : this.getOtherEnds().keySet()){
					if (this.getOtherEnds().get(p) == end) {
						if ((start && this.startPlaces.contains(p)) || (!start && this.endPlaces.contains(p))) return p;
					}
				}
			}
			return null;
		}
		
		public NodePair(PlaceExt start, PlaceExt end){
			this.startPlaces = new Vector<PlaceExt>();
			this.startPlaces.add(start);
			this.endPlaces = new Vector<PlaceExt>();			
			this.endPlaces.add(end);
		}
				
		public NodePair (List<PlaceExt> start, List<PlaceExt> end){
			this.startPlaces = start;
			this.endPlaces = end;
		}
		

		public boolean removeStart (PlaceExt node){
			return(this.startPlaces.remove(node));
		}
		
		public boolean removeEnd (PlaceExt node){
			return(this.endPlaces.remove(node));
		}
		
		public List<PlaceExt> getStart(){
			return this.startPlaces;
		}
		
		public List<PlaceExt> getEnd(){
			return this.endPlaces;
		}
		
		public void addStart(PlaceExt start){
			this.startPlaces.add(start);
		}
		
		public void addEnd (PlaceExt end){
			this.endPlaces.add(end);
		}

		public void setStartPlaces(List<PlaceExt> startPlaces) {
			this.startPlaces = startPlaces;
		}

		public void setEndPlaces(List<PlaceExt> endPlaces) {
			this.endPlaces = endPlaces;
		}
	}
}
