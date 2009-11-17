/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.greta.oclets.canonical;

import java.util.Map;
import java.util.Map.Entry;

/**
 * Abstract representation of a system for which a branching process shall be
 * constructed using {@link DNodeBP}. The representation encodes system dynamics
 * in terms of events and their pre- and post-conditions in the spirit of Petri nets.
 * 
 * This abstract class provides that basic data structures used by {@link DNodeBP}.
 * Subclass to implement constructors for building a suitable representation for
 * your system.
 * 
 * The abstract representation uses {@link DNode}s to represent static and dynamic
 * parts of system behavior. {@link DNode#isEvent} distinguishes events and conditions. 
 * 
 * @author Dirk Fahland
 */
public abstract class DNodeSys {

  /**
   * The abstract representation uses numeric IDs instead of Strings to distinguish
   * system parts. This arrays maps the IDs back to their original strings. It
   * has to be filled by suitable constructors in a subclass.
   * Use {@link #finalize_setProperNames()} to set is values consistently with
   * {@link #nameToID}. 
   */
	public String				properNames[];		  // translate IDs to names
	
	/**
	 * The reverse map from original strings to numeric identifiers. It is the
	 * developers responsibility to keep {@link #properNames} and this field
	 * consistent.
	 */
	public Map<String, Short>	nameToID;			// and names to IDs
	
	/**
	 * A {@link DNodeSys} has two kinds of events. This set contains all events of
	 * the system that can be fired.
	 */
	public DNode.SortedLinearList fireableEvents;
	
	/**
	 * A {@link DNodeSys} has two kinds of events. This set contains all events of
   * the system that occur in preconditions of {@link #fireableEvents}. These
   * precondition events cannot be fired by themselves.
	 */
	public DNode.SortedLinearList preConEvents;
	
	/**
	 * Stores all conditions that are pre-condition of an event. Use
	 * {@link #finalize_setPreConditions()} to set this field.
	 */
	public DNode[] 				preConditions;		// set of all preconditions of an event

	/**
	 * Every system has an initial state or more generally an initial run. The
	 * initial run is represented as an acylic set of {@link DNode}s.
	 */
	public DNodeSet				initialRun;
	
	/**
	 * The length of the longest history-based precondition in the system.
	 */
	public int					maxHistoryDepth;	// depth of the longest history of a transition 
	
	/**
	 * Default constructor for initializing standard fields. Needs to be called
	 * by every subclass.
	 */
	protected DNodeSys () {
		DNode.idGen = 0;				// reset IDs
		
		maxHistoryDepth = -1;
		DNode.translationTable = this;	// everything happens wrt. to this class
		
		fireableEvents = new DNode.SortedLinearList();
		preConEvents = new DNode.SortedLinearList();
	}

	/**
	 * Reverse the {@literal DNodeSys#nameToID} map and store it in
	 * {@literal DNodeSys#properNames}.
	 * 
	 * Assumes that {@literal DNodeSys#nameToID} contains entries
	 * <code>(name_i, i)</code> with indices i=0,...,(nameToID.size() - 1)
	 */
	protected void finalize_setProperNames () {
		properNames = new String[nameToID.size()];
		for (Entry<String,Short> line : nameToID.entrySet()) {
			properNames[line.getValue()] = line.getKey();
		}
	}

	/**
	 * Update data structure that stores all pre-conditions of all
	 * events in the system. This method must be called by an
	 * implementation after filling the preConEvents and fireableEvents 
	 */
	protected void finalize_setPreConditions () {
		// remember all preconditions of an event for checking
		// equivalent cuts
		int preCondNum = 0;
		for (DNode e : preConEvents) {
			preCondNum += e.pre.length;
		}
		for (DNode e : fireableEvents) {
			preCondNum += e.pre.length;
		}
		preConditions = new DNode[preCondNum];
		preCondNum = 0;
		for (DNode e : preConEvents) {
			for (DNode b : e.pre)
				preConditions[preCondNum++] = b;
		}
		for (DNode e : fireableEvents) {
			for (DNode b : e.pre)
				preConditions[preCondNum++] = b;
		}
	}
	
	/**
	 * @param d
	 * @return
	 *   <code>true</code> iff the given node <code>d</code> represents a terminal
	 *   node of the system, the actual interpretation of 'terminal' depends on the
	 *   actual system. Implementations should override this method accordingly.
	 */
	public boolean isTerminal(DNode d) {
	  return false;
	}
}
