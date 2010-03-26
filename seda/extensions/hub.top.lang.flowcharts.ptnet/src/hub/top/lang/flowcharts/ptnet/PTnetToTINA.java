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

import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;

import java.util.Hashtable;

/**
 * This class creates the Outputfiles for TINA from a given P/T Net Model
 * 
 * @author Arwen
 *
 */
public class PTnetToTINA {

	PtNet net = null;
	String cr = System.getProperty("line.separator");
	String out = null;
	
	public PTnetToTINA(PtNet net, String netName) {
		super();
		this.net = net;
		this.out = "net " + name(netName) + cr;
	}
	
	/**
	 * creates the P/T Net description in TINA's language
	 */
	public String createNetFile(){
		// holds all places with their numbers
		Hashtable<Place, String> places = new Hashtable<Place, String>();
		Hashtable<TransitionExt, ExtArc> extended = new Hashtable<TransitionExt, ExtArc>();
		String pldesc = "";
		Integer pct = 0;
		
		for (Place pl : this.net.getPlaces()){
			PlaceExt p = (PlaceExt) pl;
			places.put(p, "p" + pct);
			if (p.getToken() > 0) pldesc += "pl p" + pct + " (" + p.getToken() + ")" + cr;
			pct++;
			
			// if after p comes a optional activity with duration > 0, insert another place p1
			if (p.getPostSet().size() > 1){
				for (Transition t : p.getPostSet()){
					if (((TransitionExt) t).getMaxTime() > 0) {
						PlaceExt p1 = PtnetLoLAFactory.eINSTANCE.createPlaceExt();
						p1.setName("decided_" + t.getName());
						places.put(p1, "p"+pct);
						
						ExtArc e = new ExtArc(p, (TransitionExt) t, p1);
						extended.put((TransitionExt) t, e);
						pct++;	
					}
				}
			}
		}
		
		Integer tct = 0;
		for (Transition tr : this.net.getTransitions()){	
			TransitionExt t = (TransitionExt) tr;
			String pre = "";
			String post = "";
			
			// if t is optional with duration > 0, insert another transition
			if (extended.containsKey(t)){
				TransitionExt t1 = PtnetLoLAFactory.eINSTANCE.createTransitionExt();
				t1.setName("run_"+t.getName());
				t1.setCost(0.0);
				t1.setProbability(1.0);
				t1.setMaxTime(0);
				
				// preset excluding p
				for (Place p : t.getPreSet()){
					if (p != extended.get(t).getSrc()) pre += places.get(p) + " ";
				}
				for (Place p : t.getPostSet()){
					post += places.get(p) + " ";
				}
				
				extended.get(t).setNewTransition(t1);
				String tName = "";
				if (t.getName() == null || t.getName().length() == 0) tName = "t" + pct;
				else tName = name(t.getName());
				
				// insert t1 to out-String (pre is the original Source of the extended Arc, post is the newly inserted place (p1))
				this.out += "tr t" + tct + ":" + name(t1.getName()) + " [0, w[ " + places.get(extended.get(t).getSrc()) + "-> " + places.get(extended.get(t).getNewPlace()) + cr;
				tct++;
				// now insert t (pre is the original preSet of t excluding p, plus p1, post is original)
				this.out += "tr t" + tct + ":" + tName + " [" + t.getMinTime() + ", " + t.getMaxTime() + "] " 
						 +  pre + " " + places.get(extended.get(t).getNewPlace()) + "-> " + post + cr;
				tct++;
			}
			else {
				for (Place p : t.getPreSet()){
					pre += places.get(p) + " ";
				}
				for (Place p : t.getPostSet()){
					post += places.get(p) + " ";
				}
				String tName = "";
				if (t.getName() == null || t.getName().length() == 0) tName = "t" + pct;
				else tName = name(t.getName());
				
				if (t.getMaxTime() > 0) this.out += "tr t" + tct + ":" + tName + " [" + t.getMinTime() + ", " + t.getMaxTime() + "] "  + pre + "-> " + post + cr;
				else this.out += "tr t" + tct + ":" + name(t.getName()) + " [0, w[ "  + pre + "-> " + post + cr;
				tct++;
			}
		}
		
		this.out += pldesc;
		return this.out;
	}
	
	
	private String name (String s){
		String ret = s.replaceAll(" ", "");
		ret = ret.replaceAll("/", "");
		ret = ret.replaceAll("-", "");
		ret = ret.replaceAll("[?]", "DEC");
		ret = ret.replaceAll("[(]", "_");
		ret = ret.replaceAll("[)]", "_");
		ret = ret.replaceAll(",", "AND");
		return ret.replaceAll("[.]", "_");
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
