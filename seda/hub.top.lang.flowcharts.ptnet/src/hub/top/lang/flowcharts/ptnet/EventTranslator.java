package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.flowcharts.Event;

public class EventTranslator extends Translator {

	public Event getNode(){
		if (!(super.getNode() instanceof Event))
			throw new NullPointerException("Error. Instantiated EventTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Event)super.getNode());
	}
	
	public void translate() {
		Event ev = this.getNode();
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); 
		
		pS.setName("start_" + super.createLabel(ev.getLabel(), 'p'));
				
		TransitionExt t0 = PtnetLoLAFactory.eINSTANCE.createTransitionExt(); t0.setName(super.createLabel(ev.getLabel(), 't'));
		if (ev.isOptional()) t0.setProbability(ev.getProbability());
		else t0.setProbability(1);
		t0.setMinTime(ev.getMinTime());
		t0.setMaxTime(ev.getMaxTime());
		t0.setCost(ev.getCost());
		
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName("end_" + super.createLabel(ev.getLabel(), 'p'));
		
		this.pair = new NodePair(pS, pE);
		
		this.net.getPlaces().add(pS);
		this.net.getTransitions().add(t0);
		this.net.getPlaces().add(pE);
		// an event is always activated 
		pS.setToken(1);
		
		ArcToTransitionExt a1 = PtnetLoLAFactory.eINSTANCE.createArcToTransitionExt();
		a1.setSource(pS); a1.setTarget(t0);
		this.net.getArcs().add(a1);
		
		ArcToPlaceExt a2 = PtnetLoLAFactory.eINSTANCE.createArcToPlaceExt();
		a2.setSource(t0); a2.setTarget(pE);
		this.net.getArcs().add(a2);
		
		// more than one outgoing arc --> insert an InterfaceOut (an Event has no incoming arcs, so no InterfaceIn is needed)
		if (ev.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			addInterfaceOut(pE);
		}
		else if (ev.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, ev.getOutgoing().get(0).getTarget());
			
		super.getTable().put(ev, this.getPair());
	}
}
