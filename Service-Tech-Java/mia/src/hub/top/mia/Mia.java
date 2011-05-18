package hub.top.mia;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.HashSet;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.RuntimeErrorException;

public class Mia {
	public static final String PLACE = "[a-z]*[0-9]*";
	private Place[] placesOld, placesNew;
	private ArrayList<SafeMarking> globalStates;
	private Pattern pattern;
	private StateTree tree;
	
	public Mia() {
		globalStates = new ArrayList<SafeMarking>();
		pattern = Pattern.compile(PLACE, Pattern.CASE_INSENSITIVE);
		tree = new StateTree();
	}

	/**
	 * converts markings from string representations to bitset representation
	 * 
	 * @param marking
	 * @param places
	 */
	public BitSet parseMarking(String marking, Place[] places) {
		Matcher matcher = pattern.matcher(marking);
		BitSet result = new BitSet();

		while (matcher.find()) {
			String token = matcher.group();

			for (int i = 0; i < places.length; i++) {
				if (token.equals(places[i].getName())) {
					result.set(i);
				}
			}
		}

		return result;
	}

	public void test() {
		ArrayList<SafeMarking> global_states = new ArrayList<SafeMarking>();

		BitSet m1_old = new BitSet();
		m1_old.set(0);
		m1_old.set(1);
		m1_old.set(2);

		BitSet m1_new = new BitSet();
		m1_new.set(0);
		m1_new.set(1);
		m1_new.set(2);

		BitSet m2_old = new BitSet();
		m2_old.set(0);
		m2_old.set(1);
		m2_old.set(3);

		BitSet m2_new = new BitSet();
		m2_new.set(0);
		m2_new.set(1);
		m2_new.set(3);

		BitSet m3_old = new BitSet();
		m3_old.set(1);
		m3_old.set(2);
		m3_old.set(4);

		BitSet m3_new = new BitSet();
		m3_new.set(1);
		m3_new.set(2);
		m3_new.set(4);

		BitSet m4_old = new BitSet();
		m4_old.set(4);
		m4_old.set(6);
		m4_old.set(7);

		BitSet m4_new = new BitSet();
		m4_new.set(4);
		m4_new.set(6);
		m4_new.set(7);

		global_states.add(new SafeMarking(m1_old, m1_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m2_old, m2_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m3_old, m3_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m4_old, m4_new, SafeMarking.LEAF));

		StateTree tree = new StateTree();
		
		System.out.println("************* Building tree ***********");
		
		tree.buildTree(global_states);
		
		System.out.println("************* Traversing tree ***********");
		tree.traverseTree();
	}

	public void run(String strOld, String strNew) {
		String s;
		try {
			// read owfn nets
			PetriNet n_Old = PetriNetIO.readNetFromFile(strOld);
			PetriNet n_New = PetriNetIO.readNetFromFile(strNew);
			
			PlaceComparator comp = new PlaceComparator();
			
			// get places, HashSet does not guarantee that the iteration order
			// of places remains constant over time
			HashSet<Place> hashPlacesOld = n_Old.getPlaces();
			HashSet<Place> hashPlacesNew = n_New.getPlaces();
			
			placesOld = new Place[hashPlacesOld.size()];
			placesNew = new Place[hashPlacesNew.size()];
			
			// convert hash sets to plain arrays
			hashPlacesOld.toArray(placesOld);
			hashPlacesNew.toArray(placesNew);
			
			Arrays.sort(placesOld, comp);
			Arrays.sort(placesNew, comp);	
	
			// exec mia to compute global jumper transitions
			Process process = Runtime.getRuntime().exec(
					"mia " + strOld + " " + strNew);

			BufferedReader stdInput = new BufferedReader(new InputStreamReader(
					process.getInputStream()));

			BufferedReader stdError = new BufferedReader(new InputStreamReader(
					process.getErrorStream()));

			// read the input from the command
			// gives global jumper transitions
			while ((s = stdInput.readLine()) != null) {
				// ignore first character
				String m_old, m_new;
				int begin, end;

				// parse global jumper transitions
				// global jumper transitions look like [m_old] ->
				// [m_new]
				
				// get old net marking
				begin = end = 1;
				while (s.charAt(end) != ']') {
					end++;
				}
				m_old = s.substring(begin, end);

				// skip in between junk
				while (s.charAt(end) != '[') {
					end++;
				}

				// get new net marking
				begin = end + 1;
				while (s.charAt(end) != ']') {
					end++;
				}
				m_new = s.substring(begin, end);

				// convert markings from string representation to bitset 
				BitSet b_MOld = parseMarking(m_old, placesOld);
				BitSet b_MNew = parseMarking(m_new, placesNew);

				globalStates.add(new SafeMarking(b_MOld, b_MNew,
						SafeMarking.LEAF));
			}

			// print global states
			for (SafeMarking element : globalStates) {
				System.out.println(element);
			}
			
			tree.buildTree(globalStates);
			tree.traverseTree();
			
			// read any errors from the attempted command - not of interest
			// at the moment
			while ((s = stdError.readLine()) != null) {
				// [TODO:] handle eventual mia errors
			}

		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void main(String args[]) {
		if (args.length != 2) {
			// usage: mia n_old.owfn n_new.owfn
			System.out.println("Usage: mia n_old.owfn n_new.owfn");
		} else {
			new Mia().run(args[0], args[1]);
			//new Mia().test();
		}
	}
}
