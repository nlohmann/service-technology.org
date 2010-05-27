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
import hub.top.lang.flowcharts.MergeNode;

public class MergeTranslator extends Translator {
	
	public MergeNode getNode(){
		if (!(super.getNode() instanceof MergeNode))
			throw new NullPointerException("Error. Instantiated MergeTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((MergeNode)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		MergeNode node = this.getNode();
		
		// translate EndNode first
		this.placeCount++;
		PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(node.getLabel()+"_end", 'p')));
		pE.setContainerPageHLAPI(super.getPage());
		List<PlaceHLAPI> starts = new Vector<PlaceHLAPI>();
		List<PlaceHLAPI> end = new Vector<PlaceHLAPI>();
		Hashtable<PlaceHLAPI, DiagramNode> otherEnds = new Hashtable<PlaceHLAPI, DiagramNode>();
		end.add(pE);
			
		// add a startPlace and a Transition with Arc to the EndPlace for each incoming Edge
		for (DiagramArc arc: node.getIncoming()){
			this.placeCount++;
			PlaceHLAPI pIn = new PlaceHLAPI("p"+this.getPlaceCount()); pIn.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(node.getLabel()+"_start_"+arc.getSrc().getLabel(), 'p')));
			pIn.setContainerPageHLAPI(super.getPage());
			this.transCount++;
			TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount());
			
			String name = "";
			if (arc.getLabel() == null || arc.getLabel().equalsIgnoreCase("")) name = arc.getSrc().getLabel() + "To" + arc.getTarget().getLabel();
			else name = arc.getLabel();
			t.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(name, 't')));
			
			t.setContainerPageHLAPI(super.getPage());
			
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), pIn, t, page);
			this.arcCount++;
			new ArcHLAPI("a"+this.getArcCount(), t, pE, page);
			starts.add(pIn);
			otherEnds.put(pIn, arc.getSrc());
		}
			
		this.setPair(new NodePair(starts, end));
		this.getPair().setOtherEnds(otherEnds);
		if (node.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (node.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, node.getOutgoing().get(0).getTarget());
		super.getTable().put(node, this.getPair());
			
	}
}
