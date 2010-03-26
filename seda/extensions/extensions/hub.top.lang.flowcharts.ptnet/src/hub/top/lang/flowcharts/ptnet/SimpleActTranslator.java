package hub.top.lang.flowcharts.ptnet;


import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.SimpleActivity;

public class SimpleActTranslator extends Translator {

	
	
	public SimpleActivity getNode(){
		if (!(super.getNode() instanceof SimpleActivity))
			throw new NullPointerException("Error. Instantiated SimpleActTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SimpleActivity)super.getNode());
	}
	
	
	public void translate() {
		SimpleActivity act = this.getNode();

			PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName("start_" + super.createLabel(act.getLabel(), 'p'));
			this.net.getPlaces().add(pS);
			
			TransitionExt t0 = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t0.setName(super.createLabel(act.getLabel(), 't'));
			if (act.isOptional()) t0.setProbability(act.getProbability());
			else t0.setProbability(1);
			t0.setCost(act.getCost());
			t0.setMinTime(act.getMinTime());
			t0.setMaxTime(act.getMaxTime());
			this.net.getTransitions().add(t0);
				
			PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName("end_" + super.createLabel(act.getLabel(), 'p'));
			this.net.getPlaces().add(pE);
				
			ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
			a1.setSource(pS); a1.setTarget(t0);
			this.net.getArcs().add(a1);
			
			ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
			a2.setSource(t0); a2.setTarget(pE);
			this.net.getArcs().add(a2);
			
			
			this.setPair(new NodePair(pS, pE));
			
			if (act.getIncoming().size() > 1){
				this.getPair().removeStart(pS);
				super.addInterfaceIn(pS);
			}
			else if (act.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, act.getIncoming().get(0).getSrc());
				
			if (act.getOutgoing().size() > 1){
				this.getPair().removeEnd(pE);
				super.addInterfaceOut(pE);
			}
			else if (act.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, act.getOutgoing().get(0).getTarget());
				
			super.getTable().put(act, this.getPair());
	}

}
