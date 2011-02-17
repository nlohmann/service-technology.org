/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
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

import hub.top.petrinet.unfold.DNodeSys_PetriNet;

/**
 * Stores all options used in the construction of the branching process 
 */
public class Options {
  
  /**
   * Default options, calls {@link #configure_DefaultPrefix()}
   * @param sys set system specific configuration 
   */
  public Options (DNodeSys sys) {
    
    configure_DefaultPrefix();
    
    if (sys instanceof DNodeSys_PetriNet) configure_PetriNet();
    else configure_Scenarios();
  }

  /**
   * Default settings to construct a McMillan prefix of a system
   */
  public void configure_DefaultPrefix() {
    searchStrat_size = true;             // compute prefix using lexicographic strategy
    cutOffEquiv_conditionHistory = true; // cutoff equivalence determined by condition histories
    
    checkProperties = false;             // do not check for properties
    configure_setBound(0);
    
    cutOffTermination_reachability = true; // compute until all reachable markings and transitions
                                           // have been seen
  }
  
  /**
   * configure unfolding algorithm for scenario-based specifications
   */
  public void configure_Scenarios() {
    // search strategy: lexicographic
    searchStrat_size = true;
    searchStrat_predecessor = false;
    searchStrat_lexicographic = true;
    
    // determine equivalence of cuts by history
    cutOffEquiv_conditionHistory = true;
    cutOffEquiv_marking = false;
  }
  
   /**
   * configure unfolding algorithm for Petri net semantics
   */
  public void configure_PetriNet() {
    // search strategy: predecessor
    searchStrat_size = true;
    searchStrat_predecessor = false;
    searchStrat_lexicographic = true;
    
    // determine equivalence of cuts by markings
    cutOffEquiv_conditionHistory = true;
    cutOffEquiv_marking = false;
  }
  
  /**
   * configure unfolding algorithm to construct BP only
   */
  public void configure_buildOnly() {
      checkProperties = false;
      checkProperty_DeadCondition = false;
      
      configure_setBound(0);
      
      cutOffTermination_reachability = true;
  }
  
  /**
   * configure unfolding algorithm to construct BP only
   */
  public void configure_checkProperties() {
      checkProperties = true;
      configure_setBound(1);
      checkProperty_DeadCondition = true;
  }
  
  /**
   * configure unfolding algorithm to stop if system is unsafe
   */
  public void configure_stopIfUnSafe() {
      checkProperties = true;
      configure_setBound(1);
  }
  
  /**
   * configure unfolding algorithm to stop if system is unsafe
   */
  public void configure_synthesis() {
    checkProperties = false;
    checkProperty_DeadCondition = false;
    configure_setBound(0);
      
    cutOffTermination_reachability = false;
    cutOffTermination_synthesis = true;
  }
  
  /**
   * set bound of computed prefix, i.e. maximum number of conditions
   * with the same label in a cut
   * 
   * @param k
   */
  public void configure_setBound(int k) {
    boundToCheck = k;
  }
  
  /**
   * configure unfolding algorithm to construct BP only
   */
  public void configure_checkSoundness() {
    checkProperties = true;
    configure_setBound(1);
    checkProperty_DeadCondition = true;
  }
  
  /**
   * Construct BP to check whether the event with the given
   * label ID is executable in the system.
   * 
   * @param eventID
   */
  public void configure_checkExecutable(short eventID) {
    checkProperties = true;
    checkProperty_ExecutableEventID = eventID;
    cutOffTermination_reachability = true;
  }
  
   /**
   * Construct BP to check whether the given event from
   * {@link DNodeSys#fireableEvents} is executable.
   * 
   * @param event
   */
  public void configure_checkExecutable(DNode event) {
    checkProperties = true;
    checkProperty_ExecutableEvent = event.globalId;
    cutOffTermination_reachability = true;
  }
  
  /**
   * For simplifying system models based on their partial-order
   * behavior.
   */
  public void configure_simplifyNet() {
    checkProperties = false;
    folding_extendEquivalence_forward = false;
  } 
  
  //// --- search strategies for finding cut-off events
  /**
   *  check for cut-off events using the lexicographic order on events:
   *  compare each candidate event only with events that are predecessors
   *  of the candidate event
   */
  public boolean searchStrat_predecessor = false;
  /**
   *  check for cut-off events using the size of the local configurations:
   *  compare each candidate event only with events that have been added
   *  previously and which have a strictly smaller prime configuration
   */
  public boolean searchStrat_size = false;
  /**
   *  check for cut-off events using a lexicographic order in the transition names:
   *  compare each candidate event only with events that have been added
   *  previously and which have a smaller or equal prime configuration, if
   *  two configurations have equal size, then their lexicographic order determines
   *  the order of the configuration
   */
  public boolean searchStrat_lexicographic = false;
  
  //// --- equivalence notions for detecting whether two events are equivalent
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  enabling the same sets of events
   */
  public boolean cutOffEquiv_eventSignature = false;
  /**
   *  check for equivalence of cuts by comparing their histories for
   *  isomorphism to the maximal depth of the histories in the input system
   */
  public boolean cutOffEquiv_history = false;
  /**
   * check for equivalence of cuts by comparing the reached markings only
   */
  public boolean cutOffEquiv_marking = false;
  /**
   *  check for equivalence of cuts by comparing the histories of conditions
   *  with respect to the conditions of the given system
   */
  public boolean cutOffEquiv_conditionHistory = false;
  
  /**
   * compute finite complete prefix until all reachable states have been
   * computed (default: <code>true</code>)
   */
  public boolean cutOffTermination_reachability = true;
  /**
   * compute finite complete prefix until finite complete prefix is successor
   * complete and can be folded into an equivalent Petri net
   * (default: <code>false</code>)
   */
  public boolean cutOffTermination_synthesis = false;


  //// --- synthesis flags
  /**
   * extend the folding equivalence forward based on identified cut-off events,
   * set to <code>false</code> if no cut-off events are computed
   * (default: <code>true</code>)
   */
  public boolean folding_extendEquivalence_forward = true;

  
  //// --- analysis flags
  /**
   * whether properties shall be checked on the fly at all
   */
  public boolean checkProperties = false;
  /**
   * check whether the system is k-bounded
   */
  public int boundToCheck = 0;
  /**
   * check whether the system has dead conditions (which have no post-event
   */
  public boolean checkProperty_DeadCondition = false;
  /**
   * stop BP construction if the property to be checked has been found
   */
  public boolean stopIfPropertyFound = true;
  /**
   * check whether the event with the given label-ID can be executed 
   */
  public short checkProperty_ExecutableEventID = -1;
  /**
   * check whether the given event (in terms of its global ID from
   * {@link DNodeSys}) can be executed 
   */
  public int checkProperty_ExecutableEvent = -1;
  
}