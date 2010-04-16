/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
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

package hub.top.uma;

import java.util.HashSet;

@Deprecated
public class GraphNode {

	public String label;
	public boolean event = false;
	
	public boolean marked = false;
	
	HashSet<GraphNode> pred = new HashSet<GraphNode>();
	HashSet<GraphNode> succ = new HashSet<GraphNode>();
	
	public GraphNode (String label, boolean isEvent, boolean isMarked) {
		this.label = label;
		this.event = isEvent;
		this.marked = isMarked;
	}
	
	public void addSucc(GraphNode n) {
		this.succ.add(n);
		n.pred.add(this);
	}
	
	public void addPred(GraphNode n) {
		this.pred.add(n);
		n.succ.add(this);
	}
}
