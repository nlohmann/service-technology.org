/*****************************************************************************\
 * SAM (Scenario-based Analysis Methods and tools) [AGPL3.0]
 * Copyright (c) 2011.
 *     AIS Group, Eindhoven University of Technology,
 *     service-technology.org,
 *     Prof. Harel research lab at the Weizmann Institute of Science   
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package org.st.scenarios.clsc.export;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

import org.cpntools.accesscpn.model.Instance;
import org.cpntools.accesscpn.model.Page;
import org.cpntools.accesscpn.model.PetriNet;
import org.cpntools.accesscpn.model.Place;
import org.cpntools.accesscpn.model.RefPlace;
import org.cpntools.accesscpn.model.Transition;
import org.cpntools.accesscpn.model.util.BuildCPNUtil;
import org.st.scenarios.clsc.Chart;
import org.st.scenarios.clsc.Dependency;
import org.st.scenarios.clsc.Event;
import org.st.scenarios.clsc.Scenario;
import org.st.scenarios.clsc.Specification;


public class ExportToCPN {

	private BuildCPNUtil toCPN;
	
	private Specification spec;
	private PetriNet net;

	private Page mainPage;

	// remember which scenario is translated to which CPN page to properly
	// implement hierarchies in the net
	private HashMap<Scenario, Page> scenario_to_page;

	// each maximal event of a main-chart produces a token on a global place,
	// this place is uniquely defined by the event's name, this mapping stores
	// this relationship
	private HashMap<String, Place> maxPlaces;

	/**
	 * Create a new translation object for the given {@link Specification}.
	 * 
	 * @param spec
	 */
	public ExportToCPN(Specification spec) {
		this.spec = spec;
		this.toCPN = new BuildCPNUtil();
	}

	/**
	 * Translate the specification.
	 * 
	 * @return the resulting {@link PetriNet} that can be exported to CPN Tools
	 */
	public PetriNet translate() {
		this.net = toCPN.createPetriNet();

		// write type declarations
		addTypeDeclarations();

		// write variable declarations
		addVariableDeclarations();

		// write function declarations
		addFunctionDeclarations();

		// initialize bookkeeping data structures on the specification level
		this.scenario_to_page = new HashMap<Scenario, Page>();
		this.maxPlaces = new HashMap<String, Place>();

		// create the main page to contain all scenarios and their
		// interconnections
		mainPage = toCPN.addPage(net, "Spec");

		// translate each scenario
		for (Scenario s : spec.scenarios) {
			translateScenario_toPage(s);
		}

		return this.net;
	}

	/**
	 * Translate the scenario 's' to a Petri net page of {@link #net}.
	 * 
	 * Thes page contains the logic of the main-chart of 's', and a transition
	 * that activates that chart when its pre-chart occurred. The interface to
	 * the rest of the specification (checking occurrence of the pre-chart) and
	 * notifying other scenarios about completion of the chart is implemented by
	 * fusion places, i.e. places on the page of the scenario that are fused
	 * with places on the specification level on the {@link #mainPage}. All
	 * places are created by this method, ensuring consistency with the fusion
	 * places of other scenarios. The resulting page is added to {@link #net}.
	 * 
	 * @param s
	 * @return
	 */
	private Page translateScenario_toPage(Scenario s) {
		
		// create the page
		Page pg = toCPN.addPage(net, s.getName());
		scenario_to_page.put(s, pg);

		Instance t_scen = toCPN.createSubPageTransition(pg, mainPage, s.getName());

		Chart main = s.mainChart;

		// compute the logic to connect pre-chart and main-chart, and create
		// the pre-places of the minimal main-chart events
		HashMap<Event, Place[]> minPlaces = translateScenario_activationLogic(pg, s, t_scen);

		// create a transition for each main-chart event
		HashMap<Event, Transition> e2t = new HashMap<Event, Transition>();
		for (Event e : main.getEvents()) {
			Transition t = toCPN.addTransition(pg, e.name);
			e2t.put(e, t);

			// and add a place to count occurrences of t
			Place p = toCPN.addPlace(pg, "", "INT", "1`0");
			toCPN.addArc(pg, p, t, "j");
			toCPN.addArc(pg, t, p, "j+1");
		}

		// create the places surrounding the transitions of the main-chart events 
		for (Event e : main.getEvents()) {

			int hist_var_count = 0;
			for (Dependency d : e.in) {
				Place p = toCPN.addPlace(pg, "", "TOKENHIST");
				// Annotate the incoming arcs of the transition with a history
				// variable. Each incoming dependency 'd' defines a new incoming arc
				// which requires a new history variable "hX"
				toCPN.addArc(pg, p, e2t.get(d.getTarget()), "h"+hist_var_count);
				hist_var_count++;

				// Annotate the outgoing arcs of the transition with the
				// function that extends the consumed token histories with an
				// occurrence of the event represented by this transition
				toCPN.addArc( pg,	e2t.get(d.getSource()),	p,
						getTransitionOutGoingArcFunction(d.getSource(),	getPredecessorEvents(d.getSource(), s)));
			}

			if (e.isMax()) {
				// each maximal main-chart event produces on a specification
				// level place with the same name, first create the specification
				// level place and connect it to the hierarchical transition that
				// contains this scenario
				if (!maxPlaces.containsKey(e.name)) {
					Place p = toCPN.addPlace(mainPage, e.name, "TOKENHIST");
					maxPlaces.put(e.name, p);
				}
				Place pTop = maxPlaces.get(e.name);
				toCPN.addArc(mainPage, t_scen, pTop, "");

				// then create the scenario level reference place and connect it
				// to the specification level place
				RefPlace p = toCPN.addReferencePlace(pg, e.name, "TOKENHIST", "", pTop, t_scen);
				toCPN.addArc(pg, e2t.get(e), p,
						getTransitionOutGoingArcFunction(e,	getPredecessorEvents(e, s)));
			}

			if (e.isMin()) {
				// each minimal main chart event consumes from its places that were
				// created and defined in translateScenario_activationLogic()
				for (int i = 0; i < minPlaces.get(e).length; i++) {
					toCPN.addArc(pg, minPlaces.get(e)[i], e2t.get(e),	"h"+i);
				}
			}
		} // create places

		return pg;
	}
	
	private String translateChartToTokenHistoryString (Chart c) {
		//1`{max=[("!avail",0)],
		//     deps=[{event=("!avail",0),pred=nullEV}]}
		
		String maxEvents = "";
		String dependencies = "";
		
		HashMap<Event, Integer> eventNumber = new HashMap<Event, Integer>();
		
		int eventNum = 0;
		for (Event e : c.getEvents()) {
			eventNumber.put(e, eventNum++);
			if (e.isMax()) {
				if (maxEvents.length() > 0) maxEvents += ", ";
				maxEvents += getEventString(e, eventNumber);
			}
			if (e.isMin()) {
				if (dependencies.length() > 0) dependencies += ", ";
				dependencies += getDependencyString(getEventString(e, eventNumber), getNullEventString());
			}
		}

		for (Dependency d : c.getDependencies()) {
			if (dependencies.length() > 0) dependencies += ", ";
			dependencies += getDependencyString(
					getEventString(d.getTarget(), eventNumber),
					getEventString(d.getSource(), eventNumber));
		}
		
		
		String tokenHistory = "{max=["+maxEvents+"], deps=["+dependencies+"]}";
		
		return tokenHistory;
	}
	
	private String getEventString(String e_name, int num) {
		return "(\""+e_name+"\", "+num+")"; 
	}
		
	private String getEventString(Event e, HashMap<Event, Integer> eventNumber) {
		return "(\""+e.name+"\", "+eventNumber.get(e)+")"; 
	}
	
	private String getDependencyString(String e, String pred) {
		return "{event="+e+",pred="+pred+"}";
	}
	
	private String getNullEventString() {
		return "nullEV";
	}
	

	/**
	 * Add the activation logic of the scenario to the page.
	 * 
	 * A scenario without pre-chart is initially enabled once. Create for each
	 * minimal transition of the main-chart a dedicated place, that is initially
	 * marked with an empty token history.
	 * 
	 * A scenario with a pre-chart is only enabled after its pre-chart occurred.
	 * This is checked by a dedicated activation transition of the chart which
	 * is enabled only if there are history-tokens that describe an occurrence
	 * of the complete pre-chart. This is the case if each maximal event with
	 * label 'A' of the pre-chart produced a token on the global place 'A' with
	 * the corresponding history. The activation transition consumes these
	 * tokens from the global places and re-produces these tokens for each
	 * minimal event of the main-chart on a separate place, i.e. each minimal
	 * main-chart event has its own set of pre-places and is concurrent to all
	 * other main-chart events.
	 * 
	 * 
	 * @param pg
	 * @param s
	 * @param t_scen
	 *            the hierarchical transition of the specification level that
	 *            represents the scenario
	 * @return a mapping that assigns each minimal main-chart event its set of
	 *         pre-places
	 */
	public HashMap<Event, Place[]> translateScenario_activationLogic(Page pg,
			Scenario s, Instance t_scen) {

		Chart pre = s.preChart;
		Chart main = s.mainChart;

		// the pre-places of the minimal events of the main-chart
		HashMap<Event, Place[]> minPlaces = new HashMap<Event, Place[]>();

		if (pre == null) {
			// scenario without pre-chart, create an initially marked place for
			// each minimal main-chart event
			for (Event f : main.getMinEvents()) {
				Place p = toCPN.addPlace(pg, "init", "TOKENHIST", "1`{max=[nullEV],deps=[]}");
				minPlaces.put(f, new Place[] { p });
			}
		} else {
			// scenario with pre-chart, create interface places from which the
			// scenario consumes at the specification level
			for (Event e : pre.getMaxEvents()) {
				// the scenario consumes a token produced by each maximal
				// pre-chart event
				if (!maxPlaces.containsKey(e.name)) {
					Place p = toCPN.addPlace(mainPage, e.name, "TOKENHIST");
					maxPlaces.put(e.name, p);
				}

				// add an arc from the specification level interface place to
				// the hierarchical transition representing the scenario
				Place p = maxPlaces.get(e.name);
				toCPN.addArc(mainPage, p, t_scen, "");
			}

			// the transition to activate the scenario
			Transition t_activate = toCPN.addTransition(pg, "activate_"+s.getName(), "[endsWith(h,hPre)]");
			Place      p_preChart = toCPN.addPlace(pg, "pre_"+s.getName(), "TOKENHIST",
					"1`"+translateChartToTokenHistoryString(pre));
			toCPN.addArc(pg, p_preChart, t_activate, "hPre");
			toCPN.addArc(pg, t_activate, p_preChart, "hPre");

			// create for each minimal main chart event its own set of
			// pre-places
			// initialize array
			for (Event f : main.getMinEvents()) {
				minPlaces.put(f, new Place[pre.getMaxEvents().size()]);
			}

			// We still have to define the guard for the activation transition.
			// This guard ranges over all history variables of incoming arcs of
			// t_activate. These arcs are created next.
			String t_activate_incoming_hist = "";
			
			int hist_var_count = 0;
			for (Event e : pre.getMaxEvents()) {
				// create interface place at the scenario-level and connect it
				// to the place with the same name at the specification level
				Place pTop = maxPlaces.get(e.name);
				RefPlace p = toCPN.addReferencePlace(pg, e.name, "TOKENHIST", "", pTop, t_scen);

				// the activation transition consumes from the scenario level place 
				toCPN.addArc(pg, p, t_activate, "h"+hist_var_count);
				// and produces on the corresponding place for each minimal main-chart event 
				for (Event f : main.getMinEvents()) {
					Place p2 = toCPN.addPlace(pg, "", "TOKENHIST");
					toCPN.addArc(pg, t_activate, p2, "h"+ hist_var_count);
					minPlaces.get(f)[hist_var_count] = p2;
				}
				
				if (hist_var_count > 0) t_activate_incoming_hist += ", ";
				t_activate_incoming_hist += "h"+hist_var_count;
				
				hist_var_count++;
			}
			
			// set the guard for activating the scenario
			String t_activate_guard = "[endsWith( joinHistories(["+t_activate_incoming_hist+"]),hPre)]";
			t_activate.getCondition().setText(t_activate_guard);
		}

		return minPlaces;
	}

	/**
	 * @param e
	 * @param s
	 * @return the direct predecessors of event 'e' in scenario 's' (these are
	 *         either the predecessors denoted by {@link Event#in}, or the
	 *         maximal pre-chart events if 'e' is a minimal main-chart event)
	 */
	private List<Event> getPredecessorEvents(Event e, Scenario s) {
		if (e.isMin()) {
			if (s.preChart == null)
				return null;
			else
				return s.preChart.getMaxEvents();
		} else {
			LinkedList<Event> pre = new LinkedList<Event>();
			for (Dependency d : e.in) {
				pre.add(d.getSource());
			}
			return pre;
		}
	}

	/**
	 * The outgoing arcs of transitions that represent main-chart events extend
	 * the token histories of the consumed tokens. This extension is implemented
	 * by a function that joins the histories of all consumed tokens and appends
	 * the event that occurred. This function constructs the call to this
	 * function by basically collecting the names and history variables of all
	 * direct predecessor events of event 'e'.
	 * 
	 * @param e
	 * @param predecessors
	 * @return the ML function call to write on each outgoing arc of the
	 *         transition representing e
	 */
	private String getTransitionOutGoingArcFunction(Event e, List<Event> predecessors) {
		
		String fct = "";

		// ML list of all direct predecessor event names
		String predEvents = "[";

		// ML list of all history variables at the incoming arcs of
		// the transition that represents event e
		String predHistories = "[";
		int hist_var_count = 0;
		if (predecessors == null) {
			// minimal event of a chart without pre-chart, this is indicated by
			// predecessors being null, in the translation, the transition reads
			// from a dedicated place and requires no pre-event
			predEvents += "\"\"";
			predHistories += "h0";

		} else {
			for (Event pre : predecessors) {

				// predEvents = [ "eventName1", "eventName2", ..., "eventNameN"
				// ]
				if (hist_var_count > 0)
					predEvents += ", ";
				predEvents += "\"" + pre.name + "\"";

				// predHistories = [ h0, h1, ..., hN ]
				if (hist_var_count > 0)
					predHistories += ", ";
				predHistories += "h" + hist_var_count;
				hist_var_count++;
			}
		}
		predEvents += "]";
		predHistories += "]";

		fct += "appendEvent((\"" + e.name + "\",j), "+predEvents+", joinHistories( "+predHistories+" ) )";

		return fct;
	}

	/**
	 * In the resulting CPN model, incoming arcs of transitions are annotated
	 * with history variables. The incoming arcs of each transition need to be
	 * annotated differently. This function determines the number of required
	 * variables to achieve this.
	 * 
	 * @return
	 */
	private int getNumberOfHistVariables() {
		int numIncoming = 0;

		// count the number of direct predecessors of each event
		for (Scenario s : spec.scenarios) {
			// for minimal main-chart events, the number of predecessors is
			// the number of maximal pre-chart events
			if (s.preChart != null
					&& numIncoming < s.preChart.getMaxEvents().size())
				numIncoming = s.preChart.getMaxEvents().size();

			if (numIncoming < s.mainChart.getMinEvents().size())
				numIncoming = s.mainChart.getMinEvents().size();

			for (Event e : s.mainChart.getEvents()) {
				if (numIncoming < e.in.size())
					numIncoming = e.in.size();
			}
		}

		return numIncoming;
	}

	/**
	 * declare all color sets required for the translation
	 */
	private void addTypeDeclarations() {
		toCPN.declareStandardColors(net);
		// colset STRINGLIST = list STRING;
		toCPN.declareColorSet_list(net, "STRINGLIST", "STRING");
		// colset ID = product STRING * INT;
		toCPN.declareColorSet_product(net, "ID", new String[] {
				"STRING", "INT" });
		// val nullEV = ("", 0); (* --- the nill event used to terminate token
		// histories --- *)
		toCPN.declareMLFunction(net, "val "+getNullEventString()+" = "+getEventString("", 0)+";");

		// colset EVENTLIST = list ID;
		toCPN.declareColorSet_list(net, "EVENTLIST", "ID");
		// colset HISTREL = record event:ID * pred:ID;
		toCPN.declareColorSet_record(net, "HISTREL", new String[] {
				"event", "ID", "pred", "ID" });
		// colset EVENTDEP = list HISTREL
		toCPN.declareColorSet_list(net, "EVENTDEP", "HISTREL");
		// colset TOKENHIST = record max:EVENTLIST * deps:EVENTDEP;
		toCPN.declareColorSet_record(net, "TOKENHIST", new String[] {
				"max", "EVENTLIST", "deps", "EVENTDEP" });
		// colset TOKENHISTLIST = list TOKENHIST;
		toCPN.declareColorSet_list(net, "TOKENHISTLIST", "TOKENHIST");

		// colset EVENTPAIR = product ID * ID;
		toCPN.declareColorSet_product(net, "EVENTPAIR", new String[] {
				"ID", "ID" });
		// colset EVENTPAIRLIST = list EVENTPAIR;
		toCPN.declareColorSet_list(net, "EVENTPAIRLIST", "EVENTPAIR");
	}

	/**
	 * declare all variables (to be inscribed at arcs) to the net
	 */
	private void addVariableDeclarations() {
		// the variable to read the prechart from a dedicated place
		toCPN.declareVariable(net, "hPre", "TOKENHIST");
		// each incoming arc of a transition gets a different variable
		// determine the maximum indegree of a transition, and declare
		// a variable
		int histVars = getNumberOfHistVariables();
		for (int i = 0; i < histVars; i++) {
			toCPN.declareVariable(net, "h" + i, "TOKENHIST");
		}

		// we need to count occurrences of transitions. this is the variable
		toCPN.declareVariable(net, "j", "INT");
	}

	/**
	 * directory of ML function declarations stored in external files
	 */
	private static final String DIR_ML_DECLARATIONS = "./resources/declarations";

	/**
	 * declare all ML functions for the translation to Petri nets the
	 * declarations are read from text files stored in
	 * {@value #DIR_ML_DECLARATIONS}
	 */
	private void addFunctionDeclarations() {
		try {
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/getEvent.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/appendEvent.ml.txt"));
			toCPN.declareMLFunction(net, readFromFile(DIR_ML_DECLARATIONS
							+ "/joinHistories.ml.txt"));

			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/matchEvent.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/matchEvents.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS
							+ "/getPredecessorEvents.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS
							+ "/matchPredecessors.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/endsWith.ml.txt"));

		} catch (FileNotFoundException e) {
			System.err.println("Failed to read function declaration: " + e);
		}
	}

	/**
	 * Read contents of a text file.
	 * 
	 * @param fFileName
	 * @return
	 * @throws FileNotFoundException
	 */
	private String readFromFile(final String fFileName)
			throws FileNotFoundException {
		StringBuilder text = new StringBuilder();
		String NL = System.getProperty("line.separator");
		Scanner scanner = new Scanner(new FileInputStream(fFileName));
		try {
			while (scanner.hasNextLine()) {
				text.append(scanner.nextLine() + NL);
			}
		} finally {
			scanner.close();
		}
		return text.toString();
	}
}
