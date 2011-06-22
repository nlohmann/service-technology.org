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

/**
 * A dependency expresses a causal relation between two {@link Event}s of a
 * {@link Chart}.
 * 
 * @author dfahland
 * 
 */
public class Dependency {

	private Event source;
	private Event target;
	
	public Dependency(Event src, Event tgt) {
		source = src;
		target = tgt;
	}
	
	public Event getSource() {
		return source;
	}
	
	public Event getTarget() {
		return target;
	}
	
	@Override
	public String toString() {
	  return "("+source+" -> "+target+")";
	}
}
