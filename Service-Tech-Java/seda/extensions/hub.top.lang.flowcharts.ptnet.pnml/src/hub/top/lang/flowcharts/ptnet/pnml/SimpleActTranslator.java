package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.SimpleActivity;

public class SimpleActTranslator extends Translator {

	
	
	public SimpleActivity getNode(){
		if (!(super.getNode() instanceof SimpleActivity))
			throw new NullPointerException("Error. Instantiated SimpleActTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SimpleActivity)super.getNode());
	}
	
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		SimpleActivity act = this.getNode();
	//	if (!act.isOptional()){
			// translating a non-optional SimpleActivity
			this.placeCount++;
			PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel()+"_start", 'p')));
			pS.setContainerPageHLAPI(page);
				
			this.transCount++;
			TransitionHLAPI t0 = new TransitionHLAPI("t"+this.getTransCount()); t0.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel(), 't')));
			t0.setContainerPageHLAPI(page);
				
			this.placeCount++;
			PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel()+"_end", 'p')));
			pE.setContainerPageHLAPI(page);
				
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pS, t0, page);
			
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t0, pE, page);
				
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
	//	}
	/*	else {
			// translating an optional SimpleActivity
			// StartPlace
			this.placeCount++;
			PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel()+"_start")));
			pS.setContainerPageHLAPI(page);
			
			// path where act is executed
			this.transCount++;
			TransitionHLAPI t0 = new TransitionHLAPI("t"+this.getTransCount()); t0.setNameHLAPI(new NameHLAPI(super.createPNMLLabel("execute "+act.getLabel())));
			t0.setContainerPageHLAPI(page);
				
			this.placeCount++;
			PlaceHLAPI pS1 = new PlaceHLAPI("p"+this.getPlaceCount()); pS1.setNameHLAPI(new NameHLAPI(""));
			pS1.setContainerPageHLAPI(page);
				
			this.transCount++;
			TransitionHLAPI t1 = new TransitionHLAPI("t"+this.getTransCount()); t1.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel())));
			t1.setContainerPageHLAPI(page);
				
							
			// path where act is not executed
			this.transCount++;
			TransitionHLAPI t2 = new TransitionHLAPI("t"+this.getTransCount()); t2.setNameHLAPI(new NameHLAPI(super.createPNMLLabel("don't execute")));
			t2.setContainerPageHLAPI(page);
				
			// EndPlace
			this.placeCount++;
			PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(act.getLabel()+"_end")));
			pE.setContainerPageHLAPI(page);
				
			// Arcs
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pS, t0, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t0, pS1, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pS1, t1, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t1, pE, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pS, t2, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t2, pE, page);
			
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
		} */
	}

}
