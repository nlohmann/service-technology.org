package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.lang.flowcharts.Endnode;

import java.util.List;
import java.util.Vector;

public class EndNodeTranslator extends Translator {
	public Endnode getNode(){
		if (!(super.getNode() instanceof Endnode))
			throw new NullPointerException("Error. Instantiated EndNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return ((Endnode)super.getNode());
	}
	
	public void translate() {
		Endnode st = this.getNode();
		// translating an EndNode
		List<PlaceExt> startNodes = new Vector<PlaceExt>();
		List<PlaceExt> endNode = new Vector<PlaceExt>();
		
		PlaceExt pE = PtnetLoLAFactory.eINSTANCE.createPlaceExt(); pE.setName(super.createLabel(st.getLabel(), 'p'));
		this.net.getPlaces().add(pE);
		
		endNode.add(pE);
		this.pair = new NodePair(startNodes, endNode);
			
		// more than one incoming arc --> need to insert an InterfaceIn (on an EndNode there cannot be outgoing arcs)
		if (st.getIncoming().size() > 1){
			addInterfaceIn(pE);	
		}
		else {
			startNodes.add(pE);
			this.pair.setOtherEnd(pE, st.getIncoming().get(0).getSrc());
		}
			
		super.getTable().put(st, this.getPair());
		
	}
}
