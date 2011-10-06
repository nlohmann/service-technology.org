package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.RefPlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.SimpleActivity;
import hub.top.lang.flowcharts.SplitNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.WorkflowDiagram;

/**
 * translates a whole diagram-page into PNML 
 * @author Arwen
 *
 */
public class PageTranslator extends Translator {

	WorkflowDiagram diagram;
	// if sub is true, then this is a subprocess so start- and endNode are given in form of ReferenceNodes
	boolean sub = false;  
	// the calling SubprocessTranslatorClass
	SubprocessTranslator calling = null;
	
	public void setCalling (SubprocessTranslator c){
		this.calling = c;
	}
	
	public void setDiagram(WorkflowDiagram diagram) {
		this.diagram = diagram;
	}

	public WorkflowDiagram getDiagram(){
		return(this.diagram);
	}
	
	/**
	 * translate the diagramPage
	 * @throws VoidRepositoryException 
	 * @throws InvalidIDException 
	 */
	public void translate() throws InvalidIDException, VoidRepositoryException{
		
		WorkflowDiagram d = this.getDiagram();
		if (this.page == null) return;
		
		// diagrampage is not existent -> insert dummy-net
		if (d == null && this.sub && this.calling != null){
			this.transCount++;
			TransitionHLAPI t0 = new TransitionHLAPI("t"+this.getTransCount()); t0.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(this.calling.node.getLabel(), 't')));
			t0.setContainerPageHLAPI(page);
				
			for (RefPlaceHLAPI r : this.calling.refStarts){
				this.arcCount++;
				new ArcHLAPI("a"+this.getArcCount(), r, t0, page);
			}
			
			for (RefPlaceHLAPI r : this.calling.refEnds){
				this.arcCount++;
				new ArcHLAPI("a"+this.getArcCount(), t0, r, page);
			}
			return;
		}
		
		// translate all nodes of wfDiagram
  		for (DiagramNode node : d.getDiagramNodes()) {
  			Translator trans;
  			
  			// StartNode
  			if (node instanceof StartNode){
  				// translate only if this is not a subprocess
  				if (!this.sub) trans = new StartNodeTranslator();
  				// if this is a subprocess then do nothing with this StartNode, because it is already translated ( == referenceNode)
  				else break;
  			}
  			// EndNode
  			else if (node instanceof Endnode) {
  				// translate only if this is not a subprocess
  				if (!this.sub) trans = new EndNodeTranslator();
  				// if this is a subprocess then do nothing with this EndNode, because it is already translated ( == referenceNode)
  				else break;
  			}
  			// something else
  			else if (node instanceof SimpleActivity)	trans = new SimpleActTranslator();
  			else if (node instanceof Event) 			trans = new EventTranslator();
  			else if (node instanceof ResourceNode) 		trans = new ResourceTranslator();
  			else if (node instanceof SplitNode)			trans = new SplitTranslator();
  			else if (node instanceof MergeNode)			trans = new MergeTranslator();
  			else if (node instanceof Subprocess)		trans = new SubprocessTranslator();
  			else if (node instanceof Flow)				trans = new FlowTranslator();
  			else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");
  			
  			trans.setPage(page);
  			trans.setNode(node);
  			trans.setTable(this.table);
  			trans.setCounts(this.placeCount, this.transCount, this.arcCount);
  			trans.translate();
  			this.setPlaceCount(trans.getPlaceCount());
  			this.setTransCount(trans.getTransCount());
  			this.setArcCount(trans.getArcCount());
  			this.setTable(trans.getTable());
  			
  			if (node instanceof ActivityNode && ((ActivityNode) node).isOptional()) this.insertOpt((ActivityNode) node);	
  		}
  		
  		// translate all arcs of wfDiagram
  		for (DiagramArc arc : d.getDiagramArcs()) {
  			DiagramNode src = arc.getSrc();
  			DiagramNode tgt = arc.getTarget();
  			
  			if (src == null || tgt == null) throw new NullPointerException("Error in PageTranslator while translating arc: arc has no source- or targetNode in Diagram!");
  			  			
  			PlaceHLAPI rightArcStart;
  			PlaceHLAPI rightArcEnd;
  			
  			if (this.table.get(tgt) == null) break;
  			else {
  				rightArcEnd = this.table.get(tgt).getOtherEndPlace(src);
  				// if there is no entry in the Hashtable (@see Translator.NodePair.otherEnds), there can be only one Endplace
  	  			if (rightArcEnd == null) rightArcEnd = this.table.get(src).getEnd().get(0);  	  			
  			}
  			
  			if (this.table.get(src) == null) break;
			else {
				rightArcStart = this.table.get(src).getOtherEndPlace(tgt);
				if (rightArcStart == null) rightArcStart = this.table.get(tgt).getStart().get(0);
			}
  			
  			if (rightArcStart == null || rightArcEnd == null){
  				throw new NullPointerException("Error in PageTranslator while translating arc: arc has no source- or targetPlace!");
  			}
  			
  			this.transCount++;
  			TransitionHLAPI t1 = new TransitionHLAPI("t"+this.getTransCount()); 
  			t1.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(arc.getSrc().getLabel()+"To"+arc.getTarget().getLabel(), 't')));
  			t1.setContainerPageHLAPI(page);
  			
  			// if this is a subnet, the arcs from start and to endnode must go to the referenceplaces
  			if (this.sub && (src instanceof StartNode || tgt instanceof Endnode)){
  				if (src instanceof StartNode){
  					RefPlaceHLAPI arcStart = rightArcStart.getReferencingPlacesHLAPI().get(0);
  					this.arcCount++;
  		  			new ArcHLAPI("a"+this.getArcCount(), arcStart, t1, page);
  				}
  				if (tgt instanceof Endnode){
  					RefPlaceHLAPI arcEnd = rightArcEnd.getReferencingPlacesHLAPI().get(0);
  					this.arcCount++;
  		  			new ArcHLAPI("a"+this.getArcCount(), t1, arcEnd, page);
  				}
  			}
  			else {
  				this.arcCount++;
  				new ArcHLAPI("a"+this.getArcCount(), rightArcStart, t1, page);
  				this.arcCount++;
  				new ArcHLAPI("a"+this.getArcCount(), t1, rightArcEnd, page);
  			}
  		}
  	}
	
	/**
	 * inserts a path where the activity n is not executed 
	 * @param n an optional ActivityNode
	 * @throws VoidRepositoryException 
	 * @throws InvalidIDException 
	 */
	private void insertOpt(ActivityNode n) throws InvalidIDException, VoidRepositoryException{
		NodePair np = this.table.get(n);
		PlaceHLAPI pS;
		PlaceHLAPI pE;
		
		// if there are more than one Startplaces for n get the place after the interfaceIn
		if (np.getStart().size() > 1){
			//					first Startplace	outgoing arc			target = transition outgoing arc		target = Startplace
			pS = (PlaceHLAPI) np.getStart().get(0).getOutArcsHLAPI().get(0).getTargetHLAPI().getOutArcsHLAPI().get(0).getTargetHLAPI();
		}
		else pS = np.getStart().get(0);
		
		if (np.getEnd().size() > 1){
			//			first Endplace	incoming arc			source = transition 	incoming arc		source = Endplace
			pE = (PlaceHLAPI) np.getEnd().get(0).getInArcsHLAPI().get(0).getSourceHLAPI().getInArcsHLAPI().get(0).getSourceHLAPI();
		}
		else pE = np.getEnd().get(0);
		
		this.transCount++;
		TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount()); t.setNameHLAPI(new NameHLAPI(super.createPNMLLabel("don't execute", 't')));
		t.setContainerPageHLAPI(page);
		
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), pS, t, page);
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), t, pE, page);
	}
}
