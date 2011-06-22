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

package org.st.scenarios.clsc;

import java.util.LinkedList;
import java.util.List;

/**
 * A chart is a partial order of labeled events. The events are ordered by
 * direct causal dependencies. Prospectively, the dependencies will be labeled
 * with additional attributes to further refine the nature of the dependency.
 * 
 * @author dfahland
 
 */
public class Chart {

	private List<Event> 		events;			// events of the chart
	private List<Dependency> 	dependencies;	// dependencies between events in the chart 

	/**
	 * Create a new empty chart.
	 */
	public Chart() {
		events = new LinkedList<Event>();
		dependencies = new LinkedList<Dependency>();
	}
	
  /**
   * Create a copy of chart c
   * @param c
   */  
	public Chart(Chart c) {
    events = new LinkedList<Event>(c.getEvents());
    dependencies = new LinkedList<Dependency>(c.getDependencies());
  }

	/**
	 * Add event <code>e</code> to the chart having direct predecessors
	 * <code>pred</code>.
	 * 
	 * @param e
	 * @param pred
	 */
	public void addEvent(Event e, List<Event> pred) {
		events.add(e);
		if (pred != null) {
			for (Event p : pred) {
				addDependency(p, e);
			}
		}
	}
	
	/**
	 * Create a new event with the given name and add it to the chart.
	 * 
	 * @param name
	 * @return the new evenet
	 */
	public Event addEvent(String name) {
		Event e = new Event(name);
		addEvent(e, null);
		return e;
	}

	/**
	 * Make event <code>tgt</code> directly depend on event <code>src</code>
	 * 
	 * @param src
	 * @param tgt
	 */
	public void addDependency(Event src, Event tgt) {
		Dependency d = new Dependency(src, tgt);
		src.out.add(d);
		tgt.in.add(d);
		
		dependencies.add(d);
	}
	
	/**
	 * @return all events in the chart
	 */
	public List<Event> getEvents() {
		return events;
	}

	/**
	 * @return all dependencies in the chart
	 */
	public List<Dependency> getDependencies() {
		return dependencies;
	}

	/**
	 * @return the minimal events of the chart (i.e. having no
	 *         predecessor)
	 */
	public List<Event> getMinEvents() {
		LinkedList<Event> min = new LinkedList<Event>();
		for (Event e : events) {
			if (e.isMin()) min.add(e);
		}
		return min;
	}

	/**
	 * @return the maximal events of the chart (i.e. having no
	 *         successor)
	 */
	public List<Event> getMaxEvents() {
		LinkedList<Event> max = new LinkedList<Event>();
		for (Event e : events) {
			if (e.isMax()) max.add(e);
		}
		return max;
	}

	/**
	 * Append chart c to this chart 
	 * @param c
	 */
	public void append(Chart c) {
	  
	  List<Event> oldMax = new LinkedList<Event>();
	  for (Event e : getEvents()) {
	    if (e.isMax()) oldMax.add(e);
	  }
	  
	  this.events.addAll(c.getEvents());
	  this.dependencies.addAll(c.getDependencies());

	  // synchronize this chart with the appended chart by adding
    // a dependency from each maximal event of this chart to each
	  // minimal events of the appended chart
	  for (Event e : oldMax) {
	    for (Event f : c.getEvents()) {
	      if (f.isMin()) {
	        addDependency(e, f);
	      }
	    }
	  }
	}
	
	public static Chart concat(Chart c1, Chart c2) {
	  Chart c1_prime = new Chart(c1);
	  c1_prime.append(c2);
	  return c1_prime;
	}
	
}
