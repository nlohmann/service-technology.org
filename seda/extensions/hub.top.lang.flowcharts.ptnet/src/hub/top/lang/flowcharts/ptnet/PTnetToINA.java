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
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;

import java.util.Hashtable;

/**
 * This class creates the Outputfiles for INA from a given P/T Net Model
 * 
 * @author Arwen
 *
 */
public class PTnetToINA {

	PtNet net = null;
	String cr = System.getProperty("line.separator");
	String out = null;
	String times = "";
	
	public PTnetToINA(PtNet net, String netName) {
		super();
		this.net = net;
		// <netheader> ::= "P   M   PRE,POST   NETZ  " <netid> "<cr>"
		this.out = "P   M   PRE,POST  NETZ " + 1 + ":" + name(netName.substring(0, netName.lastIndexOf(".")-1));
	}
	
	/**
	 * creates the P/T Net description in INA's language
	 */
	public String createpntFile(){
		// holds all transitions with their numbers
		Hashtable<Transition, Integer> transitions = new Hashtable<Transition, Integer>();
		
		String transdata = "trans nr.             name priority time";
		String placedata = "place nr.             name capacity time";
		String netstruct = "";
		
		Integer tct = 0;
		for (Transition tr : this.net.getTransitions()){	
			TransitionExt t = (TransitionExt) tr;
			
			int time = t.getMinTime();
			if (time <= 0) time = 0; 
			String tName = "";
			if (t.getName().length() == 0) tName = "t" + tct;
			else tName = name(t.getName());
			transdata += cr + "       " + tct + ": " + tName + "        0    " + time;
			times += tct + ":0," + (t.getMaxTime()-t.getMinTime()) + cr;
			transitions.put(t, tct);
			tct++;
		}
		
		Integer pct = 0;
		for (Place pl : this.net.getPlaces()){
			PlaceExt p = (PlaceExt) pl;
			String pName = "";
			if (p.getName() == null || p.getName().length() == 0) pName = "p" + pct;
			else pName = name(p.getName());
			placedata += cr + "       " + pct + ": " + pName + "       oo    0";

			String pre = "";
			for (Transition t : pl.getPreSet()){
				pre += transitions.get(t)+": " + findArc(pl, t).getWeight() + " ";
			}
			
			String post = "";
			for (Transition t : pl.getPostSet()){
				post += transitions.get(t)+": " + findArc(pl, t).getWeight() + " ";
			}
			netstruct += cr + "  " + pct + " " + p.getToken() + "    " + pre;
			if (!(post.length() == 0)) netstruct+= ", " + post;
			pct++;
		}
		
		this.out += netstruct + cr + " @" + cr + placedata + cr + " @" + cr + transdata + cr + " @";
		return this.out;
	}
	
	public String getTimeFile(){
		return this.times;
	}
	
	private String name(String name){
		if (name.length() <= 16){
			String out = name;
			for (int i = 0; i < (16 - name.length()); i++) out += " ";
			return out;
		}
		else return name.substring(0, 16);
	}
	
	private Arc findArc(Place p, Transition t){
		for (Arc a : p.getIncoming()) if (a.getSource() == t) return a;
		for (Arc a : p.getOutgoing()) if (a.getTarget() == t) return a;
		return null;
	}

}
