package viptool.algorithm.postprocessing.pnetiplp;

import hub.top.petrinet.Arc;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;

import lpsolve.LpSolveException;
import viptool.algorithm.postprocessing.pnetip.Exhuminator;

/**
 * This class reads an pnml file, search implicit given places and writes an
 * output file without this implicit given places. To find the implicit places
 * an inequalitiy system is used.
 * 
 * @author thomas
 */
public class ExtendedExhuminator {
  
  public static int reduce(PetriNet net) {
    ExtendedExhuminator ex = new ExtendedExhuminator(net);
    ex.loadNet();
    ex.sortPlaces();
    return ex.deletePlaces();
  }
  

	/**
	 * Flag to activate debug output
	 */
	private boolean					DEBUG				= false;

	/**
	 * ArrayList for transitions
	 */
	private ArrayList<String>				transitions_		= new ArrayList<String>();

	/**
	 * ArrayList for places
	 */
	private ArrayList<String>				places_				= new ArrayList<String>();

	/**
	 * ArrayList for arcs
	 */
	private ArrayList<int[]>				arcs_				= new ArrayList<int[]>();

	/**
	 * Vector with all placeDataFrames
	 */
	private Vector<PlaceDataFrame>	placeDataFrames_	= new Vector<PlaceDataFrame>();

	/**
	 * source node of the net
	 */
	private PetriNet					net;
	
	private int removed = 0;

	/**
	 * This method runs the whole delete process.
	 * 
	 * @param input
	 *            pnml input file
	 * @param output
	 *            file for output
	 */
	private ExtendedExhuminator(PetriNet net) {

		// run simple algorithm first
		removed = Exhuminator.reduce(net);
		
		this.net = net;
	}

	/**
	 * This method loads all necessary information from the given pnml file
	 * 
	 * @param path
	 *            absolut path to the used pnml file
	 */
	private void loadNet() {

	  for (Transition t : net.getTransitions()) {
      transitions_.add(t.getUniqueIdentifier());
    }
		/*
		// get transition ids and generate a transition index
		for (int k = 0; k < nodeList_.getLength(); k++) {
			if (nodeList_.item(k).getNodeName().equals("transition")) {
				transitions_.add(((Element) nodeList_.item(k))
						.getAttribute("id"));
			}
		}
		*/

	  for (Place p : net.getPlaces()) {
	    String placeId = p.getUniqueIdentifier();
	    places_.add(placeId);
	    
	    int initialMarkingValue = p.getTokens();
	    
      // build a place data frame for the found place
      PlaceDataFrame pl = new PlaceDataFrame(placeId,
          places_.size() - 1, initialMarkingValue, transitions_
              .size());

      // set score to initial marking
      pl.setScore(initialMarkingValue);

      String name = "";
      if (DEBUG) {
        name = p.getUniqueIdentifier();
      }
      
      // set visible name
      pl.setName(name);

      placeDataFrames_.add(pl);

      // generate an array for all outgoing and incoming arc weights
      // and the inital marking
      int[] arcweights = new int[1 + 2 * transitions_.size()];
      arcweights[0] = initialMarkingValue;
      arcs_.add(arcweights);
	  }
		// get place informations
		/*
		for (int k = 0; k < nodeList_.getLength(); k++) {
			if (nodeList_.item(k).getNodeName().equals("place")) {
				Element place = (Element) nodeList_.item(k);

				String placeId = place.getAttribute("id");

				// store place ids and generate a place index
				places_.add(placeId);

				Element initialMarking = (Element) place.getElementsByTagName(
						"initialMarking").item(0);
				Element token = (Element) initialMarking.getElementsByTagName(
						"token").item(0);

				// get visible name for debug
				String name = "";
				
				if(DEBUG){
					NodeList nodes = place.getChildNodes();
					for (int i = 0; i < nodes.getLength(); i++) {
						if (nodes.item(i).getNodeName().equals("name")) {
							Node node = nodes.item(i);
							NodeList childs = node.getChildNodes();
							for (int j = 0; j < childs.getLength(); j++) {
								if (childs.item(j).getNodeName().equals("value")) {
									name = childs.item(i).getTextContent().trim();
									break;
								}
							}
							break;
						}
					}
				}

				int initialMarkingValue;

				if (token != null) {
					initialMarkingValue = Integer.parseInt(token
							.getElementsByTagName("value").item(0)
							.getTextContent().trim());
				} else {
					initialMarkingValue = 0;
				}

				// build a place data frame for the found place
				PlaceDataFrame pl = new PlaceDataFrame(placeId,
						places_.size() - 1, initialMarkingValue, transitions_
								.size());

				// set score to initial marking
				pl.setScore(initialMarkingValue);

				// set visible name
				pl.setName(name);

				placeDataFrames_.add(pl);

				// generate an array for all outgoing and incoming arc weights
				// and the inital marking
				int[] arcweights = new int[1 + 2 * transitions_.size()];
				arcweights[0] = initialMarkingValue;
				arcs_.add(arcweights);
			}
		}
		*/

		// read arcs
	  for (Arc arc : net.getArcs()) {
	    String sourceId = arc.getSource().getUniqueIdentifier();
	    String targetId = arc.getTarget().getUniqueIdentifier();
	    
	    int weight = 1; // assume arc-weight 1
	    
      // update place data frames with outgoing arc weights
	    if (arc.getSource() instanceof Place) {
        for (int q = 0; q < placeDataFrames_.size(); q++) {
          PlaceDataFrame pl = (PlaceDataFrame) placeDataFrames_.get(q);

          if (pl.getPlaceId().equals(sourceId)) {
            pl.setOutgoingArcWeight(transitions_.indexOf(targetId), weight);
          }
        }
      }
	    // update place data frames with incoming arc weights
	    if (arc.getTarget() instanceof Place) {
        for (int q = 0; q < placeDataFrames_.size(); q++) {
          PlaceDataFrame pl = (PlaceDataFrame) placeDataFrames_.get(q);
          if (pl.getPlaceId().equals(targetId)) {
            pl.setIncomingArcWeight(transitions_.indexOf(sourceId), weight);
            // update score
            pl.setScore(pl.getScore() + weight);
          }
        }    
	    }
	    
      if (places_.contains(sourceId)) {
        ((int[]) arcs_.get(places_.indexOf(sourceId)))[transitions_
            .indexOf(targetId) + 1] = weight;
      } else {
        ((int[]) arcs_.get(places_.indexOf(targetId)))[transitions_
            .size()
            + transitions_.indexOf(sourceId) + 1] = weight;
      }
	  }
	  /*
		for (int k = 0; k < nodeList_.getLength(); k++) {
			if (nodeList_.item(k).getNodeName().equals("arc")) {
				Element arc = (Element) nodeList_.item(k);

				String sourceId = arc.getAttribute("source");

				String targetId = arc.getAttribute("target");

				Element inscription = (Element) arc.getElementsByTagName(
						"inscription").item(0);
				int weight = Integer.parseInt(inscription.getElementsByTagName(
						"value").item(0).getTextContent().trim());

				// update place data frames with outgoing arc weights
				for (int a = 0; a < nodeList_.getLength(); a++) {
					Node node = nodeList_.item(a);
					if (node.getNodeName().equals("place")
							&& ((Element) node).getAttribute("id").equals(
									sourceId)) {
						for (int q = 0; q < placeDataFrames_.size(); q++) {
							PlaceDataFrame pl = (PlaceDataFrame) placeDataFrames_
									.get(q);

							if (pl.getPlaceId().equals(sourceId)) {
								pl.setOutgoingArcWeight(transitions_
										.indexOf(targetId), weight);
							}
						}
					}
				}

				// update place data frames with incoming arc weights
				for (int a = 0; a < nodeList_.getLength(); a++) {
					Node node = nodeList_.item(a);
					if (node.getNodeName().equals("place")
							&& ((Element) node).getAttribute("id").equals(
									targetId)) {
						for (int q = 0; q < placeDataFrames_.size(); q++) {
							PlaceDataFrame pl = (PlaceDataFrame) placeDataFrames_
									.get(q);
							if (pl.getPlaceId().equals(targetId)) {
								pl.setIncomingArcWeight(transitions_
										.indexOf(sourceId), weight);
								// update score
								pl.setScore(pl.getScore() + weight);
							}
						}
					}
				}

				if (places_.contains(sourceId)) {
					((int[]) arcs_.get(places_.indexOf(sourceId)))[transitions_
							.indexOf(targetId) + 1] = weight;
				} else {
					((int[]) arcs_.get(places_.indexOf(targetId)))[transitions_
							.size()
							+ transitions_.indexOf(sourceId) + 1] = weight;
				}
			}
		}
		*/

	}

	/**
	 * This method sorts the places by score.
	 */
	private void sortPlaces() {
		// build an array out of the vector to use the java internal sort method
		Object[] array = placeDataFrames_.toArray();
		Arrays.sort(array);
		// and write the sorted places back to a new vector
		placeDataFrames_ = new Vector<PlaceDataFrame>();
		for (int i = 0; i < array.length; i++) {
			PlaceDataFrame place = (PlaceDataFrame) array[i];
			
			if(DEBUG)
				System.out.println(place.getName() + ": " + place.getScore());
			
			placeDataFrames_.add(place);
		}
	}

	/**
	 * This method deletes the found implicit given places
	 */
	private int deletePlaces() {
		HashSet<Integer> implicitPlaces = new HashSet<Integer>();

		//TODO: was macht z????
		int z = 0;

		for (int i = 0; i < placeDataFrames_.size(); i++) {
			
			 if (i == z) { 
				 i = 0; 
				 z = 0; 
			}
			
			// places with higher score than current place
			Vector<PlaceDataFrame> highScore = new Vector<PlaceDataFrame>();
			// places with lower score than current place
			Vector<PlaceDataFrame> lowScore = new Vector<PlaceDataFrame>();

			for (int j = 0; j < i; j++) {
				highScore.add(placeDataFrames_.get(j));
			}

			for (int j = i + 1; j < placeDataFrames_.size(); j++) {
				lowScore.add(placeDataFrames_.get(j));
			}

			// current place
			PlaceDataFrame p = (PlaceDataFrame) placeDataFrames_.get(i);

			// test if inequality system can be solved

			// build inequality system
			InequaltitySystem u = new InequaltitySystem(highScore, lowScore, p);

			// result flag
			int n = -1;
			try {
				n = u.execute();
			} catch (LpSolveException e) {
				e.printStackTrace();
			}

			// n == 0 => system can be solved => place can be removed
			if (n == 0) {
				// add place to the set of implicit given places
				implicitPlaces.add(new Integer(p.getIndex()));
				// remove place for further tests of other places
				placeDataFrames_.remove(i);
				// update index
				i = i - 1; 
				z = 1;
			}
		}

		Iterator<Integer> it = implicitPlaces.iterator();
		ArrayList<Place> dasKommtWeg = new ArrayList<Place>();

		while (it.hasNext()) {
			int i = ((Integer) it.next()).intValue();

			if (i < places_.size()) {
				String id = (String) places_.get(i);

	      boolean found = false;
	      for (Place p : net.getPlaces()) {
	        if (p.getUniqueIdentifier().equals(id)) {
	          dasKommtWeg.add(p);
	          found = true;
	          break;
	        }
	      }
	      if (!found) {
	        System.err.println("Error: place "+id+" not found in the net");
	      }
			}

		}
		
		
		
    // remove found implicit places
    Iterator<Place> it2 = dasKommtWeg.iterator();
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
