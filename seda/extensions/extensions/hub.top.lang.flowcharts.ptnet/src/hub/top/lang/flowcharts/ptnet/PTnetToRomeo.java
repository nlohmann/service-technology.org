package hub.top.lang.flowcharts.ptnet;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.ArcToPlaceExt;
import hub.top.editor.ptnetLoLA.ArcToTransitionExt;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;

import java.util.Hashtable;

/**
 * This class creates the Outputfiles for Romeo from a given P/T Net Model
 * 
 * @author Arwen
 *
 */
public class PTnetToRomeo {

	PtNet net = null;
	String cr = System.getProperty("line.separator");
	String out = null;

	public PtNet getNet() {
		return net;
	}

	public void setNet(PtNet net) {
		this.net = net;
	}
	
	public PTnetToRomeo(PtNet net, String netName) {
		super();
		this.net = net;
		this.out = 	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + cr 
					+ "<TPN name=\"" + netName + "\">" + cr;
	}
	
	/**
	 * creates the P/T Net description in Romeo's language
	 */
	public String createXMLFile(){
		// holds all places/transitions with their numbers
		Hashtable<Place, Integer> places = new Hashtable<Place, Integer>();
		Hashtable<Transition, Integer> transitions = new Hashtable<Transition, Integer>();
		Hashtable<TransitionExt, ExtArc> extended = new Hashtable<TransitionExt, ExtArc>();
		
		String pldesc = "";
		String trdesc = "";
		String arcdesc = "";
		Integer pct = 1;
		double xPos = 375.0;	// ca Mitte
		double yPos = 50.0;		// oben
				
		// describe all Places
		for (Place pl : this.net.getPlaces()){
			PlaceExt p = (PlaceExt) pl;
			
			places.put(p, pct);
			pldesc += this.addPlace(pct, p, xPos, yPos);
			yPos += 100;
			pct++;
			
			// if after p comes a optional activity with duration > 0, insert another place p1
			if (p.getPostSet().size() > 1){
				for (Transition t : p.getPostSet()){
					if (((TransitionExt) t).getMaxTime() > 0) {
						PlaceExt p1 = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
						p1.setName("decided_" + t.getName());
						places.put(p1, pct);
						
						ExtArc e = new ExtArc(p, (TransitionExt) t, p1);
						extended.put((TransitionExt) t, e);
						
						pldesc += this.addPlace(pct, p1, xPos, yPos);
						yPos += 100;
						pct++;
						
					}
				}
			}
		}
		
		xPos = 375;
		yPos = 100;
		Integer tct = 1;
		
		// describe all Transitions
		for (Transition tr : this.net.getTransitions()){	
			TransitionExt t = (TransitionExt) tr;
			transitions.put(t, tct);
			trdesc += this.addTransition(tct, t, xPos, yPos);
			tct++;
			yPos += 100;
			
			// if t is optional with duration > 0, insert another transition
			if (extended.containsKey(t)){
				TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
				t1.setName("run_"+t.getName());
				t1.setCost(0.0);
				t1.setProbability(1.0);
				t1.setMaxTime(0);
				
				transitions.put(t1, tct);
				extended.get(t).setNewTransition(t1);
				
				trdesc += this.addTransition(tct, t1, xPos, yPos);
				tct++;
				yPos += 100;
			}
			
		}
		
		for (Arc a : this.net.getArcs()){
			if (a instanceof ArcToTransitionExt){
				ArcToTransitionExt aT = (ArcToTransitionExt) a;
				
				if (! extended.containsKey(aT.getTarget())){
					arcdesc += "\t<arc place=\"" + places.get(aT.getSource()) + "\" transition=\"" + transitions.get(aT.getTarget()) + "\" type=\"PlaceTransition\" weight=\"1\">" + cr
							+ "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
							+ "\t\t<graphics color=\"0\">" + cr
							+ "\t\t</graphics>" + cr
							+ "\t</arc>" + cr
							+ cr;
				}
				else {
					ExtArc ea = extended.get(aT.getTarget());
					if (ea.getSrc() != aT.getSource()) throw new NullPointerException("Error while translating to Roméo file: something is wrong with Extended Arc");
					arcdesc += "\t<arc place=\"" + places.get(aT.getSource()) + "\" transition=\"" + transitions.get(ea.getNewTransition()) + "\" type=\"PlaceTransition\" weight=\"1\">" + cr
							+ "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
							+ "\t\t<graphics color=\"0\">" + cr
							+ "\t\t</graphics>" + cr
							+ "\t</arc>" + cr
							+ cr
							+ "\t<arc place=\"" + places.get(ea.getNewPlace()) + "\" transition=\"" + transitions.get(ea.getTgt()) + "\" type=\"PlaceTransition\" weight=\"1\">" + cr
							+ "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
							+ "\t\t<graphics color=\"0\">" + cr
							+ "\t\t</graphics>" + cr
							+ "\t</arc>" + cr
							+ cr;
				}
			}
			else if (a instanceof ArcToPlaceExt){
				ArcToPlaceExt aP = (ArcToPlaceExt) a;
				if (! extended.containsKey(aP.getSource())){
					arcdesc += "\t<arc place=\"" + places.get(aP.getTarget()) + "\" transition=\"" + transitions.get(aP.getSource()) + "\" type=\"TransitionPlace\" weight=\"1\">" + cr
							 + "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
							 + "\t\t<graphics color=\"0\">" + cr
							 + "\t\t</graphics>" + cr
							 + "\t</arc>" + cr
							 + cr;
				}
				else {
					ExtArc ea = extended.get(aP.getSource());
					arcdesc += "\t<arc place=\"" + places.get(ea.getNewPlace()) + "\" transition=\"" + transitions.get(ea.getNewTransition()) + "\" type=\"TransitionPlace\" weight=\"1\">" + cr
					 + "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
					 + "\t\t<graphics color=\"0\">" + cr
					 + "\t\t</graphics>" + cr
					 + "\t</arc>" + cr
					 + cr
					 + "\t<arc place=\"" + places.get(aP.getTarget()) + "\" transition=\"" + transitions.get(aP.getSource()) + "\" type=\"TransitionPlace\" weight=\"1\">" + cr
					 + "\t\t<nail xnail=\"0\" ynail=\"0\"/>" + cr
					 + "\t\t<graphics color=\"0\">" + cr
					 + "\t\t</graphics>" + cr
					 + "\t</arc>" + cr
					 + cr;
				}
			}
		}
		
		String prefs = "\t<preferences>" + cr
					 + "\t\t<colorPlace c0=\"SkyBlue2\" c1=\"gray\" c2=\"cyan\" c3=\"green\" c4=\"yellow\" c5=\"brown\" />" + cr + cr
					 + "\t\t<colorTransition c0=\"yellow\" c1=\"gray\" c2=\"cyan\" c3=\"green\" c4=\"SkyBlue2\" c5=\"brown\" />" + cr + cr
					 + "\t\t<colorArc c0=\"black\" c1=\"gray\" c2=\"blue\" c3=\"#beb760\" c4=\"#be5c7e\" c5=\"#46be90\" />" + cr + cr
					 + "\t</preferences>" +cr
					 + "</TPN>";
		
		this.out += pldesc + trdesc + arcdesc + prefs;
		return this.out;
	}
	
	private String addPlace(Integer pct, PlaceExt p, double xPos, double yPos){
		String pName = "";
		if (p.getName() == null || p.getName().length() == 0) pName = "p" + pct;
		else pName = name(p.getName());
		return( "\t<place id=\"" + pct + "\" label=\"" + pName + "\" initialMarking=\"" + p.getToken() + "\">" + cr
			   + "\t\t<graphics color=\"0\">" + cr
			   + "\t\t\t<position x=\"" + xPos + "\" y=\"" + yPos + "\"/>" + cr
			   + "\t\t\t<deltaLabel deltax=\"19\" deltay=\"-6\"/>" + cr
			   + "\t\t</graphics>" + cr
			   + "\t\t<scheduling gamma=\"0\" omega=\"0\"/>" + cr
			   + "\t</place>" + cr 
			   + cr);
	}
	
	private String addTransition(Integer tct, TransitionExt t, double xPos, double yPos){
		String tName = "";
		if (t.getName() == null || t.getName().length() == 0) tName = "p" + tct;
		else tName = name(t.getName());
		return ("\t<transition id=\"" + tct + "\" label=\"" + tName + "\" eft=\"" + t.getMinTime() + "\" lft=\"" + t.getMaxTime() + "\">" + cr
					+ "\t\t<graphics color=\"0\">" + cr
					+ "\t\t\t<position x=\"" + xPos + "\" y=\"" + yPos + "\"/>" + cr
					+ "\t\t\t<deltaLabel deltax=\"35\" deltay=\"-7\"/>" + cr
					+ "\t\t</graphics>" + cr
					+ "\t</transition>" + cr
					+ cr);
	}
	
		
	private String name (String s){
		return s.replace('.', '_');
	}
	
	private class ExtArc{
		PlaceExt src;
		TransitionExt tgt;
		PlaceExt newPlace;
		TransitionExt newTransition;
		
		public TransitionExt getNewTransition() {
			return newTransition;
		}

		public void setNewTransition(TransitionExt newTransition) {
			this.newTransition = newTransition;
		}

		public PlaceExt getSrc() {
			return src;
		}

		public TransitionExt getTgt() {
			return tgt;
		}

		public PlaceExt getNewPlace() {
			return newPlace;
		}

		public ExtArc(PlaceExt src, TransitionExt tgt, PlaceExt newPlace,
				TransitionExt newTransition) {
			super();
			this.src = src;
			this.tgt = tgt;
			this.newPlace = newPlace;
			this.newTransition = newTransition;
			
		}
		
		public ExtArc(PlaceExt src, TransitionExt tgt, PlaceExt newPlace) {
			super();
			this.src = src;
			this.tgt = tgt;
			this.newPlace = newPlace;
			this.newTransition = null;
		}	
	}
}
