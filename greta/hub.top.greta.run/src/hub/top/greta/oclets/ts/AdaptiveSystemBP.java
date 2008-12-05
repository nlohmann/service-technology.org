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
	
	public AdaptiveSystemBP(hub.top.adaptiveSystem.AdaptiveSystem as) {
		bp = CNodeSet.fromAdaptiveProcess(as.getAdaptiveProcess());
		
		//initialState = new AdaptiveSystemState(initial);
		//states.add(initialState);
		
		/*
		CNode[] initialCut = new CNode[bp.getCutNodes().size()];
		bp.getCutNodes().toArray(initialCut);
		unvisitedCuts.add(initialCut);
		*/
		unvisitedCuts.add(bp.getCutNodes());
		knownCuts.add(bp.getCutNodes());
		
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
	
	public CNode.MatchingRelation[] enabledOclets (HashSet<CNode> cut) {
		CNode.MatchingRelation enabled[] = new CNode.MatchingRelation[oclets.length];
		for (int i=0;i<oclets.length;i++) {
			CNode.MatchingRelation m = new CNode.MatchingRelation();
			if (CNodeSet.nodesContainedIn(ocletNets[i].getCutNodes(), cut, m))
				enabled[i] = m;
			else
				enabled[i] = null;
		}
		return enabled;
	}
	
	public void applyAllOclets(CNode.MatchingRelation fireOclets[]) {
		for (int i=0; i< oclets.length;i++) {
			if (fireOclets[i] != null) {
				CNodeSet o = ocletNets[i].instantiate(fireOclets[i]);
				
				if (oclets[i].getOrientation() == Orientation.NORMAL) {
					for (CNode c : o.getAllNodes()) {
						CNode added = bp.addNode(c);
						if (added != c) {
							for (CNode other : o.getAllNodes()) {
								if (other != c)
									other.replacePredecessorNode(c, added);
							}
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
								bp.removeNode(remC);
								break;
							}
						}
					}
				}
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
	
	public boolean construct () {
		if (unvisitedCuts.isEmpty())
			return false;

		HashSet<CNode> cut = unvisitedCuts.removeFirst();
		CNode.MatchingRelation[] oEnabled = enabledOclets(cut);
		
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
			}
		}
		
		return true;
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
	
	public static String cutToString(HashSet<CNode> cut) {
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
		System.out.println("number of cuts: "+this.knownCuts.size());
		return bp.toDot();
	}
}
