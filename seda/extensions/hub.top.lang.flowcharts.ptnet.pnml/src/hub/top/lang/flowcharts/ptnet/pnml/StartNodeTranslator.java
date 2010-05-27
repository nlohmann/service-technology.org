package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import hub.top.lang.flowcharts.StartNode;

public class StartNodeTranslator extends Translator {

	public StartNode getNode(){
		if (!(super.getNode() instanceof StartNode)){
			throw new NullPointerException("Error. Instantiated StartNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		}
		return ((StartNode)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		StartNode st = this.getNode();
		
		// translating a StartNode
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(st.getLabel(), 'p')));
		pS.setContainerPageHLAPI(page);
		this.setPair(new NodePair(pS, pS));
		if (st.getOutgoing().size() > 1){
			this.getPair().removeEnd(pS);
			super.addInterfaceOut(pS);
		}
		else if (st.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, this.getNode().getOutgoing().get(0).getTarget());
		this.getTable().put(st, this.getPair());
	}
}
