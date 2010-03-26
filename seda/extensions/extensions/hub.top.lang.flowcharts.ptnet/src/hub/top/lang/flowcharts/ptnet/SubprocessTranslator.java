package hub.top.lang.flowcharts.ptnet;


import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
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

import java.util.Hashtable;

public class SubprocessTranslator extends Translator {
	
	// remember the resource nodes by name for later reuse
	Hashtable<String, NodePair> translatedResources;
	
	public Hashtable<String, NodePair> getTranslatedResources() {
		return translatedResources;
	}

	public void setTranslatedResources(
			Hashtable<String, NodePair> translatedResources) {
		this.translatedResources = translatedResources;
	}
	
	public Subprocess getNode(){
		if (!(super.getNode() instanceof Subprocess))
			throw new NullPointerException("Error. Instantiated SubprocessTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Subprocess)super.getNode());
	}
	
	public void translate() {
		Subprocess sub = this.getNode();
		
		// Start
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(sub.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		
		// End
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName("end_" + super.createLabel(sub.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		//WorkflowDiagram subProc = sub.getSubprocess();
		
		// weighted subprocess --> subprocess will be executed several times parallel
		if (sub.getWeight() > 1){
			// a transition to synchronize the begin of all instances
			TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t1.setName("weightedSub_" + super.createLabel(sub.getLabel(), 't'));
			this.getNet().getTransitions().add(t1);
			
			ArcToTransitionExt aS = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			aS.setSource(pS); aS.setTarget(t1);
			this.getNet().getArcs().add(aS);
			
			// a transition to synchronize the end of all instances
			TransitionExt t2 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t2.setName("weightedSub_end_" + super.createLabel(null, 't'));
			this.getNet().getTransitions().add(t2);
			
			ArcToPlaceExt aE = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			aE.setSource(t2); aE.setTarget(pE);
			this.getNet().getArcs().add(aE);
			
			for (int i=1; i<=sub.getWeight(); i++){
				// a startplace for each instance
				PlaceExt p1 = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
				p1.setName("inst"+ i + "_" + super.createLabel(null, 'p'));
				this.getNet().getPlaces().add(p1);
				
				ArcToPlaceExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
				a1.setSource(t1); a1.setTarget(p1);
				this.getNet().getArcs().add(a1);
				
				// an endplace for each instance
				PlaceExt p2 = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
				p2.setName("inst"+ i + "_" + super.createLabel(null, 'p'));
				this.getNet().getPlaces().add(p2);
				
				ArcToTransitionExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
				a2.setSource(p2); a2.setTarget(t2);
				this.getNet().getArcs().add(a2);
				
				this.createProcess(p1, p2);
			}
		}
		else this.createProcess(pS, pE);		
		
		this.setPair(new NodePair(pS, pE));		
			
		// more than one incoming arc?
		if (sub.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (sub.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, sub.getIncoming().get(0).getSrc());
			
		// more than one outgoing arc?
		if (sub.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (sub.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, sub.getOutgoing().get(0).getTarget());
			
		super.getTable().put(sub, this.getPair());
		
	}
	
	private void createProcess(PlaceExt pS, PlaceExt pE){
		
		Subprocess sub = this.getNode();
		WorkflowDiagram subProc = sub.getSubprocess();
		// no subprocess or subprocess empty --> insert dummy
		if (subProc == null || subProc.getDiagramNodes().isEmpty()){
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			t.setName(super.createLabel(sub.getLabel(), 't'));
			
			if (sub.isOptional()) t.setProbability(sub.getProbability());
			else t.setProbability(1);
			
			t.setCost(sub.getCost());
			t.setMinTime(sub.getMinTime());
			t.setMaxTime(sub.getMaxTime());
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pS); a1.setTarget(t);
			this.net.getArcs().add(a1);
			
			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);
		}
		// translate subprocess
		else {
			// Start
			TransitionExt tS = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			tS.setName("start_" + super.createLabel(subProc.getName(), 't'));
			if (sub.isOptional()) tS.setProbability(sub.getProbability());
			
			// cost and time
			int c = 0;
			int mint = 0;
			int maxt = 0;
			for (DiagramNode a : subProc.getDiagramNodes()){
				if (a instanceof ActivityNode){
					c += ((ActivityNode) a).getCost();
					mint += ((ActivityNode) a).getMinTime();
					maxt += ((ActivityNode) a).getMaxTime();
				}
			}
			this.net.getTransitions().add(tS);
			
			ArcToTransitionExt aS = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			aS.setSource(pS); aS.setTarget(tS);
			this.net.getArcs().add(aS);
			
			// End with restcost and -time
			TransitionExt tE = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); 
			tE.setName("end_" + super.createLabel(subProc.getName(), 't'));
			c = sub.getCost()-c;
			if (c < 0) c = 0;
			mint = sub.getMinTime()-mint;
			if (mint < 0) mint = 0;
			maxt = sub.getMaxTime()-maxt;
			if (maxt < 0) maxt = 0;
			tE.setCost(c);
			tE.setMinTime(mint);
			tE.setMaxTime(maxt);
			this.net.getTransitions().add(tE);
			
			ArcToPlaceExt aE = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			aE.setSource(tE); aE.setTarget(pE);
			this.net.getArcs().add(aE);
			
			for (DiagramNode node : subProc.getDiagramNodes()){
				Translator trans;
	  			
	  			if (node instanceof StartNode)				trans = new StartNodeTranslator();
	  			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
	  			else if (node instanceof SimpleActivity)	trans = new SimpleActTranslator();
	  			else if (node instanceof Event) 			trans = new EventTranslator();
	  			else if (node instanceof ResourceNode) 		trans = new ResourceTranslator();
	  			else if (node instanceof SplitNode)			trans = new SplitTranslator();
	  			else if (node instanceof MergeNode)			trans = new MergeTranslator();
	  			else if (node instanceof Subprocess)		trans = new SubprocessTranslator();
	  			else if (node instanceof Flow)				trans = new FlowTranslator();
	  			else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");
	  			
	  			trans.setNet(this.getNet());
	  			trans.setNode(node);
	  			trans.setTable(this.getTable());
	  			trans.setPct(this.getPct());
	  			trans.setTct(this.getTct());
	  			if (trans instanceof ResourceTranslator) ((ResourceTranslator) trans).setTranslatedResources(this.translatedResources);	  			
	  			if (trans instanceof StartNodeTranslator) ((StartNodeTranslator) trans).translate(false);
	  			else trans.translate();
	  			this.setTable(trans.getTable());
	  			this.setNet(trans.getNet());
	  			this.setPct(trans.getPct());
	  			this.setTct(trans.getTct());
	  			if (trans instanceof ResourceTranslator) this.setTranslatedResources(((ResourceTranslator) trans).getTranslatedResources());
	  			
	  			if (node instanceof ActivityNode && ((ActivityNode) node).isOptional()) this.insertOpt((ActivityNode) node);
	  			
	  			// insert arcs where there are no arcs in the diagram
	  				// between start of complex activity and startnode(s)
	  			if (node instanceof StartNode && node.getIncoming().isEmpty()){
	  				ArcToPlaceExt a3 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
	  				a3.setSource(tS); a3.setTarget(this.table.get(node).getStart().get(0));
	  				this.net.getArcs().add(a3);
	  			}
	  			
	  				// between endnode(s) and end of flow
	  			if (node instanceof Endnode && node.getOutgoing().isEmpty()){
	  				ArcToTransitionExt a4 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
	  				a4.setSource(this.table.get(node).getEnd().get(0)); a4.setTarget(tE);
	  				this.net.getArcs().add(a4);
	  			}
			}
			
			// translate all arcs of Subprocess
	  		for (DiagramArc arc : subProc.getDiagramArcs()) {
	  			DiagramNode src = arc.getSrc();
	  			DiagramNode tgt = arc.getTarget();
	  			
	  			if (src == null || tgt == null) throw new NullPointerException("Error in PageTranslator while translating arc: arc has no source- or targetNode in Diagram!");
	  			  			
	  			PlaceExt rightArcStart;
	  			PlaceExt rightArcEnd;
	  			
	  			// Arc from ResourceNode
	  			if (src instanceof ResourceNode) {
	  				// Resources are always only one Place --> ArcStart is the resourcePlace
	  				rightArcStart = this.getTable().get(src).getEnd().get(0);
	  				// ArcEnd is the Startplace of the other Node which is marked as other Startplace of the resource
	  				rightArcEnd = this.getTable().get(tgt).getOtherEndPlace(src, true);
	  			}
	  			// Arc to ResourceNode
	  			else if (tgt instanceof ResourceNode) {
	  				// ArcStart is the EndPlace which is marked as other Endplace of the resource 
	  				rightArcStart = this.getTable().get(src).getOtherEndPlace(tgt, false);
	  				// ArcEnd is the resourcePlace
	  				rightArcEnd = this.getTable().get(tgt).getStart().get(0);
	  				
	  			// Arc with weight-Intervall
	  				if (arc.getMaxWeight() != arc.getMinWeight()) {
	  					double prob = 1/(arc.getMaxWeight() - arc.getMinWeight());
	  					for (int i = arc.getMinWeight(); i <= arc.getMaxWeight(); i++){
	  						TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
	  						t.setName(super.createLabel("weightedArc" + i, 't'));
	  						this.getNet().getTransitions().add(t);
	  						
	  						ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
	  						a1.setSource(rightArcStart); a1.setTarget(t);
	  						a1.setProbability(prob);
	  						this.getNet().getArcs().add(a1);
	  						
	  						ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
	  						a2.setSource(t); a2.setTarget(rightArcEnd);
	  						a2.setWeight(i);
	  						this.getNet().getArcs().add(a2);
	  						
	  					}
	  					break;
	  				}
	  			}
	  			// other Arc
	  			else {
	  				if (this.table.get(tgt) == null) break;
	  				else {
	  					rightArcEnd = this.table.get(tgt).getOtherEndPlace(src, true);
	  					// if there is no entry in the Hashtable (@see Translator.NodePair.otherEnds), there can be only one Endplace
	  					if (rightArcEnd == null) rightArcEnd = this.table.get(src).getEnd().get(0);  	  			
	  				}
	  			
	  				if (this.table.get(src) == null) break;
	  				else {
	  					rightArcStart = this.table.get(src).getOtherEndPlace(tgt, false);
	  					if (rightArcStart == null) rightArcStart = this.table.get(tgt).getStart().get(0);
	  				}
	  			}
	  			if (rightArcStart == null || rightArcEnd == null){
	  				throw new NullPointerException("Error in SubprocessTranslator while translating arc: arc has no source- or targetPlace!");
	  			}
	  			
	  			TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
	  			if (arc.getSrc().getLabel() != null && arc.getTarget().getLabel() != null)
	  				t1.setName(super.createLabel(arc.getSrc().getLabel()+"To"+arc.getTarget().getLabel(), 't'));
	  			else t1.setName(super.createLabel(null, 't'));
	  			this.net.getTransitions().add(t1);
	  			
	  			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
	  			a1.setSource(rightArcStart); a1.setTarget(t1);
	  			this.net.getArcs().add(a1);
	  			
	  			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
	  			a2.setSource(t1); a2.setTarget(rightArcEnd);
	  			this.net.getArcs().add(a2);
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
			//				first Startplace	outgoing arc		  target = transition outgoing arc target = Startplace
			pS = (PlaceExt) np.getStart().get(0).getOutgoing().get(0).getTarget().getOutgoing().get(0).getTarget();
		}
		else pS = np.getStart().get(0);
		
		if (np.getEnd().size() > 1){
			//			first Endplace	incoming arc			source = transition incoming arc	source = Endplace
			pE = (PlaceExt) np.getEnd().get(0).getIncoming().get(0).getSource().getIncoming().get(0).getSource();
		}
		else pE = np.getEnd().get(0);
		
		TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t.setName(super.createLabel("no_execution", 't'));
		// set probability of the "no execution" path
		t.setProbability(1-n.getProbability());
		this.net.getTransitions().add(t);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(t); 
		this.net.getArcs().add(a1);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(t); a2.setTarget(pE);
		this.net.getArcs().add(a2);
	}
}
