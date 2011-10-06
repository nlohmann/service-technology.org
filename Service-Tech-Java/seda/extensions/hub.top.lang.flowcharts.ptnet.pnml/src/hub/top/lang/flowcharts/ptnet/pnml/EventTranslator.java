package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.Event;

public class EventTranslator extends Translator {

	public Event getNode(){
		if (!(super.getNode() instanceof Event))
			throw new NullPointerException("Error. Instantiated EventTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Event)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		Event ev = this.getNode();
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(ev.getLabel()+"_start", 'p')));
		this.transCount++;
		TransitionHLAPI t0 = new TransitionHLAPI("t"+this.getTransCount()); t0.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(ev.getLabel(), 't')));
		this.placeCount++;
		PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(ev.getLabel()+"_end", 'p')));
		
		this.pair = new NodePair(pS, pE);
		
		pS.setContainerPageHLAPI(page);
		t0.setContainerPageHLAPI(page);
		pE.setContainerPageHLAPI(page);
		
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), pS, t0, page).setNameHLAPI(new NameHLAPI(""));
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), t0, pE, page).setNameHLAPI(new NameHLAPI(""));
			
		// more than one outgoing arc --> insert an InterfaceOut (an Event has no incoming arcs, so no InterfaceIn is needed)
		if (ev.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			addInterfaceOut(pE);
		}
		else if (ev.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, ev.getOutgoing().get(0).getTarget());
			
		super.getTable().put(ev, this.getPair());
	}
}
