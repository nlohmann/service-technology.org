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
 * An event of a {@link Chart}.
 * 
 * @author dfahland
 */
public class Event {

	public String name;
	public List<Dependency> in;
	public List<Dependency> out;

	/**
	 * Create a new event with the given name.
	 * @param name
	 */
	public Event(String name) {
		this.name = name;
		this.in = new LinkedList<Dependency>();
		this.out = new LinkedList<Dependency>();
	}
	
	/**
	 * @return <code>true</code> iff the event has no successor events.
	 */
	public boolean isMax () {
		return this.out.isEmpty();
	}

	/**
	 * @return <code>true</code> iff the event has no predecessor events.
	 */
	public boolean isMin() {
		return this.in.isEmpty();
	}
	
	@Override
	public String toString() {
	  return name;
	}
}
