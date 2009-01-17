/*****************************************************************************\
 * Copyright (c) 2008, 2009 Dirk Fahland. All rights reserved.
 * EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.greta.oclets.ts;

import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Orientation;
import hub.top.greta.oclets.canonical.CNode;
import hub.top.greta.oclets.canonical.CNodeSet;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedList;

public class AdaptiveSystemTS {
	
	//public HashSet<AdaptiveSystemState> states = new HashSet<AdaptiveSystemState>();
	public HashSet<AdaptiveSystemStep> steps = new HashSet<AdaptiveSystemStep>();
	public HashSet<CNodeSet> states = new HashSet<CNodeSet>();
	
	//public AdaptiveSystemState initialState;
	public CNodeSet initial;
	
	private Oclet[] oclets;
	// private CNodeSet[] preNets;
	private CNodeSet[] ocletNets;
	
	private CNode[][][] ocletCuts;
	
	private LinkedList<CNodeSet> unvisitedStates = new LinkedList<CNodeSet>();
	
	private int stateCount = 0;
	
	public AdaptiveSystemTS(hub.top.adaptiveSystem.AdaptiveSystem as) {
		initial = CNodeSet.fromAdaptiveProcess(as.getAdaptiveProcess());
		
		//initialState = new AdaptiveSystemState(initial);
		//states.add(initialState);
		initial.num = stateCount++;
		states.add(initial);
		unvisitedStates.add(initial);
		
		int ocletNum = as.getOclets().size();
		oclets = new Oclet[ocletNum];
		//preNets = new CNodeSet[ocletNum];
		ocletNets = new CNodeSet[ocletNum];
		ocletCuts = new CNode[ocletNum][][];
		
		// first store normal oclets, then store anti-oclets, this guarantees
		// the correct order of application when firing all enabled oclets
		int ocletCount = 0;
		for (Oclet o : as.getOclets()) {
			if (o.getOrientation() == Orientation.NORMAL) {
				oclets[ocletCount] = o;
				ocletNets[ocletCount] = CNodeSet.fromOclet(o);

				Collection<CNode[]> cuts = ocletNets[ocletCount].getAllCuts(true, true);
				ocletCuts[ocletCount] = new CNode[cuts.size()][];
				int ocletCutCount = 0;
				for (CNode[] cut : cuts)
					ocletCuts[ocletCount][ocletCutCount++] = cut;
				/*
				System.out.println("cuts of "+o.getName()+" ----------");
				ocletCutCount = 0;
				for (CNode[] cut : cuts) {
					System.out.print(ocletCutCount+" [");
					for (int i=0;i<cut.length;i++)
						System.out.print(cut[i]+",");
					System.out.println("]");
					ocletCutCount++;
				}
				*/
				ocletCount++;
			}
		}
		for (Oclet o : as.getOclets()) {
			if (o.getOrientation() == Orientation.ANTI) {
				oclets[ocletCount] = o;
				ocletNets[ocletCount] = CNodeSet.fromOclet(o);

				Collection<CNode[]> cuts = ocletNets[ocletCount].getAllCuts(true, true);
				ocletCuts[ocletCount] = new CNode[cuts.size()][];
				int ocletCutCount = 0;
				for (CNode[] cut : cuts)
					ocletCuts[ocletCount][ocletCutCount++] = cut;
				/*
				System.out.println("cuts of "+o.getName()+" ----------");
				ocletCutCount = 0;
				for (CNode[] cut : cuts) {
					System.out.print(ocletCutCount+" [");
					for (int i=0;i<cut.length;i++)
						System.out.print(cut[i]+",");
					System.out.println("]");
					ocletCutCount++;
				}
				*/
				ocletCount++;
			}
		}
	}
	
	public CNode.MatchingRelation[] enabledOclets (CNodeSet state) {
		CNode.MatchingRelation enabled[] = new CNode.MatchingRelation[oclets.length];
		for (int i=0;i<oclets.length;i++) {
			CNode.MatchingRelation m = new CNode.MatchingRelation();
			if (CNodeSet.nodesContainedIn(ocletNets[i].getCutNodes(), state.getCutNodes(), m))
				enabled[i] = m;
			else
				enabled[i] = null;
		}
		return enabled;
	}
	
	public CNode[] enabledEvents (CNodeSet state) {
		Collection<CNode> listEnabled = state.getEnabledEvents();
		return listEnabled.toArray(new CNode[listEnabled.size()]);
	}
	
	public CNodeSet succ(CNodeSet state, CNode.MatchingRelation fireOclets[]) {
		CNodeSet s = CNodeSet.fromCNodeSet_afterCut(state);
		
		for (int i=0; i< oclets.length;i++) {
			if (fireOclets[i] != null) {
				CNodeSet o = ocletNets[i].instantiate(fireOclets[i]);
				
				if (oclets[i].getOrientation() == Orientation.NORMAL) {
					for (CNode c : o.getAllNodes()) {
						s.addNode(c);
					}
					// update the maximal nodes of the successor state
					s.getMaxNodes().addAll(o.getMaxNodes());
				} else {
					for (CNode c : o.getAllNodes()) {
						Collection<CNode> removeCandidates = s.get(new Integer(c.hashCode()));
						if (removeCandidates == null)
							continue;
						
						for (CNode remC : removeCandidates) {
							if (c.equals(remC)) {
								s.removeNode(remC);
								break;
							}
						}
					}
				}
			}
		}
		
		return s;
	}
	
	public CNodeSet succ(CNodeSet state, CNode fireEvent) {
		CNodeSet s = state.fire(fireEvent);

		return s;
	}
	
	public CNodeSet[] succ(CNodeSet state, CNode[] fireEvents) {
		CNodeSet successors[] = new CNodeSet[fireEvents.length];
		for (int i=0;i<fireEvents.length;i++) {
			successors[i] = succ(state, fireEvents[i]);
		}
		return successors;
	}
	
	public boolean equivalentStates(CNodeSet s1, CNodeSet s2) {
		int enabledCutNum_s1[] = new int[oclets.length];
		int enabledCutNum_s2[] = new int[oclets.length];
		
		for (int ocletNum = 0; ocletNum < oclets.length; ocletNum++) {
			int cutNum = 0;
			for (; cutNum < ocletCuts[ocletNum].length; cutNum++) {
				if (CNodeSet.nodesContainedIn(ocletCuts[ocletNum][cutNum], s1.getCutNodes(), null))
					break;
			}
			enabledCutNum_s1[ocletNum] = cutNum;
		}
		
		for (int ocletNum = 0; ocletNum < oclets.length; ocletNum++) {
			int cutNum = 0;
			for (; cutNum < ocletCuts[ocletNum].length; cutNum++) {
				if (CNodeSet.nodesContainedIn(ocletCuts[ocletNum][cutNum], s2.getCutNodes(), null))
					break;
			}
			enabledCutNum_s2[ocletNum] = cutNum;
		}
		/*
			System.out.print(s1.num+": ");
			for (int i=0;i<enabledCutNum_s1.length;i++) System.out.print(enabledCutNum_s1[i]+"|");
			System.out.println();
			System.out.print(s2.num+": ");
			for (int i=0;i<enabledCutNum_s2.length;i++) System.out.print(enabledCutNum_s2[i]+"|");
			System.out.println("\n---------");
		*/
		
		for (int i=0;i<enabledCutNum_s1.length;i++)
			if (enabledCutNum_s1[i] != enabledCutNum_s2[i])
				return false;
		
		return true;
	}
	
	public boolean withOcletSteps = false;
	
	public boolean explore () {
		if (unvisitedStates.isEmpty())
			return false;
		
		CNodeSet state = unvisitedStates.removeFirst();
		
		CNode.MatchingRelation[] oEnabled = enabledOclets(state);
		CNodeSet s_oclet;
		
		boolean doOcletStep = false;
		for (int i=0; i < oEnabled.length; i++) {
			if (oEnabled[i] != null) {
				doOcletStep = true;
				break;
			}
		}
		
		boolean newState = true;
		
		if (doOcletStep) {
			s_oclet = succ(state, oEnabled);
			s_oclet.num = stateCount++;
			
			// check of the there is an equivalent state in the transition system
			for (CNodeSet s : states) {
				if (s == state)	// do not identify states of one oclet-step
					continue;
				
				if (equivalentStates(s_oclet, s)) {
				//	System.out.println("state "+s.num+" and "+(s_oclet.num)+ " enable the same oclet-cuts");
				//}
				//if (s.equivalentTo(s_oclet)) {
				//	System.out.println("state "+s.num+" equals state "+(s_oclet.num));
					s_oclet = s;
					newState = false;
					break;
				}
			}
		
			if (withOcletSteps) {
				// construct new step and add to the transition system
				AdaptiveSystemStep step = new AdaptiveSystemStep();
				step.source = state;
				step.target = s_oclet;
				step.label = "";
				for (int i=0; i < oEnabled.length; i++) {
					if (oEnabled[i] == null)
						continue;
					
					if (step.label.length() > 0)
						step.label += ", ";
					step.label += oclets[i].getName();
				}
				this.steps.add(step);
					
				if (newState)
					this.states.add(s_oclet);
			}
		} else {
			s_oclet = state;
		}
		
		if (!doOcletStep || newState)
		{
			CNode[] eEnabled = enabledEvents(s_oclet);
			CNodeSet[] s_events = succ(s_oclet, eEnabled);
			
			for (int i=0; i< eEnabled.length; i++) {
				
				newState = true;
				
				s_events[i].num = stateCount++;
				
				for (CNodeSet s : states) {
					if (equivalentStates(s_events[i], s)) {
						//System.out.println("state "+s.num+" and "+(s_events[i].num)+ " enable the same oclet-cuts");
					//}
					//if (s.equivalentTo(s_events[i])) {
					//	System.out.println("state "+s.num+" equals state "+(s_events[i].num));
						s_events[i] = s;
						newState = false;
						break;
					}
				}
				
				// construct new step and add to the transition system
				AdaptiveSystemStep step = new AdaptiveSystemStep();
				if (withOcletSteps)
					step.source = s_oclet;
				else
					step.source = state;
				
				step.target = s_events[i];
				step.label = eEnabled[i].getLabel();
				this.steps.add(step);
				
	
				if (newState) {
					this.states.add(s_events[i]);
					this.unvisitedStates.addLast(s_events[i]);
				}
			}
		}
		return true;
	}
	
	public void printEnabledOclets (CNodeSet state) {
		String result = "enabled in "+this+" :";
		CNode.MatchingRelation[] enabled = enabledOclets (state);
		for (int i=0;i<oclets.length;i++) {
			if (enabled[i] != null)
				result += oclets[i].getName()+", ";
		}
		System.out.println(result);
	}
	
	private static final int OUT_DOT_AVG_STATE_STRING = 50;
	private static final int OUT_DOT_AVG_STEP_STRING = 35;
	private static final int OUT_DOT_AVG_OVERHEAD = 50;
	
	public String toDot () {
		System.out.println("number of states: "+this.states.size());
		
		StringBuilder b = new StringBuilder(states.size()*OUT_DOT_AVG_STATE_STRING+steps.size()*OUT_DOT_AVG_STEP_STRING+OUT_DOT_AVG_OVERHEAD);
		b.append("digraph TS {\n");
		b.append("node [fontname=\"Helvetica\" fontsize=10];\n");
		b.append("edge [fontname=\"Helvetica\" fontsize=10];\n");
		for (CNodeSet state : states) {
			String n_string = ""+state.num;
			/*
			n_string += "\\ncut: \\n";
			for (CNode c : state.getCutNodes()) {
				n_string += c.getLabel()+" ("+c.localId+")\\n";
			}*/
			/*
			n_string += "nodes: \\n";
			for (CNode c : state.getAllNodes()) {
				n_string += c.getLabel()+" ("+c.localId+")\\n";
			}
			*/
			b.append(state.num + " [shape=ellipse label=\""+n_string+"\"];\n");
		}
		for (AdaptiveSystemStep step : steps) {
			b.append(step.source.num + " -> " + step.target.num + " [label=\""+step.label+"\"];\n");
		}
		b.append("}");
		return b.toString();
	}
	
	public String getStatistics() {
		return "|Q|="+states.size() + " |delta|="+steps.size() + " |Q_visited|="+stateCount;
	}
	
	public boolean hasDeadlock() {
		HashSet<CNodeSet> statesWithoutSucc = new HashSet<CNodeSet>(states);
		for (AdaptiveSystemStep step : steps) {
			statesWithoutSucc.remove(step.source);
		}
		return !statesWithoutSucc.isEmpty();
	}
	
	private static final int OUT_GENET_AVG_STEP_STRING = 20;
	private static final int OUT_GENET_AVG_OVERHEAD = 50;
	
	public String toGenet () {
		StringBuilder b = new StringBuilder(steps.size()*OUT_GENET_AVG_STEP_STRING+OUT_GENET_AVG_OVERHEAD);
		b.append(".outputs ");
		HashSet<String> stepLabels = new HashSet<String>(); 
		for (AdaptiveSystemStep step : steps)
			stepLabels.add(toAlphaNum_(step.label));
		for (String stepLabel : stepLabels)
			b.append(stepLabel+" ");
		b.append("\n");
		b.append(".state graph\n");
		for (AdaptiveSystemStep step : steps)
			b.append("s"+step.source.num+" "+toAlphaNum_(step.label)+" s"+step.target.num+"\n");
		b.append(".marking {s"+initial.num+"}\n");
		b.append(".end");
		return b.toString();
	}
	
	public static String toAlphaNum_(String s) {
		char[] letters = s.toCharArray();
		for (int i=0;i<letters.length;i++) {
			if (Character.isLetterOrDigit(letters[i]))
				continue;
			letters[i] = '_';
		}
		return new String(letters);
	}
}
