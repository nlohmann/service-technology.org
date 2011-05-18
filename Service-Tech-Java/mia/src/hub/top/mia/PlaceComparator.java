package hub.top.mia;

import hub.top.petrinet.Place;

import java.util.Comparator;

/**
 * compare places based on lexicographical order
 * @author darsinte
 *
 */
public class PlaceComparator implements Comparator<Place>{

	@Override
	public int compare(Place p1, Place p2) {
		return p1.getName().compareTo(p2.getName());
	}

}
