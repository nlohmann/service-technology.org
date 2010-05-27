package hub.top.lang.flowcharts.ptnet.pnml;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.ArcHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.TransitionHLAPI;
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.SimpleActivity;
import hub.top.lang.flowcharts.SplitNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.Subprocess;

public class FlowTranslator extends Translator {

	public Flow getNode(){
		if (!(super.getNode() instanceof Flow))
			throw new NullPointerException("Error. Instantiated FlowTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Flow)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		Flow fl = this.getNode();
		// startplace of the Flow
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(fl.getLabel()+"_start", 'p')));
		this.transCount++;
		TransitionHLAPI tS = new TransitionHLAPI("t"+this.getTransCount()); tS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(fl.getLabel()+"_synchronizeStart", 't')));
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), pS, tS, page).setNameHLAPI(new NameHLAPI(""));
		pS.setContainerPageHLAPI(page);
		tS.setContainerPageHLAPI(page);
	
		// endplace of the Flow
		this.transCount++;
		TransitionHLAPI tE = new TransitionHLAPI("t"+this.getTransCount()); tE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(fl.getLabel()+"_synchronizeEnd", 't')));
		this.placeCount++;
		PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(fl.getLabel()+"_end", 'p')));
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), tE, pE, page).setNameHLAPI(new NameHLAPI(""));
		tE.setContainerPageHLAPI(page);
		pE.setContainerPageHLAPI(page);
		
		this.setPair(new NodePair(pS, pE));
			
		if (fl.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (fl.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, fl.getIncoming().get(0).getSrc());
			
		if (fl.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (fl.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, fl.getOutgoing().get(0).getTarget());
			
		super.getTable().put(fl, this.getPair() );
			
		// translate the Flow as if it was a page
		// translate all nodes
  		for (DiagramNode node : fl.getChildNodes()){
  			Translator trans;

			if (node instanceof StartNode) 				trans = new StartNodeTranslator();
			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
  			else if (node instanceof SimpleActivity)	trans = new SimpleActTranslator();
  			else if (node instanceof Event) 			trans = new EventTranslator();
  			else if (node instanceof ResourceNode) 		trans = new ResourceTranslator();
  			else if (node instanceof Endnode) 			trans = new EndNodeTranslator();
  			else if (node instanceof SplitNode)			trans = new SplitTranslator();
  			else if (node instanceof MergeNode)			trans = new MergeTranslator();
  			else if (node instanceof Subprocess)		trans = new SubprocessTranslator();
  			else if (node instanceof Flow)				trans = new FlowTranslator();
  			else throw new NullPointerException("Error while translating Node " + node.getLabel() + ". Node has no known or yet implemented Type.");
  				
  			trans.setPage(page);
  			trans.setNode(node);
  			trans.setTable(this.table);
  			trans.setCounts(this.placeCount, this.transCount, this.arcCount);
  			trans.translate();
  			this.setPlaceCount(trans.getPlaceCount());
  			this.setTransCount(trans.getTransCount());
  			this.setArcCount(trans.getArcCount());
  			
  			if (node instanceof ActivityNode && ((ActivityNode) node).isOptional()) this.insertOpt((ActivityNode) node);
  			
  			// insert arcs where no arcs are in the diagram 
  				//between begin of flow and startnodes
  			if (node instanceof StartNode && node.getIncoming().isEmpty()){
  				this.arcCount++;
  				new ArcHLAPI("a"+this.getArcCount(), tS, this.table.get(node).getStart().get(0), page);
  			}
  				// between endnodes and end of flow
  			if (node instanceof Endnode && node.getOutgoing().isEmpty()){
  				this.arcCount++;
  				new ArcHLAPI("a"+this.getArcCount(), this.table.get(node).getEnd().get(0), tE, page);
  			}
  		}
  		
	}
	
	/**
	 * inserts a path where the activity n is not executed 
	 * @param n an optional ActivityNode
	 * @throws VoidRepositoryException 
	 * @throws InvalidIDException 
	 */
	private void insertOpt(ActivityNode n) throws InvalidIDException, VoidRepositoryException{
		NodePair np = this.table.get(n);
		PlaceHLAPI pS;
		PlaceHLAPI pE;
		
		// if there are more than one Startplaces for n get the place after the interfaceIn
		if (np.getStart().size() > 1){
			//					first Startplace	outgoing arc			target = transition outgoing arc		target = Startplace
			pS = (PlaceHLAPI) np.getStart().get(0).getOutArcsHLAPI().get(0).getTargetHLAPI().getOutArcsHLAPI().get(0).getTargetHLAPI();
		}
		else pS = np.getStart().get(0);
		
		if (np.getEnd().size() > 1){
			//			first Endplace	incoming arc			source = transition 	incoming arc		source = Endplace
			pE = (PlaceHLAPI) np.getEnd().get(0).getInArcsHLAPI().get(0).getSourceHLAPI().getInArcsHLAPI().get(0).getSourceHLAPI();
		}
		else pE = np.getEnd().get(0);
		
		this.transCount++;
		TransitionHLAPI t = new TransitionHLAPI("t"+this.getTransCount()); t.setNameHLAPI(new NameHLAPI(super.createPNMLLabel("don't execute", 't')));
		t.setContainerPageHLAPI(page);
		
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), pS, t, page);
		this.arcCount++;
		new ArcHLAPI("a"+this.getArcCount(), t, pE, page);
	}
}
