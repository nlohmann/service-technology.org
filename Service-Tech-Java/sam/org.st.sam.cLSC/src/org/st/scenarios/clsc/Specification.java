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

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * A scenario-based specification is a set of {@link Scenario}s.
 * 
 * @author dfahland
 */
public class Specification {

	public List<Scenario> scenarios;
	public Map<String, String> eventToComponent;
	
	public Specification() {
		scenarios = new LinkedList<Scenario>();
		eventToComponent = new HashMap<String, String>();
	}
	
	public String toDot() {
		StringBuilder b = new StringBuilder();
		
		b.append("digraph Specification {\n");
		
	    // standard style for nodes and edges
	    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
	    b.append("node [fontname=\"Helvetica\" fontsize=8 shape=box fixedsize width=\"1.0\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
	    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=black arrowhead=none weight=\"20.0\"];\n\n");

	    for (Scenario s : scenarios) {
	    	b.append(s.toDot());
	    	b.append("\n");
	    }
		
	    b.append("}\n");
	    
		return b.toString();
	}
}
