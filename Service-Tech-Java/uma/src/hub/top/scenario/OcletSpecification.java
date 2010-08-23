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

package hub.top.scenario;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.InvalidModelException;

import java.util.HashMap;
import java.util.LinkedList;

public class OcletSpecification {

  private Oclet initialRun;
  private LinkedList<Oclet> oclets;
  
  /**
   * create an empty oclet specification
   */
  public OcletSpecification() {
    initialRun = null;
    oclets = new LinkedList<Oclet>();
  }
  
  /**
   * Read a Petri net of which the nodes are labeled to distinguish
   * oclets, histories, and anti-oclets according to {@link OcletIdentifier},
   * and split this Petri net into the described {@link Oclet}s.
   * 
   * @param net
   * 
   * @throws InvalidModelException if 'net' does not decompose into proper oclets
   */
  public OcletSpecification(PetriNet net) {
    this();
    splitNetIntoOclets(net);
  }
  
  /**
   * Add an oclet to the specification.
   * 
   * @param o
   */
  public void addOclet(Oclet o) {
    oclets.add(o);
  }
  
  /**
   * @return all oclet of the specification
   */
  public LinkedList<Oclet> getOclets() {
    return oclets;
  }
  
  /**
   * set the initial run of the specification
   * @param o
   */
  public void setInitialRun(Oclet o) {
    initialRun = o;
  }
  
  /**
   * @return the initial run of the specification
   */
  public Oclet getInitialRun() {
    return initialRun;
  }
  
  /**
   * Takes a Petri net and splits it into oclets if the names of the
   * net's nodes are as specified by {@link OcletIdentifier}.
   * 
   * @param net
   */
  private void splitNetIntoOclets(PetriNet net) {
    HashMap<String, Oclet> ocletMap = new HashMap<String, Oclet>();
    HashMap<Place, Place> placeMap = new HashMap<Place, Place>();
    
    for (Place p : net.getPlaces()) {
      OcletIdentifier oid = new OcletIdentifier(p.getName());
      if (!ocletMap.containsKey(oid.ocletName)) {
        
        Oclet o = new Oclet(oid.ocletName, oid.isAnti);
        ocletMap.put(oid.ocletName, o);
      }
      
      Oclet o = ocletMap.get(oid.ocletName);
      Place pNew = o.addPlace(oid.nodeName, oid.isHist);
      placeMap.put(p, pNew);
      pNew.setTokens(p.getTokens());
    }
    
    for (Transition t : net.getTransitions()) {
      OcletIdentifier oid = new OcletIdentifier(t.getName());
      if (!ocletMap.containsKey(oid.ocletName)) {

        Oclet o = new Oclet(oid.ocletName, oid.isAnti);
        ocletMap.put(oid.ocletName, o);
      }
      
      Oclet o = ocletMap.get(oid.ocletName);
      
      // overwrite anti-values for oclets following from names of oclet transitions
      if (oid.isAnti) o.isAnti = true;
      
      Transition tNew = o.addTransition(oid.nodeName, oid.isHist);
      if (oid.isAnti) o.makeHotNode(tNew);
      for (Place p : t.getPreSet()) {
        o.addArc(placeMap.get(p), tNew);
      }
      for (Place p : t.getPostSet()) {
        o.addArc(tNew, placeMap.get(p));
        if (oid.isAnti) o.makeHotNode(placeMap.get(p));
      }
    }

    for (Oclet o : ocletMap.values()) {
      boolean hasHistory = false;
      for (Place p : o.getPlaces()) {
        if (o.isInHistory(p)) {
          hasHistory = true; break;
        }
      }
      if (hasHistory) addOclet(o);
      else setInitialRun(o);
    }
  }
  
  /**
   * @return <code>true</code> iff each oclet of this specification satisfies
   * all structural constraints of an oclet ({@link Oclet#isValidOclet()}).
   * 
   * @throws InvalidModelException if an oclet violates a constraint
   */
  public boolean isValidSpecification() throws InvalidModelException {
    if (initialRun != null) {
      if (!initialRun.isCausalNet())
        throw new InvalidModelException(InvalidModelException.OCLET_NO_CAUSALNET, initialRun);
    }
    
    for (Oclet o : oclets) {
      // check whether oclet is valid, and re-throw exception if invalid
      o.isValidOclet();
    }
    
    return true;
  }
  
  /**
   * Extract oclet name and node name from a given string, assuming the format
   * [!]OCLET_NAME[_SUFFIX]#NODE_NAME[_ID], where _SUFFIX and _ID are optional.
   */
  private static class OcletIdentifier {
    public String ocletName = null;
    public String ocletNameSuffix = null;
    public boolean isHist = false;
    public boolean isAnti = false;
    public String nodeName = null;
    public String nodeIdent = null;
    public String fullNodeName = null;
    
    public OcletIdentifier(String ident) {
      String ocletPrefix = ident.substring(0, ident.indexOf('#'));
      
      int ocletNameEnd = ocletPrefix.lastIndexOf('_');
      int ocletSuffixStart;
      
      if (ocletNameEnd == -1) { 
        ocletNameEnd = ocletPrefix.length();
        ocletSuffixStart = ocletNameEnd;
      } else {
        ocletSuffixStart = ocletNameEnd+1;
      }
      
      ocletName = ocletPrefix.substring(0, ocletNameEnd);
      ocletNameSuffix = ocletPrefix.substring(ocletSuffixStart);
      
      if ("hist".equals(ocletNameSuffix)) isHist = true;
      if (ocletName.charAt(0) == '!') {
        isAnti = true;
        ocletName = ocletName.substring(1);
      }
      
      fullNodeName = ident.substring(ident.indexOf('#')+1);
      int nodeNameEnd = fullNodeName.lastIndexOf('_');
      int nodeSuffixStart;
      
      if (nodeNameEnd == -1) { 
        nodeNameEnd = fullNodeName.length();
        nodeSuffixStart = nodeNameEnd;
      } else {
        nodeSuffixStart = nodeNameEnd+1;
      }

      nodeName = fullNodeName.substring(0, nodeNameEnd);
      nodeIdent = fullNodeName.substring(nodeSuffixStart);
    }
  }
}
