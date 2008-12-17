/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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

public class AdaptiveSystemBP {
	
	CNodeSet bp;
	
	private Oclet[] oclets;
	// private CNodeSet[] preNets;
	private CNodeSet[] ocletNets;
	
	private CNode[][][] ocletCuts;
	
	private LinkedList< HashSet<CNode> > unvisitedCuts = new LinkedList<HashSet<CNode> >();
	private LinkedList< HashSet<CNode> > knownCuts = new LinkedList<HashSet<CNode> >();
	private LinkedList< HashSet<CNode> > dirtyCutNodes = new LinkedList<HashSet<CNode> >();
	
	private HashSet<CNode> border = new HashSet<CNode>();
	//private LinkedList<CNode> queue = new LinkedList<CNode>();
	private HashSet<CNode> dirtyBorderNodes = new HashSet<CNode>();
	
	private LinkedList<CNode> unvisitedNodes = new LinkedList<CNode>();
	
	private LinkedList<CNode> visitedEvents = new LinkedList<CNode>();
	private HashSet<CNode> cutOffEvents = new HashSet<CNode>();
	private HashSet<CNode> initialCut = new HashSet<CNode>();
	
	public AdaptiveSystemBP(hub.top.adaptiveSystem.AdaptiveSystem as) {
		bp = CNodeSet.fromAdaptiveProcess(as.getAdaptiveProcess());
		
		//initialState = new AdaptiveSystemState(initial);
		//states.add(initialState);
		
		/*
		CNode[] initialCut = new CNode[bp.getCutNodes().size()];
		bp.getCutNodes().toArray(initialCut);
		unvisitedCuts.add(initialCut);
		*/
		initialCut.addAll(bp.getCutNodes()); 
		unvisitedCuts.add(initialCut);
		knownCuts.add(initialCut);
		dirtyCutNodes.add(initialCut);
		
		border.addAll(initialCut);
		dirtyBorderNodes.addAll(initialCut);
		
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

				Collection<CNode[]> cuts = ocletNets[ocletCount].getAllCuts(true);
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
				}*/
				
				ocletCount++;
			}
		}
		
		for (Oclet o : as.getOclets()) {
			if (o.getOrientation() == Orientation.ANTI) {
				oclets[ocletCount] = o;
				ocletNets[ocletCount] = CNodeSet.fromOclet(o);

				Collection<CNode[]> cuts = ocletNets[ocletCount].getAllCuts(true);
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
				}*/

				ocletCount++;
			}
		}
	}
	
	public CNode.MatchingRelation isOcletEnabled(int i, CNode[] cut, HashSet<CNode> dirtyNodes) {
		CNode.MatchingRelation enabled = null;
		
		for (CNode c : cut) {
			for (CNode p : c.getPred())
				if (cutOffEvents.contains(p))
					return null;
		}
		
		boolean maybeDirty = false;
		for (CNode c : ocletNets[i].getCutNodes()) {
			for (CNode d : dirtyNodes) {
				if (d.getLabel().equals(c.getLabel())) {
					maybeDirty = true;
					break;
				}
			}
			if (maybeDirty)
				break;
		}
		
		CNode.MatchingRelation m = new CNode.MatchingRelation();
		if (maybeDirty && CNodeSet.nodesContainedIn(ocletNets[i].getCutNodes(), cut, m)) {
			
			boolean enablesDirty = false;
			for (CNode dirtyNode : dirtyNodes) {
				if (m.containsValue(dirtyNode)) {
					enablesDirty = true;
					break;
				}
			}
			if (enablesDirty)
				enabled = m;
		}
		
		return enabled;
	}
	
	public CNode.MatchingRelation isOcletEnabled(int i, HashSet<CNode> cut, HashSet<CNode> dirtyNodes) {
		CNode[] aCut = new CNode[cut.size()];
		cut.toArray(aCut);
		return isOcletEnabled(i, aCut, dirtyNodes);
	}
	
	public CNode.MatchingRelation[] enabledOclets (HashSet<CNode> cut, HashSet<CNode> dirtyNodes) {
		CNode.MatchingRelation enabled[] = new CNode.MatchingRelation[oclets.length];
		for (int i=0;i<oclets.length;i++) {
			enabled[i] = isOcletEnabled(i, cut, dirtyNodes);
		}
		return enabled;
	}
	
	public void applyOclet (int i, CNode.MatchingRelation applyAt) {
		CNodeSet o = ocletNets[i].instantiate(applyAt);
		
		//System.out.println(bp.toDot());
		
		if (oclets[i].getOrientation() == Orientation.NORMAL) {
			for (CNode c : o.getAllNodesOrdered()) {
				CNode added = bp.addNode(c);
				if (added != c) {
					for (CNode other : o.getAllNodes()) {
						if (other != c)
							other.replacePredecessorNode(c, added);
					}
				}
				if (added.isEvent() && !unvisitedNodes.contains(added)) {
					unvisitedNodes.addLast(added);
					//System.out.println("adding "+added+" from "+oclets[i].getName());
				}
			}
			// update the maximal nodes of the successor state
			bp.getMaxNodes().addAll(o.getMaxNodes());
		}
		else {
			for (CNode c : o.getAllNodes()) {
				Collection<CNode> removeCandidates = bp.get(new Integer(c.hashCode()));
				for (CNode remC : removeCandidates) {
					if (c.equals(remC)) {
						Collection<CNode> removedNodes = bp.removeNode(remC);
						unvisitedNodes.removeAll(removedNodes);
						break;
					}
				}
			}
		}
	}
	
	public void applyAllOclets(CNode.MatchingRelation fireOclets[]) {
		for (int i=0; i< oclets.length;i++) {
			if (fireOclets[i] != null) {
				applyOclet(i,fireOclets[i]);
			}
		}
	}
	
	public boolean equivalentCuts(HashSet<CNode> cut1, HashSet<CNode> cut2) {
		int enabledCutNum_s1[] = new int[oclets.length];
		int enabledCutNum_s2[] = new int[oclets.length];
		
		for (int ocletNum = 0; ocletNum < oclets.length; ocletNum++) {
			int cutNum = 0;
			for (; cutNum < ocletCuts[ocletNum].length; cutNum++) {
				if (CNodeSet.nodesContainedIn(ocletCuts[ocletNum][cutNum], cut1, null))
					break;
			}
			enabledCutNum_s1[ocletNum] = cutNum;
		}
		
		for (int ocletNum = 0; ocletNum < oclets.length; ocletNum++) {
			int cutNum = 0;
			for (; cutNum < ocletCuts[ocletNum].length; cutNum++) {
				if (CNodeSet.nodesContainedIn(ocletCuts[ocletNum][cutNum], cut2, null))
					break;
			}
			enabledCutNum_s2[ocletNum] = cutNum;
		}
/*
		    for (int i=0;i<enabledCutNum_s1.length;i++) System.out.print(oclets[i].getName()+"|");
		    System.out.println();
		    System.out.println(cutToString(cut1));
			for (int i=0;i<enabledCutNum_s1.length;i++) System.out.print(enabledCutNum_s1[i]+"|");
			System.out.println();
			System.out.println(cutToString(cut2));
			for (int i=0;i<enabledCutNum_s2.length;i++) System.out.print(enabledCutNum_s2[i]+"|");
			System.out.println("\n---------");
		*/
		
		for (int i=0;i<enabledCutNum_s1.length;i++)
			if (enabledCutNum_s1[i] != enabledCutNum_s2[i])
				return false;
		
		return true;
	}
	
	public boolean construct () {
		if (unvisitedCuts.isEmpty())
			return false;

		HashSet<CNode> cut = unvisitedCuts.removeFirst();
		HashSet<CNode> cutDirty = dirtyCutNodes.removeFirst();
		CNode.MatchingRelation[] oEnabled = enabledOclets(cut, cutDirty);
		
		/*
		System.out.println("current cut: "+cutToString(cut));
		for (int i=0;i<oclets.length;i++)
			if (oEnabled[i] != null)
				System.out.println("oclet "+oclets[i].getName()+" is enabled");
		*/
		
		applyAllOclets(oEnabled);
		Collection<CNode> events = bp.getEnabledEvents(cut);
		for (CNode e : events) {
			//System.out.println("event "+e.getLabel()+" is enabled");
			HashSet<CNode> succCut = bp.fire(e, cut);
			HashSet<CNode> succDirty = new HashSet<CNode>(succCut);
			succDirty.removeAll(cut);
			
			//System.out.println(cutToString(cut)+" -> "+cutToString(succCut)+", dirty: "+succDirty);
				
			boolean newCut = true;
			for (HashSet<CNode> knownCut : knownCuts) {
				if (equivalentCuts(succCut, knownCut)) {
					newCut = false;
					break;
				}
			}
			
			if (newCut) {
				unvisitedCuts.add(succCut);
				knownCuts.add(succCut);
				dirtyCutNodes.add(succDirty);
			}
		}
		
		return true;
	}
	
	public boolean constructBorder () {
		
		boolean change = false;
		boolean cutOffEvent = false;
		
		if (unvisitedNodes.size() > 0) {
			CNode e = null;
			
			for (CNode eCand : unvisitedNodes) {
				if (border.containsAll(eCand.getPred())) {
					e = eCand;
					break;
				}
			}
			
			if (cutOffEvents.contains(e))
				System.out.println("exploring event after cut-off");
			
			if (e != null) {
				unvisitedNodes.remove(e);
				
				Collection<CNode> ePost = bp.getPostSet(e);
				dirtyBorderNodes.addAll(ePost);
				/*
				for (CNode p : e.getPred()) {
					boolean hasUnvisitedSucc = false;
					for (CNode e2 : unvisitedNodes) {
						if (e2.getPred().contains(p)) {
							hasUnvisitedSucc = true;
							break;
						}
					}
					// remove p from the border only if p has no unvisited
					// successor events left, i.e. if all behavior that
					// relies on p has been explored
					if (!hasUnvisitedSucc)
						border.remove(p);
				}*/
				
				//border.removeAll(e.getPred());
				border.addAll(ePost);
				change = true;
				/*
				HashSet<CNode> postEvents = new HashSet<CNode>(ePost.size());
				for (CNode c : ePost) {
					for (CNode e2 : bp.getPostSet(c)) {
						if (   !postEvents.contains(e2) 
							&& border.containsAll(e2.getPred()))
						{
							queue.addLast(e2);
						}
						postEvents.add(e2);
					}
				}*/
				
				HashSet<CNode> eCut = bp.reachedCut(e);

				/*
				System.out.println("\ncheck cut-off of "+e+" at "+cutToString(eCut));
				System.out.println("vs initial: "+cutToString(initialCut));
				*/
				if (equivalentCuts(eCut, initialCut)) {

					cutOffEvent = true;
				} else {
					
					// retrieve all nodes that might match CNode n via the
					// hash key index of this CNodeSet
					for (CNode other : visitedEvents) {
						HashSet<CNode> otherCut = bp.reachedCut(other);

						//System.out.println("vs "+other+": "+cutToString(otherCut));

						if (otherCut.size() != eCut.size())
							continue;
						if (equivalentCuts(eCut, otherCut)) {
							cutOffEvent = true;
							/*
							System.out.println("cut off: "+e+" "+cutToString(eCut));
							System.out.println("earlier: "+other+" "+cutToString(otherCut));
							*/
							break;
						}
					}
				}
				
				if (cutOffEvent) {
					// do not explore any success of a cut-off event
					HashSet<CNode> future = bp.getAllCausalSuccessors(e);
					unvisitedNodes.removeAll(future);
					cutOffEvents.add(e);
					cutOffEvents.addAll(future);
//					System.out.println("removing "+cutToString(future));
				}
				
				visitedEvents.add(e);
			} else {
				System.err.println("no fitting extend nodes found in: "+cutToString(unvisitedNodes));
				System.err.println("current border: "+cutToString(border));
			}
		}
		
		if (!cutOffEvent) {
			for (int i=0;i<oclets.length;i++) {
				HashSet<CNode> ocletCut = ocletNets[i].getCutNodes();
				
				LinkedList<CNode[]> matchingCuts = bp.findEnablingCuts(ocletCut, dirtyBorderNodes, border);
				
				if (matchingCuts == null)
					continue;
				
				for (CNode[] cut : matchingCuts) {
					CNode.MatchingRelation oEnabled = isOcletEnabled(i, cut, dirtyBorderNodes);
					if (oEnabled != null) {
						applyOclet(i, oEnabled);
						change = true;
					}
				}
			}
		}
		/*
		CNode.MatchingRelation[] oEnabled = enabledOclets(border, dirtyBorderNodes);
		applyAllOclets(oEnabled);
		for (int i=0;i < oEnabled.length; i++)
			if (oEnabled[i] != null) {
				change = true;
				break;
			}
		*/
		dirtyBorderNodes.clear();
		
		return change;
	}
	
	public static String cutToString(CNode[] cut) {
		String s = "[";
		for (int i=0;i<cut.length;i++) {
			s += cut[i]+" ";
			if (i < cut.length-1)
				s += ",";
		}
		return s+"]";
	}
	
	public static String cutToString(Collection<CNode> cut) {
		String s = "[";
		int i=0;
		for (CNode c : cut) {
			s += c+" ";
			if (i < cut.size()-1)
				s += ",";
			i++;
		}
		return s+"]";
	}
	

	public String toDot () {
		//System.out.println("number of cuts: "+this.knownCuts.size());
		//System.out.println("cut-off events:"+cutToString(cutOffEvents));
		return bp.toDot(cutOffEvents);
	}
}
