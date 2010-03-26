package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.StartNode;

public class StartNodeTranslator extends Translator {

	public StartNode getNode(){
		if (!(super.getNode() instanceof StartNode)){
			throw new NullPointerException("Error. Instantiated StartNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		}
		return ((StartNode)super.getNode());
	}
	
	public void translate(Boolean isStart) {
		StartNode st = this.getNode();
		
		// translating a StartNode
		PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName(super.createLabel(st.getLabel(), 'p'));
		this.net.getPlaces().add(pS);
		// activate StartNode, if this is the start of the workflow
		if (isStart) pS.setToken(1);
		
		this.setPair(new NodePair(pS, pS));
		if (st.getOutgoing().size() > 1){
			this.getPair().removeEnd(pS);
			super.addInterfaceOut(pS);
		}
		else if (st.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, this.getNode().getOutgoing().get(0).getTarget());
		this.getTable().put(st, this.getPair());
	}
}
