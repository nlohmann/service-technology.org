package hub.top.mia;

import java.util.Vector;

public class Cone {
	private Vector<Marking> cone;
	
	public Cone() {
		cone = new Vector<Marking>();
	}
	
	public void addMarking(Marking element) {
		cone.add(element);
	}
	
	public void removeMarking(Marking element) {
		cone.remove(element);
	}
	
	public Marking getElement(Marking element) {
		int index = cone.indexOf(element);
		if (index == -1) {
			return null;
		}
		
		return cone.get(index);
	}
	
	public void generateCone() {
		//TODO: implement generate cone
	}
}
