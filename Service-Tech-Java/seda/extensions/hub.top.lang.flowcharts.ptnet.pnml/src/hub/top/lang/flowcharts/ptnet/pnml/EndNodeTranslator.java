package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import hub.top.lang.flowcharts.Endnode;

import java.util.List;
import java.util.Vector;

public class EndNodeTranslator extends Translator {
	public Endnode getNode(){
		if (!(super.getNode() instanceof Endnode))
			throw new NullPointerException("Error. Instantiated EndNodeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return ((Endnode)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		Endnode st = this.getNode();
		// translating an EndNode
		List<PlaceHLAPI> startNodes = new Vector<PlaceHLAPI>();
		List<PlaceHLAPI> endNode = new Vector<PlaceHLAPI>();
		
		this.placeCount++;
		PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(st.getLabel(), 'p')));
		pE.setContainerPageHLAPI(page);
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
