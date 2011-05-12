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
 * A scenario consists of two {@link Chart}s, a pre-chart (describing the
 * pre-condition of the scenario) and a main-chart (describing the contribution
 * of the scenario).
 * 
 * @author dfahland
 * 
 */
public class Scenario {

	public Chart preChart;
	public Chart mainChart;
	private String name;
	
	/**
	 * Create a new scenario of the given pre- and main-chart.
	 * 
	 * @param name
	 * @param pre
	 * @param main
	 */
	public Scenario(String name, Chart pre, Chart main) {
		this.name = name;
		this.preChart = pre;
		this.mainChart = main;
	}
	
	public String getName() {
		return name;
	}
}
