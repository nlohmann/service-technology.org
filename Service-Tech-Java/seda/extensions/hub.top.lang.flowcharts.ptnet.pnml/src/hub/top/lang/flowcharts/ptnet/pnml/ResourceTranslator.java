package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import hub.top.lang.flowcharts.ResourceNode;

public class ResourceTranslator extends Translator {

	public ResourceNode getNode(){
		if (!(super.getNode() instanceof ResourceNode))
			throw new NullPointerException("Error. Instantiated ResourceTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((ResourceNode)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		ResourceNode res = this.getNode();
		// translating a resource
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(res.getLabel(), 'p')));
		pS.setContainerPageHLAPI(page);
		this.setPair(new NodePair(pS, pS));
			
		if (res.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (res.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, res.getIncoming().get(0).getSrc());
			
		if (res.getOutgoing().size() > 1){
			this.getPair().removeEnd(pS);
			super.addInterfaceOut(pS);
		}
		else if (res.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, res.getOutgoing().get(0).getTarget());
			
		super.getTable().put(res, this.getPair());
	}
}
