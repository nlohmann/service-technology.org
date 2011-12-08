package viptool.algorithm.postprocessing.pnetip;

import hub.top.petrinet.Arc;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;

public class Exhuminator {

  public static int reduce(PetriNet net) {
    Exhuminator ex = new Exhuminator(net);
    ex.loadNet();
    ex.findPlaces();
    return ex.deletePlaces();
  }
  
	private int		kill			= 0;
  private int removed = 0;
  
	/**
	 * Arraylist for transition IDs
	 */
	private ArrayList<String>		transitionen	= new ArrayList<String>();

	/**
	 * Arraylist for place IDs
	 */
	private ArrayList<String>		places			= new ArrayList<String>();

	/**
	 * Arraylist for int[] with: 
	 * int[0]: token count of initial marking
	 * int[i], i>0: arc weight to transitions (0 if there is no arc)
	 */
	private ArrayList<int[]>		arcs			= new ArrayList<int[]>();

	/**
	 * Hashset for deleteable places
	 */
	private HashSet<Integer> implicitPlaces;

	/**
	 * source node of net
	 */
	private PetriNet net = null;
	
	private Exhuminator(PetriNet net) {
	  this.net = net; 
	}

	private void loadNet() {
		try {

		  for (Transition t : net.getTransitions()) {
		    transitionen.add(t.getUniqueIdentifier());
		  }

		  for (Place p : net.getPlaces()) {
		    places.add(p.getUniqueIdentifier());
		    int i = p.getTokens();
		    
        // construct int[] for arc weights and initial marking
        int[] kantengewichte = new int[1 + 2 * transitionen.size()];
        kantengewichte[0] = i;
        arcs.add(kantengewichte);
		  }

		  /*
			// Places
			for (int k = 0; k < nodeList.getLength(); k++) {
				if (nodeList.item(k).getNodeName().equals("place")) {
					places.add(((Element) nodeList.item(k)).getAttribute("id"));
					// count of inital tokens (XML element)
					Element e = (Element) ((Element) ((Element) nodeList
							.item(k)).getElementsByTagName("initialMarking")
							.item(0)).getElementsByTagName("token").item(0);
					// count of initial tokens
					int i = 0;
					if (e != null) {
						i = Integer.parseInt(e.getElementsByTagName("value")
								.item(0).getTextContent());
					}
					// construct int[] for arc weights and initial marking
					int[] kantengewichte = new int[1 + 2 * transitionen.size()];
					kantengewichte[0] = i;
					arcs.add(kantengewichte);
				}
			}
		  */
		  
		  for (Arc a : net.getArcs()) {
		    String sourceId = a.getSource().getUniqueIdentifier();
		    String targetId = a.getTarget().getUniqueIdentifier();
		    int w = 1; // weight 1
		    
        // add arc
        if (places.contains(sourceId)) {
          // get arc weights for outgoing arcs
          ((int[]) arcs.get(places.indexOf(sourceId)))[transitionen
              .indexOf(targetId) + 1] = w;
        } else {
          // get arc weights for incoming arcs
          ((int[]) arcs.get(places.indexOf(targetId)))[transitionen
              .size()
              + transitionen.indexOf(sourceId) + 1] = w;
        }
		  }
		  
			// Arcs
		  /*
			for (int k = 0; k < nodeList.getLength(); k++) {
				if (nodeList.item(k).getNodeName().equals("arc")) {
					String sourceId = ((Element) nodeList.item(k))
							.getAttribute("source");
					String targetId = ((Element) nodeList.item(k))
							.getAttribute("target");
					// get arc weight
					int w = Integer.parseInt(((Element) ((Element) nodeList
							.item(k)).getElementsByTagName("inscription").item(
							0)).getElementsByTagName("value").item(0)
							.getTextContent().trim());

					// add arc
					if (places.contains(sourceId)) {
						// get arc weights for outgoing arcs
						((int[]) arcs.get(places.indexOf(sourceId)))[transitionen
								.indexOf(targetId) + 1] = w;
					} else {
						// get arc weights for incoming arcs
						((int[]) arcs.get(places.indexOf(targetId)))[transitionen
								.size()
								+ transitionen.indexOf(sourceId) + 1] = w;
					}
				}
			}
			*/

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void findPlaces(){

		implicitPlaces = new HashSet<Integer>();

		for (int i = 0; i < arcs.size(); i++) {
			// get initial marking and arc weights for place i
			int[] place = (int[]) arcs.get(i);
			// is place deleteable
			boolean deletable = true;

			for (int j = 1; j <= transitionen.size(); j++) {
				// test if place have outgoing arcs
				if (place[j] > 0)
					deletable = false;
			}

			if (deletable) {
				// place has only incoming arcs => deleteable
				implicitPlaces.add(new Integer(i));
			} else {
				// second test if place is deleteable
				boolean deletable2 = false;

				for (int j = 0; j < arcs.size(); j++) {
					// test all places
					if (i != j && !deletable2) {
						// other place
						deletable2 = true;
						// get initial marking and arc weights for place j
						int[] place2 = (int[]) arcs.get(j);
						// iterate over deleteable places
						Iterator<Integer> it = implicitPlaces.iterator();
						while (it.hasNext()) {
							// if place is already marked as deleteable do nothing
							if (((Integer) it.next()).intValue() == j) {
								deletable2 = false;
							}
						}
						// initial marking is lower than initial marking of other place
						if (place[0] < place2[0])
							deletable2 = false;

						for (int k = 1; k <= transitionen.size(); k++) {
							// arc weight of outgoing arc is bigger than weight of other outgoing arc
							if (place[k] > place2[k])
								deletable2 = false;
						}

						for (int l = transitionen.size() + 1; l <= 2 * transitionen
								.size(); l++) 
						{
							// arc weight from incoming arc smaller than arc weight of other incoming arc 
							if (place[l] < place2[l])
								deletable2 = false;
						}

						if (deletable2) {
							// place is deleteable
							implicitPlaces.add(new Integer(i));
						}
					}
				}

			}
		}
	}
	
	private int deletePlaces(){
		Iterator<Integer> it = implicitPlaces.iterator();
		ArrayList<Place> implicitPlacesNodes = new ArrayList<Place>();

		while (it.hasNext()) {
			int nodenr = ((Integer) it.next()).intValue();
			String id = (String) places.get(nodenr);

			boolean found = false;
			for (Place p : net.getPlaces()) {
			  if (p.getUniqueIdentifier().equals(id)) {
			    implicitPlacesNodes.add(p);
			    found = true;
			  }
			}
			if (!found) {
			  System.err.println("Error: place "+id+" not found in the net");
			}
			
			/*
			for (int k = 0; k < nodeList.getLength(); k++) {
				if (nodeList.item(k).getNodeName().equals("place")) {
					String s = ((Element) nodeList.item(k)).getAttribute("id");
					if (s.equals(id))
						implicitPlacesNodes.add((Node) nodeList.item(k));
				}
				if (nodeList.item(k).getNodeName().equals("arc")) {
					String s = ((Element) nodeList.item(k))
							.getAttribute("source");
					if (s.equals(id))
						implicitPlacesNodes.add((Node) nodeList.item(k));
				}
				if (nodeList.item(k).getNodeName().equals("arc")) {
					String s = ((Element) nodeList.item(k))
							.getAttribute("target");
					if (s.equals(id))
						implicitPlacesNodes.add((Node) nodeList.item(k));
				}
			}
			*/
			kill = kill + 1;
		}

		// remove found implicit places
		Iterator<Place> it2 = implicitPlacesNodes.iterator();
		while (it2.hasNext()) {
		  Place p = it2.next();
		  
		  boolean uniquePre = false;
		  for (Transition t : p.getPreSet()) {
		    if (t.getOutgoing().size() == 1) uniquePre = true;
		  }
      boolean uniquePost = false;
      for (Transition t : p.getPostSet()) {
        if (t.getIncoming().size() == 1) uniquePost = true;
      }
      
      if (!uniquePre && !uniquePost) {
        System.out.println("remove "+p);
        net.removePlace(p);
        removed++;
      }
		}
		return removed;
	}
	
}
