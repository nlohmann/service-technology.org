package hub.top.lang.flowcharts.ptnet;

import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.SplitNode;

public class SplitTranslator extends Translator {

	public SplitNode getNode(){
		if (!(super.getNode() instanceof SplitNode))
			throw new NullPointerException("Error. Instantiated SplitTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SplitNode)super.getNode());
	}
	
	public void translate() {
		SplitNode node = this.getNode();
		
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(node.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		List<PlaceExt> start = new Vector<PlaceExt>();
		List<PlaceExt> ends = new Vector<PlaceExt>();
		Hashtable<PlaceExt, DiagramNode> otherEnds = new Hashtable<PlaceExt, DiagramNode>();
		start.add(pS);
		
		// add a transition and an EndPlace for every outgoing Edge
		for(DiagramArc arc: node.getOutgoing()){
			
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t.setName(super.createLabel(arc.getLabel(), 't'));
			
			double p = 0;	// probability of path (if not set, all paths have the same probability)
			if (arc.getProbability() != 1) p = arc.getProbability();
			else p = 1 / node.getOutgoing().size();
			t.setProbability(p);
			
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a.setSource(pS); a.setTarget(t);
			this.net.getArcs().add(a);
			
			PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
			pE.setName("to_" + super.createLabel(arc.getTarget().getLabel(), 'p'));
			this.net.getPlaces().add(pE);

			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);
			
			ends.add(pE);
			otherEnds.put(pE, arc.getTarget());
		}
		
		this.setPair(new NodePair(start, ends));
		this.getPair().setOtherEnds(otherEnds);
			
		if (node.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (node.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, node.getIncoming().get(0).getSrc());
			
		super.getTable().put(node, this.getPair());
	}
}
