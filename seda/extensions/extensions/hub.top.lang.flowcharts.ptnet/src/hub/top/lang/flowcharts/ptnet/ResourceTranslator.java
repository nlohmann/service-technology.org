package hub.top.lang.flowcharts.ptnet;


import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.ResourceNode;

import java.util.Hashtable;

public class ResourceTranslator extends Translator {

	// remember the resource nodes by name for later reuse
	Hashtable<String, NodePair> translatedResources;
	
	public Hashtable<String, NodePair> getTranslatedResources() {
		return translatedResources;
	}

	public void setTranslatedResources(
			Hashtable<String, NodePair> translatedResources) {
		this.translatedResources = translatedResources;
	}

	public ResourceNode getNode(){
		if (!(super.getNode() instanceof ResourceNode))
			throw new NullPointerException("Error. Instantiated ResourceTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((ResourceNode)super.getNode());
	}
	
	public void translate() {
		ResourceNode res = this.getNode();
		// resource is already translated --> use the same NodePair
		if (this.getTranslatedResources().containsKey(res.getLabel())){
			this.getTable().put(res, this.getTranslatedResources().get(res.getLabel()));
		}
		else {
			// translating a resource
			PlaceExt pS = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pS.setName(super.createLabel(res.getLabel(), 'p'));
			//if (res.getIncoming().isEmpty()) pS.setToken(1);
			this.net.getPlaces().add(pS);
			this.setPair(new NodePair(pS, pS));
			
//			if (res.getIncoming().size() > 1){
//				this.getPair().removeStart(pS);
//				super.addInterfaceIn(pS);
//			}
//			else if (res.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, res.getIncoming().get(0).getSrc());
//			
//			if (res.getOutgoing().size() > 1){
//				this.getPair().removeEnd(pS);
//				super.addInterfaceOut(pS);
//			}
//			else if (res.getOutgoing().size() > 0) this.getPair().setOtherEnd(pS, res.getOutgoing().get(0).getTarget());
			
			super.getTable().put(res, this.getPair());
			this.getTranslatedResources().put(res.getLabel(), this.getPair());
		}
	}
	
}
