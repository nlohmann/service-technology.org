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
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Scanner;
import java.util.Set;

import org.cpntools.accesscpn.model.Arc;
import org.cpntools.accesscpn.model.HasId;
import org.cpntools.accesscpn.model.Instance;
import org.cpntools.accesscpn.model.Node;
import org.cpntools.accesscpn.model.Object;
import org.cpntools.accesscpn.model.Page;
import org.cpntools.accesscpn.model.PetriNet;
import org.cpntools.accesscpn.model.Place;
import org.cpntools.accesscpn.model.PlaceNode;
import org.cpntools.accesscpn.model.RefPlace;
import org.cpntools.accesscpn.model.Transition;
import org.cpntools.accesscpn.model.TransitionNode;
import org.cpntools.accesscpn.model.util.BuildCPNUtil;
import org.omg.PortableInterceptor.SUCCESSFUL;
import org.st.scenarios.clsc.Chart;
import org.st.scenarios.clsc.Dependency;
import org.st.scenarios.clsc.Event;
import org.st.scenarios.clsc.Scenario;
import org.st.scenarios.clsc.Specification;


public class ExportToCPN2 {

	private BuildCPNUtil toCPN;
	
	private Specification spec;
	private PetriNet net;

	// remember which scenario is translated to which CPN page to properly
	// implement hierarchies in the net
	private HashMap<Scenario, Page> scenario_to_page;
	
	private HashSet<Transition> activation_transitions;
	private HashSet<Place> activation_transition_conditions;
	
	private Map<Node, List<String> > roleAssignment;

	// each maximal event of a main-chart produces a token on a global place,
	// this place is uniquely defined by the event's name, this mapping stores
	// this relationship
	private HashMap<String, Place> maxPlaces;

	/**
	 * Create a new translation object for the given {@link Specification}.
	 * 
	 * @param spec
	 */
	public ExportToCPN2(Specification spec) {
		this.spec = spec;
		this.toCPN = new BuildCPNUtil();
	}

	/**
	 * Translate the specification.
	 * 
	 * @param hierarchical put each scenario into a separate subpage of the net
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
		this.activation_transitions = new HashSet<Transition>();
		this.activation_transition_conditions = new HashSet<Place>();
		this.roleAssignment = new HashMap<Node, List<String>>();

		// create the main page to contain all scenarios and their
		// interconnections
		Page mainPage = toCPN.addPage(net, "Spec");

		// translate each scenario
		for (Scenario s : spec.scenarios) {
			translateScenario_toPage(s, mainPage);
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
	private Page translateScenario_toPage(Scenario s, Page pg) {
		
		scenario_to_page.put(s, pg);

		Chart main = s.mainChart;

		// compute the logic to connect pre-chart and main-chart, and create
		// the pre-places of the minimal main-chart events
		HashMap<Event, Place[]> minPlaces = translateScenario_activationLogic(pg, s);

		// create a transition for each main-chart event
		HashMap<Event, Transition> e2t = new HashMap<Event, Transition>();
		for (Event e : main.getEvents()) {
			Transition t = toCPN.addTransition(pg, e.name);
			e2t.put(e, t);

			// and add a place to count occurrences of t
			RefPlace p = toCPN.addFusionPlace(pg, "", "INT", "1`0", e.name+"_counter");
			toCPN.addArc(pg, p, t, "j");
			toCPN.addArc(pg, t, p, "j+1");
			
			Chart cHist = s.getLocalHistory(e);
			
			if (cHist.getEvents().size() > 0) {
  			Place pHist = toCPN.addPlace(pg, e.name+"_hist", "TOKENHIST", "1`"+translateChartToTokenHistoryString(cHist));
        toCPN.addArc(pg, pHist, t, "hPre");
        toCPN.addArc(pg, t, pHist, "hPre");
        
        // get the number of direct predecessors of e and build a list
        // of history variable names, one for each predecessor
        String t_activate_incoming_hist = "";
        int hist_var_count = 0;
        for (Event f : cHist.getEvents()) {
          if (!f.isMax()) continue;
          
          if (hist_var_count > 0) t_activate_incoming_hist += ", ";
          t_activate_incoming_hist += "h"+hist_var_count;
          hist_var_count++;
        }
  
        // set the guard for activating the scenario
        String t_activate_guard = "[endsWith( joinHistories(["+t_activate_incoming_hist+"]),hPre)]";
        t.getCondition().setText(t_activate_guard);
			}
		}

		// create the places surrounding the transitions of the main-chart events 
		for (Event e : main.getEvents()) {

			int hist_var_count = 0;
			for (Dependency d : e.in) {
				Place p = toCPN.addPlace(pg, d.getSource().name+"_"+d.getTarget().name, "TOKENHIST");
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
					Place p = toCPN.addPlace(pg, e.name, "TOKENHIST");
					maxPlaces.put(e.name, p);
				}
				Place pTop = maxPlaces.get(e.name);
        toCPN.addArc(pg, e2t.get(e), pTop,
              getTransitionOutGoingArcFunction(e, getPredecessorEvents(e, s)));				  
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
	 * @return a mapping that assigns each minimal main-chart event its set of
	 *         pre-places
	 */
	public HashMap<Event, Place[]> translateScenario_activationLogic(Page pg, Scenario s) {

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
					Place p = toCPN.addPlace(pg, e.name, "TOKENHIST");
					maxPlaces.put(e.name, p);
				}
			}

			// the transition to activate the scenario
			Transition t_activate = toCPN.addTransition(pg, "activate_"+s.getName(), "[endsWith(h,hPre)]");
			Place      p_preChart = toCPN.addPlace(pg, "pre_"+s.getName(), "TOKENHIST",
					"1`"+translateChartToTokenHistoryString(pre));
			toCPN.addArc(pg, p_preChart, t_activate, "hPre");
			toCPN.addArc(pg, t_activate, p_preChart, "hPre");
			
			activation_transitions.add(t_activate);
			activation_transition_conditions.add(p_preChart);

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
				PlaceNode p = maxPlaces.get(e.name);

				// the activation transition consumes from the scenario level place 
				toCPN.addArc(pg, p, t_activate, "h"+hist_var_count);
				// and produces on the corresponding place for each minimal main-chart event 
				for (Event f : main.getMinEvents()) {
					Place p2 = toCPN.addPlace(pg, e.name+"_"+f.name, "TOKENHIST");
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
	
	private static String ROLE_UNASSIGNED = "unassigned";
	
	public void assignRoles(Map<String, String> assignment) {
	  for (Page pg : net.getPage()) {
	    for (Object o : pg.getObject()) {
	      if (!(o instanceof TransitionNode)) continue;
	      TransitionNode t = (TransitionNode)o;

        roleAssignment.put(t, new LinkedList<String>());
	      if (assignment.containsKey(t.getName().getText())) {
	        roleAssignment.get(t).add(assignment.get(t.getName().getText()));
	      } else {
	        roleAssignment.get(t).add(ROLE_UNASSIGNED);
	      }
	    }
	  }
	  
	  for (Page pg : net.getPage()) {
      for (Object o : pg.getObject()) {
        if (!(o instanceof PlaceNode)) continue;
        spreadRolesToPlace((PlaceNode)o);
      }
    }
	}
	
	private void spreadRolesToPlace(PlaceNode p) {
    Set<String> env_roles = new HashSet<String>();
    for (Arc a : p.getSourceArc()) {
      List<String> roles = roleAssignment.get(a.getTarget());
      env_roles.addAll(roles);
    }
    for (Arc a : p.getTargetArc()) {
      List<String> roles = roleAssignment.get(a.getSource());
      env_roles.addAll(roles);
    }
    roleAssignment.put(p, new LinkedList<String>(env_roles));	  
	}
	
	public void multiplyActivationLogic() {
	  // move scenario activation transitions to a page if all successor transitions
    // also occur on this page
    for (Transition t : activation_transitions) {
      
      if (roleAssignment.get(t).size() == 1 && roleAssignment.get(t).contains(ROLE_UNASSIGNED))
      {
        Set<String> successorRoles = new HashSet<String>();
        for (Arc a : t.getSourceArc()) {
          for (Arc a2 : a.getTarget().getSourceArc()) {
            if (a2.getTarget() == t) continue;
            successorRoles.addAll(roleAssignment.get(a2.getTarget()));
          }
        }
      
        if (successorRoles.size() == 1 && !successorRoles.contains(ROLE_UNASSIGNED)) {
          roleAssignment.get(t).clear();
          roleAssignment.get(t).addAll(successorRoles);
        } else {
          successorRoles.remove(ROLE_UNASSIGNED);
          LinkedList<String> _successorRoles = new LinkedList<String>(successorRoles);
          
          // reassign current activation transition and its guard
          String oldTransitionName = t.getName().getText();
          String successorRole = _successorRoles.get(0);
          t.getName().setText(oldTransitionName+"_"+successorRole);
          roleAssignment.get(t).clear();
          roleAssignment.get(t).add(successorRole);
          
          Page pg = t.getPage();
  
          Place activation_place = null;
          for (Arc a : t.getSourceArc()) {
            if (activation_transition_conditions.contains(a.getTarget())) {
              activation_place = (Place)a.getTarget();
              break;
            }
          }
          
          List<Place> newPlaces = new LinkedList<Place>();
          for (int i=1; i<_successorRoles.size(); i++) {
            successorRole = _successorRoles.get(i);
            Transition t2 = toCPN.addTransition(pg, oldTransitionName+"_"+successorRole,
                t.getCondition().getText());
            Place p2 = toCPN.addPlace(pg, activation_place.getName().getText()+"_"+successorRole,
                activation_place.getSort().getText(), activation_place.getInitialMarking().getText());
            toCPN.addArc(pg, t2, p2, "hPre");
            toCPN.addArc(pg, p2, t2, "hPre");
            
            roleAssignment.put(t2, new LinkedList<String>());
            roleAssignment.get(t2).add(successorRole);
            newPlaces.add(p2);
            
            for (Arc a : t.getSourceArc()) {
              if (a.getTarget() == activation_place) continue;
              toCPN.addArc(pg, t2, a.getTarget(), a.getHlinscription().getText());
            }
            for (Arc a : t.getTargetArc()) {
              if (a.getSource() == activation_place) continue;
              toCPN.addArc(pg, a.getSource(), t2, a.getHlinscription().getText());
            }
          }
          
          for (Place p : newPlaces) {
            spreadRolesToPlace(p);
          }
        }
      }
    }
	}

	public void splitRolesToSubpages(Page oldPage) {
	  
	  Page parentPage = toCPN.addPage(net, "spec");
	  
	  Set<String> roles = new HashSet<String>();
	  Map<String, Page> subPages = new HashMap<String, Page>();
	  Map<String, Instance> subPageTrans = new HashMap<String, Instance>();
	  for (List<String> r : roleAssignment.values())
	    roles.addAll(r);
	  
	  roles.remove(oldPage.getName().getText());
	  
	  for (String role : roles) {
	    Page subPage = toCPN.addPage(net, role);
	    subPages.put(role, subPage);
	    Instance t_page = toCPN.createSubPageTransition(subPage, parentPage, role);
	    subPageTrans.put(role, t_page);
	  }
	  
	  Set<Place> sharedPlace = new HashSet<Place>();
    for (org.cpntools.accesscpn.model.Object o : oldPage.getObject()) {
      if (! (o instanceof Place))
        continue;
      Place p = (Place)o;
      if (!roleAssignment.containsKey(p)) sharedPlace.add(p);
      if (roleAssignment.get(p).contains(ROLE_UNASSIGNED)) sharedPlace.add(p);
      if (roleAssignment.get(p).size() > 1) sharedPlace.add(p);
    }
    
    for (Place p : sharedPlace) {
      p.setPage(parentPage);
    }
    
    Map<HasId, Page> page_to_set = new HashMap<HasId, Page>();
    LinkedList<Arc> arc_toRemove = new LinkedList<Arc>();
	  for (Object o : oldPage.getObject()) {
      if (! (o instanceof TransitionNode || o instanceof Instance))
      {
        continue;
      }
      
      Node t = (Node)o;
      String subPageName = roleAssignment.get(t).get(0);
      Page pg = subPages.get(subPageName);
      if (pg == null) pg = parentPage;
      page_to_set.put(t,pg);
      
      LinkedList<Arc> tgt_arcs = new LinkedList<Arc>(t.getTargetArc());
      for (Arc a : tgt_arcs) {
        PlaceNode p = (PlaceNode)a.getSource();
        if (!sharedPlace.contains(p) || pg == parentPage) {
          page_to_set.put(p,pg);
          page_to_set.put(a,pg);
        } else if (p instanceof Place){
          
          Place p2 = (Place) p;
          
          RefPlace p_ref = null;
          for (Object o_ref : pg.getObject()) {
            if (o_ref instanceof RefPlace) {
              if (((RefPlace)o_ref).getRef() == p2) {
                p_ref = (RefPlace)o_ref;
                break;
              }
            }
          }
          
          if (p_ref == null)
            p_ref = toCPN.addReferencePlace(pg,
              p2.getName().getText(), 
              p2.getSort().getText(), 
              p2.getInitialMarking().getText(), p2, 
              subPageTrans.get(subPageName));
          
          Arc a2 = toCPN.addArc(pg, p_ref, t, a.getHlinscription().getText());
          
          boolean arcExists = false;
          for (Arc p2_out : p2.getSourceArc()) {
            if (p2_out.getTarget() == subPageTrans.get(subPageName)) {
              arcExists = true;
              break;
            }
          }
          if (!arcExists)
            toCPN.addArc(parentPage, p2, subPageTrans.get(subPageName), a.getHlinscription().getText());
          
          arc_toRemove.add(a);
        }
      }
      
      LinkedList<Arc> src_arcs = new LinkedList<Arc>(t.getSourceArc());
      for (Arc a : src_arcs) {
        PlaceNode p = (PlaceNode)a.getTarget();
        if (!sharedPlace.contains(p) || pg == parentPage) {
          page_to_set.put(p,pg);
          page_to_set.put(a,pg);
        } else if (p instanceof Place){
          
          Place p2 = (Place) p;
          
          RefPlace p_ref = null;
          for (Object o_ref : pg.getObject()) {
            if (o_ref instanceof RefPlace) {
              if (((RefPlace)o_ref).getRef() == p2) {
                p_ref = (RefPlace)o_ref;
                break;
              }
            }
          }
          
          if (p_ref == null)
            p_ref = toCPN.addReferencePlace(pg,
              p2.getName().getText(), 
              p2.getSort().getText(), 
              p2.getInitialMarking().getText(), p2, 
              subPageTrans.get(subPageName));
          
          Arc a2 = toCPN.addArc(pg, t, p_ref, a.getHlinscription().getText());
          
          boolean arcExists = false;
          for (Arc p2_in : p2.getTargetArc()) {
            if (p2_in.getSource() == subPageTrans.get(subPageName)) {
              arcExists = true;
              break;
            }
          }
          if (!arcExists)
            toCPN.addArc(parentPage, subPageTrans.get(subPageName), p2, a.getHlinscription().getText());
          
          arc_toRemove.add(a);
        }
      }
	  }
	  
	  for (Entry<HasId, Page> set : page_to_set.entrySet()) {
	    if (set.getKey() instanceof Object) {
	      ((Object)set.getKey()).setPage(set.getValue());
	    }
	    if (set.getKey() instanceof Arc) {
	      ((Arc)set.getKey()).setPage(set.getValue());
	    }
	  }
	  
	  for (Arc a : arc_toRemove) {
	    a.getPage().getArc().remove(a);
	  }
	  
	  net.getPage().remove(oldPage);
	  
	   for (Page pg : net.getPage()) {
	     for (Arc a : pg.getArc()) {
	       if (a.getSource().getPage() != a.getTarget().getPage()) {
	         System.out.println("ERROR: "+a+" connects nodes in different pages:");
	         System.out.println("   "+a.getSource()+" in "+a.getSource().getPage().getName().getText());
           System.out.println("   "+a.getTarget()+" in "+a.getTarget().getPage().getName().getText());
	       }
	     }
	   }
	     

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
		toCPN.declareColorSet_product(net, "ID", new String[] { "STRING", "INT" });
		// val nullEV = ("", 0); (* --- the nill event used to terminate token
		// histories --- *)
		toCPN.declareMLFunction(net, "val "+getNullEventString()+" = "+getEventString("", 0)+";");

		// colset EVENTLIST = list ID;
		toCPN.declareColorSet_list(net, "EVENTLIST", "ID");
		// colset HISTREL = record event:ID * pred:ID;
		toCPN.declareColorSet_record(net, "HISTREL", new String[] { "event", "ID", "pred", "ID" });
		// colset EVENTDEP = list HISTREL
		toCPN.declareColorSet_list(net, "EVENTDEP", "HISTREL");
		// colset TOKENHIST = record max:EVENTLIST * deps:EVENTDEP;
		toCPN.declareColorSet_record(net, "TOKENHIST", new String[] {	"max", "EVENTLIST", "deps", "EVENTDEP" });
		// colset TOKENHISTLIST = list TOKENHIST;
		toCPN.declareColorSet_list(net, "TOKENHISTLIST", "TOKENHIST");

		// colset EVENTPAIR = product ID * ID;
		toCPN.declareColorSet_product(net, "EVENTPAIR", new String[] { "ID", "ID" });
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
			toCPN.declareMLFunction(net,
			    readFromFile(DIR_ML_DECLARATIONS + "/joinHistories.ml.txt"));

			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/matchEvent.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/matchEvents.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/getPredecessorEvents.ml.txt"));
      toCPN.declareMLFunction(net,
          readFromFile(DIR_ML_DECLARATIONS + "/consistentMapping.ml.txt"));
			toCPN.declareMLFunction(net,
					readFromFile(DIR_ML_DECLARATIONS + "/matchPredecessors.ml.txt"));
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
