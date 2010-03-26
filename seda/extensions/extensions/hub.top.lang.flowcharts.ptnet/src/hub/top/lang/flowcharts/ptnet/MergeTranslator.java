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
import hub.top.lang.flowcharts.MergeNode;

public class MergeTranslator extends Translator {
	
	public MergeNode getNode(){
		if (!(super.getNode() instanceof MergeNode))
			throw new NullPointerException("Error. Instantiated MergeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((MergeNode)super.getNode());
	}
	
	public void translate(){
		MergeNode node = this.getNode();
		
		// translate EndNode first
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		pE.setName("end" + super.createLabel(node.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		List<PlaceExt> starts = new Vector<PlaceExt>();
		List<PlaceExt> end = new Vector<PlaceExt>();
		Hashtable<PlaceExt, DiagramNode> otherEnds = new Hashtable<PlaceExt, DiagramNode>();
		end.add(pE);
			
		// add a startPlace and a Transition with Arc to the EndPlace for each incoming Edge
		for (DiagramArc arc: node.getIncoming()){
			PlaceExt pIn = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
			if (node.getLabel() != null && arc.getSrc().getLabel() != null)
				pIn.setName(super.createLabel(node.getLabel()+"_start_"+arc.getSrc().getLabel(), 'p'));
			else pIn.setName(super.createLabel(null, 'p'));
			this.net.getPlaces().add(pIn);
			
			TransitionExt t = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
			t.setName(super.createLabel(arc.getLabel(), 't'));
			this.net.getTransitions().add(t);
			
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pIn); a1.setTarget(t);
			this.net.getArcs().add(a1);

			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t); a2.setTarget(pE);
			this.net.getArcs().add(a2);

			starts.add(pIn);
			otherEnds.put(pIn, arc.getSrc());
		}
			
		this.setPair(new NodePair(starts, end));
		this.getPair().setOtherEnds(otherEnds);
		if (node.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (node.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, node.getOutgoing().get(0).getTarget());
		super.getTable().put(node, this.getPair());
			
	}
}
