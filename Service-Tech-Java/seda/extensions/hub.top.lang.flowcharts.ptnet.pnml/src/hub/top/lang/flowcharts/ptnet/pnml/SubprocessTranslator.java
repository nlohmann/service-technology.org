package hub.top.lang.flowcharts.ptnet.pnml;

import java.util.List;
import java.util.Vector;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.hlapi.NameHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PageHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.PlaceHLAPI;
import fr.lip6.move.pnml.ptnet.hlapi.RefPlaceHLAPI;
import hub.top.lang.flowcharts.Subprocess;

public class SubprocessTranslator extends Translator {
	
	List<RefPlaceHLAPI> refStarts;
	List<RefPlaceHLAPI> refEnds;

	public Subprocess getNode(){
		if (!(super.getNode() instanceof Subprocess))
			throw new NullPointerException("Error. Instantiated SubprocessTranslator with wrong type: " + super.getNode().getClass().getSimpleName());
		return((Subprocess)super.getNode());
	}
	
	public void translate() throws InvalidIDException, VoidRepositoryException{
		this.refStarts = new Vector<RefPlaceHLAPI>();
		this.refEnds = new Vector<RefPlaceHLAPI>();
		
		Subprocess sub = this.getNode();
		
		// no arcs here, because they will be inserted automatically when creating arc to/from the ReferencePlaces
		this.placeCount++;
		PlaceHLAPI pS = new PlaceHLAPI("p"+this.getPlaceCount()); pS.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(sub.getLabel()+"_start", 'p')));
		this.transCount++;
		PageHLAPI subpage = new PageHLAPI("t"+this.getTransCount(), new NameHLAPI(super.createPNMLLabel(sub.getLabel(), 't')), null, page);
		this.placeCount++;
		PlaceHLAPI pE = new PlaceHLAPI("p"+this.getPlaceCount()); pE.setNameHLAPI(new NameHLAPI(super.createPNMLLabel(sub.getLabel()+"_end", 'p')));
			
		pS.setContainerPageHLAPI(page);
		subpage.setContainerPageHLAPI(page);
		pE.setContainerPageHLAPI(page);
			
		this.setPair(new NodePair(pS, pE));
				
			
		// more than one incoming arc?
		if (sub.getIncoming().size() > 1){
			this.getPair().removeStart(pS);
			super.addInterfaceIn(pS);
		}
		else if (sub.getIncoming().size() > 0) this.getPair().setOtherEnd(pS, sub.getIncoming().get(0).getSrc());
			
		// for each inserted startPlace a ReferencePlace in the subnet
		for (PlaceHLAPI p : this.getPair().getStart()){
			RefPlaceHLAPI r = new RefPlaceHLAPI("ref_"+p.getId(), p);
			r.setContainerPageHLAPI(subpage);
			this.refStarts.add(r);
		}
			
		// more than one outgoing arc?
		if (sub.getOutgoing().size() > 1){
			this.getPair().removeEnd(pE);
			super.addInterfaceOut(pE);
		}
		else if (sub.getOutgoing().size() > 0) this.getPair().setOtherEnd(pE, sub.getOutgoing().get(0).getTarget());
			
		// for each inserted endPlace a ReferencePlace in the subnet
		for (PlaceHLAPI p : this.getPair().getEnd()){
			RefPlaceHLAPI r = new RefPlaceHLAPI("ref_"+p.getId(), p);
			r.setContainerPageHLAPI(subpage);
			this.refEnds.add(r);
		}
			
		super.getTable().put(sub, this.getPair());
		
		// translate the subprocess
		PageTranslator trans = new PageTranslator();
		trans.setCounts(this.placeCount, this.transCount, this.arcCount);
		trans.sub = true;
		trans.setCalling(this);
		trans.setNode(this.getNode());
		trans.setDiagram(sub.getSubprocess());
		trans.setPage(subpage);
		trans.setTable(this.table);
		trans.translate();
		this.setCounts(trans.getPlaceCount(), trans.getTransCount(), trans.getArcCount());
		
	}
}
