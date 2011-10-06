package hub.top.lang.flowcharts.ptnet.pnml;

import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.SplitNode;

public class SplitTranslator extends Translator {

	public SplitNode getNode(){
		if (!(super.getNode() instanceof SplitNode))
			throw new NullPointerException("Error. Instantiated SplitTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((SplitNode)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		SplitNode node = this.getNode();
		
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(node.getLabel()+"_start", 'p')));
		pS.setContainerPageHLAPI(super.getPage());
		List<PlaceHLAPI> start = new Vector<PlaceHLAPI>();
		List<PlaceHLAPI> ends = new Vector<PlaceHLAPI>();
		Hashtable<PlaceHLAPI, DiagramNode> otherEnds = new Hashtable<PlaceHLAPI, DiagramNode>();
		start.add(pS);
			
		// add a transition and an EndPlace for every outgoing Edge
		for(DiagramArc arc: node.getOutgoing()){
			this.transCount++;
			TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount()); 
			String name = "";
			if (arc.getLabel().equals("") || arc.getLabel() == null) name = "";
			else name = arc.getLabel();
			t.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(name, 't')));
			t.setContainerPageHLAPI(super.getPage());
				
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pS, t, page);
			
			this.placeCount++;
			PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount());
			pE.setContainerPageHLAPI(super.getPage());
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t, pE, page);
			ends.add(pE);
			otherEnds.put(pE, arc.getTarget());
		}
		this.setPair(new NodePair(start, ends));
		this.getPair().setOtherEnds(otherEnds);
			
		if (node.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (node.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, node.getIncoming().get(0).getSrc());
			
		super.getTable().put(node, this.getPair());
	}
}
