package hub.top.mia;

import java.util.BitSet;
import java.util.HashSet;

public class SafeMarking {
	private int type, depth;
	private BitSet m_Old, m_New;
	private HashSet<SafeMarking> leafs, fathers, children;
	
	public static final int INNER = 0;
	public static final int LEAF = 1;
	
	public SafeMarking() {
		setM_Old(new BitSet());
		setM_New(new BitSet());
		type = INNER;
		setDepth(0);
		
		// leaf set
		leafs = new HashSet<SafeMarking>();
		fathers = new HashSet<SafeMarking>();
		
		// direct successors
		children = new HashSet<SafeMarking>();
	}
	public SafeMarking(BitSet m_Old, BitSet m_New, int type) {
		this.m_Old = m_Old;
		this.m_New = m_New;
		this.type = type;
		setDepth(0);
		
		leafs = new HashSet<SafeMarking>();
		fathers = new HashSet<SafeMarking>();
		children = new HashSet<SafeMarking>();
	}
	
	public void setType(int type) {
		this.type = type;
	}
	public int getType() {
		return type;
	}

	public void setM_Old(BitSet m_Old) {
		this.m_Old = m_Old;
	}

	public BitSet getM_Old() {
		return m_Old;
	}

	public void setM_New(BitSet m_New) {
		this.m_New = m_New;
	}

	public BitSet getM_New() {
		return m_New;
	}
	
	public void addFather(SafeMarking father) {
		getFathers().add(father);
	}
	
	public void addLeaf(SafeMarking leaf) {
		getLeafs().add(leaf);
	}
	
	public void addChild(SafeMarking child) {
		children.add(child);
	}
	
	public SafeMarking intersect(SafeMarking other) {
		BitSet new_m_Old = (BitSet) m_Old.clone();
		BitSet new_m_New = (BitSet) m_New.clone();
		
		new_m_Old.and(other.getM_Old());
		new_m_New.and(other.getM_New()); 
		
		SafeMarking result = new SafeMarking(new_m_Old, new_m_New, SafeMarking.INNER);
		return result;
	}
	
	public boolean equals(SafeMarking other) {
		return (m_Old.equals(other.getM_Old()) && m_New.equals(other.getM_New())); 
	}
	
	public boolean contains(SafeMarking other) {
		SafeMarking intersection = intersect(other);
		return intersection.equals(other);
	}
	
	public String toString() {
		return "[" + m_Old + " , " + m_New + "]";
	}

	public HashSet<SafeMarking> getLeafs() {
		return leafs;
	}

	public HashSet<SafeMarking> getFathers() {
		return fathers;
	}
	
	public HashSet<SafeMarking> getChildren() {
		return children;
	}
	public void setDepth(int depth) {
		this.depth = depth;
	}
	public int getDepth() {
		return depth;
	}
}
