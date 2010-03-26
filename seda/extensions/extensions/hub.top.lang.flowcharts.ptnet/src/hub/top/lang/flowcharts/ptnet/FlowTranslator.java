package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.ActivityNode;
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

public class FlowTranslator extends Translator {

	public Flow getNode(){
		if (!(super.getNode() instanceof Flow))
			throw new NullPointerException("Error. Instantiated FlowTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Flow)super.getNode());
	}
	
	public void translate() {
		Flow fl = this.getNode();
		// startplace of the Flow
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pS.setName("start_" + super.createLabel(fl.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		
		TransitionExt tS = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
		tS.setName("synchronizeStart_" + super.createLabel(fl.getLabel(), 't'));
		this.net.getTransitions().add(tS);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(tS);
		this.net.getArcs().add(a1);
		
		// endplace of the Flow
		TransitionExt tE = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
		tE.setName("synchronizeEnd_" + super.createLabel(fl.getLabel(), 't'));
		this.net.getTransitions().add(tE);
		
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pE.setName("end" + super.createLabel(fl.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(tE); a2.setTarget(pE);
		this.net.getArcs().add(a2);
		
		this.setPair(new NodePair(pS, pE));
			
		if (fl.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (fl.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, fl.getIncoming().get(0).getSrc());
			
		if (fl.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (fl.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, fl.getOutgoing().get(0).getTarget());
			
		super.getTable().put(fl, this.getPair() );
			
		// translate the Flow as if it was a page
		// translate all nodes
  		for (DiagramNode node : fl.getChildNodes()){
  			Translator trans;

			if (node instanceof StartNode) 				trans = new StartNodeTranslator();
			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
  			else if (node instanceof SimpleActivity)	trans = new SimpleActTranslator();
  			else if (node instanceof Event) 			trans = new EventTranslator();
  			else if (node instanceof ResourceNode) 		trans = new ResourceTranslator();
  			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
  			else if (node instanceof SplitNode)			trans = new SplitTranslator();
  			else if (node instanceof MergeNode)			trans = new MergeTranslator();
  			else if (node instanceof Subprocess)		trans = new SubprocessTranslator();
  			else if (node instanceof Flow)				trans = new FlowTranslator();
  			else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");
  			
  			trans.setNet(this.getNet());
  			trans.setNode(node);
  			trans.setTable(this.table);
  			if (trans instanceof StartNodeTranslator) ((StartNodeTranslator) trans).translate(false);
  			else trans.translate();
  			this.setNet(trans.getNet());
  			this.setTable(trans.getTable());
  			
  			if (node instanceof ActivityNode && ((ActivityNode) node).isOptional()) this.insertOpt((ActivityNode) node);
  			
  			// insert arcs where no arcs are in the diagram 
  				//between begin of flow and startnodes
  			if (node instanceof StartNode){	// && node.getIncoming().isEmpty()
  				ArcToPlaceExt a3 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
  				a3.setSource(tS); a3.setTarget(this.table.get(node).getStart().get(0));
  				this.net.getArcs().add(a3);
  				
  			}
  				// between endnodes and end of flow
  			if (node instanceof Endnode){	// && node.getOutgoing().isEmpty()
  				ArcToTransitionExt a4 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
  				a4.setSource(this.table.get(node).getEnd().get(0)); a4.setTarget(tE);
  				this.net.getArcs().add(a4);
  				
  			}
  		}
  		
	}
	
	/**
	 * inserts a path where the activity n is not executed 
	 * @param n an optional ActivityNode
	 * @throws VoidRepositoryException 
	 * @throws InvalidIDException 
	 */
	private void insertOpt(ActivityNode n) {
		NodePair np = this.table.get(n);
		PlaceExt pS;
		PlaceExt pE;
		
		// if there are more than one Startplaces for n get the place after the interfaceIn
		if (np.getStart().size() > 1){
			//				first Startplace	 outgoing arc  target = transition  outgoing arc	   target = Startplace
			pS = (PlaceExt) np.getStart().get(0).getOutgoing().get(0).getTarget().getOutgoing().get(0).getTarget();
		}
		else pS = np.getStart().get(0);
		
		if (np.getEnd().size() > 1){
			//			first Endplace	       incoming arc	 source = transition incoming arc		source = Endplace
			pE = (PlaceExt) np.getEnd().get(0).getIncoming().get(0).getSource().getIncoming().get(0).getSource();
		}
		else pE = np.getEnd().get(0);
		
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t.setName(super.createLabel("no_execution", 't'));
		this.net.getTransitions().add(t);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(t);
		t.setProbability(1-n.getProbability());
		this.getNet().getArcs().add(a1);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(t); a2.setTarget(pE);
		this.getNet().getArcs().add(a2);
	}
}
