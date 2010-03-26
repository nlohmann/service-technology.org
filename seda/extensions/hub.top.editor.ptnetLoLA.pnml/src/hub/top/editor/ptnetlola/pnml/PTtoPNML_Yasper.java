/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
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
 *    Dirk Fahland
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

package hub.top.editor.ptnetlola.pnml;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;
import hub.top.lang.ptnet.yasper.YasperEPNML114.Net;
import hub.top.lang.ptnet.yasper.YasperEPNML114.Pnml;
import hub.top.lang.ptnet.yasper.YasperEPNML114.ToolspecificType;
import hub.top.lang.ptnet.yasper.YasperEPNML114.pnmlFactory;
import hub.top.lang.ptnet.yasper.YasperEPNML114.pnmlPackage;

import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;

import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.URIConverter;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.xmi.impl.GenericXMLResourceFactoryImpl;
import org.eclipse.emf.ecore.xml.type.XMLTypeDocumentRoot;
import org.eclipse.gmf.runtime.notation.Bounds;
import org.eclipse.gmf.runtime.notation.Diagram;

/**
*
* Convert a {@link PtNet} into a PNML representation that is compatible
* to the tool Yasper. Use this model transformation by creating
* a new {@link PTtoPNML_Yasper} object with the model to be converted. Then
* call {@link PTtoPNML_Yasper#process()} to invoke the transformation.
* 
* @author Dirk Fahland
*/
public class PTtoPNML_Yasper {

  // create a map from original places to PNML places for later reference 
  private EMap<Place, hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1> placeMap;
  // create a map from original transitions to PNML transitions for later reference
  private EMap<Transition, hub.top.lang.ptnet.yasper.YasperEPNML114.Transition> transitionMap;

  // source net
  private PtNet   net;
  // source diagram
  private Diagram diagram;
  
  // internal id for new nodes
  private int id = 1;

  // target document
  Pnml pnmlDoc;
  // target net
  Net _net;

  
  private pnmlFactory fact;
  
  /**
   * Create a new model transformation instance for translating one
   * Petri net (either EMF model of GMF diagram) into PNML. The model
   * object/the model of the diagram should be of type {@link PtNet}.
   *  
   * @param pnModel
   */
  public PTtoPNML_Yasper(EObject pnModel) {
    
    if (pnModel instanceof PtNet) {
      net = (PtNet)pnModel;
      diagram = null;
      
    } else if (pnModel instanceof Diagram) {
      diagram = (Diagram) pnModel;
      if (diagram.getElement() instanceof PtNet)
        net = (PtNet)diagram.getElement();
        
    } else {
      net = null;
      diagram = null;
    }
    
    placeMap = 
      new BasicEMap<Place, hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1>();
    transitionMap = 
      new BasicEMap<Transition, hub.top.lang.ptnet.yasper.YasperEPNML114.Transition>();
    
    fact = pnmlPackage.eINSTANCE.getpnmlFactory();
  }
  
  /**
   * Translate the Petri net model that was passed to the constructor
   * into PNML representation.
   * 
   * @return
   */
  public Pnml process () {
    if (net == null) return null;
    Pnml pnmlDoc = translateNet();
    if (diagram != null) transferLayout();
    return pnmlDoc;
  }
  
  /**
   * Translate stored {@link #net} into a Yasper-PNML-Net object. The method
   * will build up some internal translation tables mapping old to new
   * elements, e.g. {@link #placeMap} and {@link #transitionMap}
   * 
   * @return
   */
  private Pnml translateNet () {
    
    pnmlDoc = fact.createPnml();  // create the document
    _net = fact.createNet();      // create the model top-level class

    _net.setId("id"+(id++));
    _net.setType("http://www.yasper.org/specs/epnml-1.1");
    pnmlDoc.getNet().add(_net);
    
    // translate each place
    for (Place p : net.getPlaces()) {
      hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1 _p = fact.createPlaceType1();
      _p.setId("id"+(id++));

      // give it a name
      hub.top.lang.ptnet.yasper.YasperEPNML114.PnmlAnnotation _pName = fact.createPnmlAnnotation();
      _pName.setText(p.getName());
      _p.getName().add(_pName);
      
      // and a marking
      if (p.getToken() > 0) {
        hub.top.lang.ptnet.yasper.YasperEPNML114.InitialMarking _mark = fact.createInitialMarking();
        _mark.setText(new BigInteger(Integer.toString(p.getToken())));
        _p.getInitialMarking().add(_mark);
      }
      
      // <toolspecific>
      ToolspecificType toolSpecific = fact.createToolspecificType();
      toolSpecific.setTool("Yasper");
      toolSpecific.setVersion("1.2.1959.33714");
      _p.getToolspecific().add(toolSpecific);
      
      // create the XML text that shall be written
      // inside of <toolspecific> ... </toolspecific>, do not write
      // any xmlns="" attributes to the XML elements
      // xmlns=\"http://www.yasper.org/specs/epnml-1.1/toolspec\"
      String tokenCaseSensitiveString = 
        "<tokenCaseSensitive>"
      + "  <text>true</text>"
      + "</tokenCaseSensitive>";
      
      // and add this XML text as contents of <toolspecific>
      addXMLtoElement(tokenCaseSensitiveString, toolSpecific.getMixed());
      // </toolspecific>
      
      // and add it to the net
      _net.getPlace().add(_p);
      placeMap.put(p, _p);

      // for places with no incoming arcs add emitor
      boolean addPreEmitor = false;
      if (p instanceof PlaceExt && ((PlaceExt)p).isIsStart())
        addPreEmitor = true;
      else if (p.getToken() > 0 && p.getIncoming().isEmpty())
        addPreEmitor = true;
      else if (p.getToken() > 0 && !p.getIncoming().isEmpty() && !p.getOutgoing().isEmpty())
        addPreEmitor = true;

      if (addPreEmitor){
          hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = getEmitor();
          hub.top.lang.ptnet.yasper.YasperEPNML114.Arc _arc = fact.createArc();
          
          _arc.setId("id"+(id++));
          _arc.setSource(_t.getId());
          _arc.setTarget(_p.getId());
          
          _net.getArc().add(_arc);
      }
      
      // for places with no outgoing arcs add collector
      if (p.getOutgoing().isEmpty()
          || p.getToken() > 0 && !p.getIncoming().isEmpty() && !p.getOutgoing().isEmpty())
      {
    	  hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = getCollector();
        hub.top.lang.ptnet.yasper.YasperEPNML114.Arc _arc = fact.createArc();
        
        _arc.setId("id"+(id++));
        _arc.setSource(_p.getId());
        _arc.setTarget(_t.getId());
        
        _net.getArc().add(_arc);
      }
    }
    
    // translate each transition
    for (Transition t : net.getTransitions()) {
      hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = fact.createTransition();
      _t.setId("id"+(id++));
      // give it a name
      hub.top.lang.ptnet.yasper.YasperEPNML114.PnmlAnnotation _tName = fact.createPnmlAnnotation();
      _tName.setText(t.getName());
      _t.getName().add(_tName);

      if (t instanceof TransitionExt && (((TransitionExt)t).getCost() > 0 || ((TransitionExt)t).getMaxTime() > 0))
      {

        // <toolspecific>
        ToolspecificType toolSpecific = fact.createToolspecificType();
        toolSpecific.setTool("Yasper");
        toolSpecific.setVersion("1.2.1959.33714");
        _t.getToolspecific().add(toolSpecific);
      
        // create the XML text that shall be written
        // inside of <toolspecific> ... </toolspecific>, do not write
        // any xmlns="" attributes to the XML elements
        // xmlns=\"http://www.yasper.org/specs/epnml-1.1/toolspec\"
        String costString = 
      	  "<cost>" +
      	  "	<fixed>" +
      	  "	 <text>"+ ((TransitionExt) t).getCost() +"</text>" +
      	  "	</fixed>" +
      	  "	<variable>" +
      	  "  <text>0</text>" +
      	  "	</variable>" +
      	  "</cost>";
  
        // duchschnittsschaltdauer
        double mean = (((TransitionExt) t).getMaxTime() + ((TransitionExt) t).getMinTime())/2;
        // abweichung
        double dev = mean - ((TransitionExt) t).getMinTime();
        String timeString =
      	  "<processingTime >" +
            "  <mean>" +
            "   <text>" + mean + "</text>" +
            "  </mean>" +
            "  <deviation>" +
            "	  <text>" + dev + "</text>" +
            "  </deviation>" +
            "</processingTime>";
        
          // and add this XML text as contents of <toolspecific>
          addXMLtoElement(costString, toolSpecific.getMixed());
          addXMLtoElement(timeString, toolSpecific.getMixed());
          // </toolspecific>
      }
      
      // and add it to the net
      _net.getTransition().add(_t);
      transitionMap.put(t, _t);
      
      // and translate all adjacent arcs
      // arcs ending at the transition
      for (Place p : t.getPreSet()) {
        hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1 _p = placeMap.get(p);
        hub.top.lang.ptnet.yasper.YasperEPNML114.Arc _arc = fact.createArc();
        
        _arc.setId("id"+(id++));
        _arc.setSource(_p.getId());
        _arc.setTarget(_t.getId());
        
        _net.getArc().add(_arc);
      }

      // arcs leaving the transition
      //for (Place p : t.getPostSet()) {
      for (Arc a : t.getOutgoing()) {
        hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1 _p = placeMap.get(a.getTarget());
        for (int i = 1; i <= a.getWeight(); i++){
        	hub.top.lang.ptnet.yasper.YasperEPNML114.Arc _arc = fact.createArc();
            
            _arc.setId("id"+(id++));
            _arc.setSource(_t.getId());
            _arc.setTarget(_p.getId());
            
            _net.getArc().add(_arc);
        }
      }
    }
    
    return pnmlDoc;
  }
  
  /**
   * @return a valid toolspecific entry
   */
  private ToolspecificType createNewToolSpecific() {
    ToolspecificType toolSpecific = fact.createToolspecificType();
    toolSpecific.setTool("Yasper");
    toolSpecific.setVersion("1.2.1959.33714");
    return toolSpecific;
  }
  
  /**
   * @param _t
   * @return
   *    the toolspecific element of the Transition _t, creates a new toolspecific
   *    element if no such toolspecific element is defined yet
   */
  protected ToolspecificType getToolSpecific(hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t) {
    if (_t.getToolspecific().size() > 0)
      return _t.getToolspecific().get(0);
    ToolspecificType toolSpecific = createNewToolSpecific();
    _t.getToolspecific().add(toolSpecific);
    return toolSpecific;
  }
  
  /**
   * @param _p
   * @return
   *    the toolspecific element of the Place _p, creates a new toolspecific
   *    element if no such toolspecific element is defined yet
   */
  protected ToolspecificType getToolSpecific(hub.top.lang.ptnet.yasper.YasperEPNML114.Place _p) {
    if (_p.getToolspecific().size() > 0)
      return _p.getToolspecific().get(0);
    ToolspecificType toolSpecific = createNewToolSpecific();
    _p.getToolspecific().add(toolSpecific);
    return toolSpecific;
  }  
  
  /**
   * Assign transition _t a cost specification: the cost of firing _t is
   * fixedCost + r where r is a random value between 0 and varCost
   * 
   * @param _t
   * @param fixedCost
   * @param varCost
   */
  protected void addCostSpecification(hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t, double fixedCost, double varCost) {
    // create the XML text that shall be written
    // inside of <toolspecific> ... </toolspecific>, do not write
    // any xmlns="" attributes to the XML elements
    // xmlns=\"http://www.yasper.org/specs/epnml-1.1/toolspec\"
    String costString = 
      "<cost>" +
      " <fixed>" +
      "  <text>"+ fixedCost +"</text>" +
      " </fixed>" +
      " <variable>" +
      "  <text>"+ varCost +"</text>" +
      " </variable>" +
      "</cost>";
    
    // and add this XML text as contents of <toolspecific>
    addXMLtoElement(costString, getToolSpecific(_t).getMixed());
  }
  
  /**
   * Assign transition _t a timing specification: the time _t needs to execute
   * is a random value between minTime and maxTime.
   * 
   * @param _t
   * @param minTime
   * @param maxTime
   */
  protected void addTimeSpecification(hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t, double minTime, double maxTime) {
    // average duration
    double mean = (maxTime + minTime)/2;
    // deviation
    double dev = mean - minTime;
    String timeString =
      "<processingTime >" +
        "  <mean>" +
        "   <text>" + mean + "</text>" +
        "  </mean>" +
        "  <deviation>" +
        "   <text>" + dev + "</text>" +
        "  </deviation>" +
        "</processingTime>";
    
    getToolSpecific(_t).getMixed();

    // and add this XML text as contents of <toolspecific>
    addXMLtoElement(timeString, getToolSpecific(_t).getMixed());
  }
  
  /**
   * Make place _p case sensitive in Yasper.
   * 
   * @param _p
   */
  protected void makeCaseSensitive(hub.top.lang.ptnet.yasper.YasperEPNML114.Place _p) {
    // <toolspecific>
    // create the XML text that shall be written
    // inside of <toolspecific> ... </toolspecific>, do not write
    // any xmlns="" attributes to the XML elements
    // xmlns=\"http://www.yasper.org/specs/epnml-1.1/toolspec\"
    String tokenCaseSensitiveString = 
      "<tokenCaseSensitive>"
    + "  <text>true</text>"
    + "</tokenCaseSensitive>";
    
    // and add this XML text as contents of <toolspecific>
    addXMLtoElement(tokenCaseSensitiveString, getToolSpecific(_p).getMixed());
    // </toolspecific>
  }
  
  /**
   * every simulation net should have only one emittor transition unless overlapping
   * cases are handled
   */
  private hub.top.lang.ptnet.yasper.YasperEPNML114.Transition uniqueEmitor = null;
  
  /**
   * every simulation net should have only one collector transition unless overlapping
   * cases are handled
   */
  private hub.top.lang.ptnet.yasper.YasperEPNML114.Transition uniqueCollector = null;
  
  /**
   * @return an emitor transition, returns the {@link #uniqueEmitor} if it is set,
   * otherwise it creates a new emitor transition and sets {@link #uniqueEmitor}
   */
  private hub.top.lang.ptnet.yasper.YasperEPNML114.Transition getEmitor() {
    
    if (uniqueEmitor == null) {
    
      hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = fact.createTransition();
      _t.setId("id"+(id++));
      
      ToolspecificType toolSpec = fact.createToolspecificType();
      toolSpec.setTool("Yasper");
      toolSpec.setVersion("1.2.1959.33714");
      _t.getToolspecific().add(toolSpec);
      
      int emitorRate = 1000;
      int emitorDeviation = 0;
      
      String emitorString = 
        "<emitor>" +
        "  <text>true</text>" +
        "</emitor>";
      String timeString =      
        "<processingTime>" +
        "  <mean>" +
        "    <text>"+emitorRate+"</text>" +
        "  </mean>" +
        "  <deviation>" +
        "    <text>"+emitorDeviation+"</text>" +
        "  </deviation>" +
        "</processingTime>";
      
      addXMLtoElement(emitorString, toolSpec.getMixed());
      addXMLtoElement(timeString, toolSpec.getMixed());
      _net.getTransition().add(_t);
      
      uniqueEmitor = _t;
    }
    return uniqueEmitor;
  }

  /**
   * @return a collector transition, returns the {@link #uniqueCollector} if it is set,
   * otherwise it creates a new collector transition and sets {@link #uniqueCollector}
   */
  private hub.top.lang.ptnet.yasper.YasperEPNML114.Transition getCollector() {

    if (uniqueCollector == null) {
      hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = fact.createTransition();
      _t.setId("id"+(id++));
      
      ToolspecificType toolSpec = fact.createToolspecificType();
      toolSpec.setTool("Yasper");
      toolSpec.setVersion("1.2.1959.33714");
      _t.getToolspecific().add(toolSpec);
      
      String collectorString = 
        "<collector>" +
        "  <text>true</text>" +
        "</collector>";
      
      addXMLtoElement(collectorString, toolSpec.getMixed());
      _net.getTransition().add(_t);
      uniqueCollector = _t;
    }
    return uniqueCollector;
  }


  
  /**
   * Parses an XML string and adds the parsed Object tree as contents
   * to the given {@link FeatureMap} <code>parentElementContents</code>
   * 
   * @param xmlString
   * @param parentElementContents
   */
  private static void addXMLtoElement(String xmlString, FeatureMap parentElementContents) {
    
    GenericXMLResourceFactoryImpl fact = new GenericXMLResourceFactoryImpl();
    Resource resource = fact.createResource(URI.createURI(""));
    try {
      
      resource.load(new URIConverter.ReadableInputStream(xmlString), null);
      XMLTypeDocumentRoot root = (XMLTypeDocumentRoot)resource.getContents().get(0);
      parentElementContents.add(root.getMixed().get(0));
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Error during Yasper export. Could not create XML contents for the toolspecific tag.", e);
    }
  }
  
  /**
   * Read the layout information from the diagram and write the
   * coordinates to the resulting nodes. Uses the placeMap and transitionMap
   * for relating old nodes to new nodes. 
   */
  @SuppressWarnings("unchecked")
  private void transferLayout ()
  {
    EList<org.eclipse.gmf.runtime.notation.Node> nodes = diagram.getChildren();
    for (org.eclipse.gmf.runtime.notation.Node node : nodes) {
      if (node.getElement() instanceof Place) {
        Place p = (Place) node.getElement();
        hub.top.lang.ptnet.yasper.YasperEPNML114.PlaceType1 _p = placeMap.get(p);
        
        // if diagram node has bounds-layout (x,y,w,h):
        if (node.getLayoutConstraint() instanceof Bounds) {
          Bounds bound = (Bounds) node.getLayoutConstraint();
          hub.top.lang.ptnet.yasper.YasperEPNML114.NodeGraphics _ng = fact.createNodeGraphics();
          hub.top.lang.ptnet.yasper.YasperEPNML114.TwoDimVector _pos = fact.createTwoDimVector();
          _pos.setX(new BigDecimal(bound.getX()));
          _pos.setY(new BigDecimal(bound.getY()));
          _ng.getPosition().add(_pos);
          _p.getGraphics().add(_ng);
        }
      }
      
      if (node.getElement() instanceof Transition) {
        Transition t = (Transition) node.getElement();
        hub.top.lang.ptnet.yasper.YasperEPNML114.Transition _t = transitionMap.get(t);
        
        // if diagram node has bounds-layout (x,y,w,h):
        if (node.getLayoutConstraint() instanceof Bounds) {
          Bounds bound = (Bounds) node.getLayoutConstraint();
          hub.top.lang.ptnet.yasper.YasperEPNML114.NodeGraphics _ng = fact.createNodeGraphics();
          hub.top.lang.ptnet.yasper.YasperEPNML114.TwoDimVector _pos = fact.createTwoDimVector();
          _pos.setX(new BigDecimal(bound.getX()));
          _pos.setY(new BigDecimal(bound.getY()));
          _ng.getPosition().add(_pos);
          _t.getGraphics().add(_ng);
        }
      }
    }
  }
}
