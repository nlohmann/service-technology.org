package hub.top.greta.oclets.canonical;

import java.util.HashSet;

public class GraphNode {

	public String label;
	public boolean event = false;
	
	public boolean marked = false;
	
	HashSet<GraphNode> pred = new HashSet<GraphNode>();
	HashSet<GraphNode> succ = new HashSet<GraphNode>();
	
	public GraphNode (String label, boolean isEvent, boolean isMarked) {
		this.label = label;
		this.event = isEvent;
		this.marked = isMarked;
	}
	
	public void addSucc(GraphNode n) {
		this.succ.add(n);
		n.pred.add(this);
	}
	
	public void addPred(GraphNode n) {
		this.pred.add(n);
		n.succ.add(this);
	}
}
