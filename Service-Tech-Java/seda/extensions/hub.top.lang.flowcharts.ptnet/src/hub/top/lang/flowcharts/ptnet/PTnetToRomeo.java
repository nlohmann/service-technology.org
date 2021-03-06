/*****************************************************************************\
 * Copyright (c) 2009 Konstanze Swist. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Modeling Languages
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on December 12, 2009.
 * The Initial Developer of the Original Code are
 *    Konstanze Swist
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

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
			//PlaceExt p = (PlaceExt) pl;
			
			places.put(pl, pct);
			pldesc += this.addPlace(pct, pl, xPos, yPos);
			yPos += 100;
			pct++;
			
			// if after p comes a optional activity with duration > 0, insert another place p1
			if (pl.getPostSet().size() > 1){
				for (Transition t : pl.getPostSet()){
					if (t instanceof TransitionExt) {
						if (((TransitionExt) t).getMaxTime() > 0) {
							Place p1 = PtnetLoLAFactory.eINSTANCE.createPlace();
							p1.setName("decided_" + t.getName());
							places.put(p1, pct);
							
							ExtArc e = new ExtArc(pl, (TransitionExt) t, p1);
							extended.put((TransitionExt) t, e);
							
							pldesc += this.addPlace(pct, p1, xPos, yPos);
							yPos += 100;
							pct++;
							
						}
					}
				}
			}
		}
		
		xPos = 375;
		yPos = 100;
		Integer tct = 1;
		
		// describe all Transitions
		for (Transition tr : this.net.getTransitions()){	
			//TransitionExt t = (TransitionExt) tr;
			transitions.put(tr, tct);
			trdesc += this.addTransition(tct, tr, xPos, yPos);
			tct++;
			yPos += 100;
			
			// if t is optional with duration > 0, insert another transition
			if (extended.containsKey(tr)){
				TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
				t1.setName("run_"+tr.getName());
				t1.setCost(0.0);
				t1.setProbability(1.0);
				t1.setMaxTime(0);
				
				transitions.put(t1, tct);
				extended.get(tr).setNewTransition(t1);
				
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
					if (ea.getSrc() != aT.getSource()) throw new NullPointerException("Error while translating to Rom�o file: something is wrong with Extended Arc");
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
	
	private String addPlace(Integer pct, Place p, double xPos, double yPos){
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
	
	private String addTransition(Integer tct, Transition t, double xPos, double yPos){
		String tName = "";
		if (t.getName() == null || t.getName().length() == 0) tName = "p" + tct;
		else tName = name(t.getName());
		
		int minTime = 0;
		int maxTime = 0;
		if (t instanceof TransitionExt) {
			minTime = ((TransitionExt)t).getMinTime();
			maxTime = ((TransitionExt)t).getMaxTime();
		}
		
		
		return ("\t<transition id=\"" + tct + "\" label=\"" + tName + "\" eft=\"" + minTime + "\" lft=\"" + maxTime + "\">" + cr
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
		Place src;
		Transition tgt;
		Place newPlace;
		Transition newTransition;
		
		public Transition getNewTransition() {
			return newTransition;
		}

		public void setNewTransition(TransitionExt newTransition) {
			this.newTransition = newTransition;
		}

		public Place getSrc() {
			return src;
		}

		public Transition getTgt() {
			return tgt;
		}

		public Place getNewPlace() {
			return newPlace;
		}

		public ExtArc(Place src, Transition tgt, Place newPlace,
				Transition newTransition) {
			super();
			this.src = src;
			this.tgt = tgt;
			this.newPlace = newPlace;
			this.newTransition = newTransition;
			
		}
		
		public ExtArc(Place src, Transition tgt, Place newPlace) {
			super();
			this.src = src;
			this.tgt = tgt;
			this.newPlace = newPlace;
			this.newTransition = null;
		}	
	}
}
