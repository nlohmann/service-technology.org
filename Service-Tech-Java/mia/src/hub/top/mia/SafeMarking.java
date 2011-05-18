package hub.top.mia;

import java.util.BitSet;
import java.util.HashSet;

public class SafeMarking implements Cloneable{
	private int type, depth;
	private boolean decomposed;
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
		if (!fathers.contains(father) && !equals(father)) {
			//System.out.println("Link between: " + toString() + ": " + father);
			fathers.add(father);
		}
	}
	
	public void addLeaf(SafeMarking leaf) {
		if (!leafs.contains(leaf)) {
			leafs.add(leaf);
		}
	}
	
	public void addChild(SafeMarking child) {
		if (!children.contains(child) && !equals(child)) {
			children.add(child);
		}
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

	public boolean isEmpty() {
		return m_Old.isEmpty() && m_New.isEmpty();
	}
	
	public void printLeafs() {
		System.out.println("Leafs of node: " + toString());
		for(SafeMarking leaf : leafs) {
			System.out.print(leaf + " ");
		}
		System.out.println();	
	}
	
	/**
	 * compute difference between cluster and leafset 
	 * end result is subcone
	 * @return
	 */
	public HashSet<SafeMarking> getSubcone() {
		HashSet<SafeMarking> subcone = new HashSet<SafeMarking>();
		
		for (SafeMarking leaf : leafs) {
			BitSet common_m_Old = (BitSet) leaf.getM_Old().clone();
			BitSet common_m_New = (BitSet) leaf.getM_New().clone();
			
			common_m_Old.xor(m_Old);
			common_m_New.xor(m_New);
			
			SafeMarking element = new SafeMarking(common_m_Old, common_m_New, SafeMarking.INNER);
			subcone.add(element);
		}
		
		/*
		System.out.println("Subcone for " + toString());
		for (SafeMarking element : subcone) {
			System.out.println(element);
		} */
		
		return subcone;
	}
	
	public boolean isEquivalent(SafeMarking other) {
		HashSet<SafeMarking> otherLeafs = other.getLeafs();
		
		if (leafs.size() != otherLeafs.size()) {
			return false;
		}
		
		for (SafeMarking leaf : leafs) {
			if (!otherLeafs.contains(leaf))
				return false;
		}
		
		return true;
	}
	
	public void mergeLeafs(SafeMarking other) {		
		if (equals(other)) {
			return;
		}
		
		//System.out.println("Merging leafs of " + toString() + " and " + other);
		HashSet<SafeMarking> otherLeafs = other.getLeafs();
		for (SafeMarking otherLeaf : otherLeafs) {
			addLeaf(otherLeaf);
		}
	}
	
	public void setDecomposed(boolean decomposed) {
		this.decomposed = decomposed;
	}
	
	public boolean isDecomposed() {
		return decomposed;
	}
	
	public Object clone() {
		try {
			return super.clone();
		} catch (CloneNotSupportedException e) {
			e.printStackTrace();
			return this;
		}
	}
}
