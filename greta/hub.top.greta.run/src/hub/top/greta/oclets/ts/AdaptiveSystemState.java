package hub.top.greta.oclets.ts;

import hub.top.greta.oclets.canonical.CNode;
import hub.top.greta.oclets.canonical.CNodeSet;

import java.util.Collection;
import java.util.HashSet;

public class AdaptiveSystemState {

	public HashSet<CNode> cut;
	public HashSet<CNode> knownFuture;
	
	public AdaptiveSystemState() {
		cut = new HashSet<CNode>();
		knownFuture = new HashSet<CNode>();
	}
	
	public AdaptiveSystemState(CNodeSet set) {
		// store the maximal nodes only, assuming a
		Collection<CNode> maxNodes = set.getMaxNodes();
		cut = new HashSet<CNode>(maxNodes.size());
		cut.addAll(maxNodes);	
		
		knownFuture = new HashSet<CNode>();
	}
}
