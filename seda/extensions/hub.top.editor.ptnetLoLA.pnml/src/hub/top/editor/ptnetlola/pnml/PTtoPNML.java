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

import fr.lip6.move.pnml.ptnet.PetriNetDoc;
import fr.lip6.move.pnml.ptnet.PtnetFactory;
import fr.lip6.move.pnml.ptnet.PtnetPackage;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.Transition;

import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.notation.Bounds;
import org.eclipse.gmf.runtime.notation.Diagram;

public class PTtoPNML {
  

  public static PetriNetDoc transformModel (EObject pnModel)
  {
    if (pnModel instanceof PtNet)
    {
      int id = 1;
      
      PtNet net = (PtNet)pnModel;
      
      PtnetFactory ptf = PtnetPackage.eINSTANCE.getPtnetFactory();
  
      // create the document
      PetriNetDoc pnmlDoc = ptf.createPetriNetDoc();
      //create the model top-level class.
      fr.lip6.move.pnml.ptnet.PetriNet _net = ptf.createPetriNet();
      _net.setId("id"+(id++));
      pnmlDoc.getNets().add(_net);
      
      //we set the model name.
      fr.lip6.move.pnml.ptnet.Name _modelName = ptf.createName(); 
      _modelName.setText("name1");
      _net.setName(_modelName);
      //Every PNML model must have at least one Page.
      fr.lip6.move.pnml.ptnet.Page _firstPage = ptf.createPage();
      _firstPage.setId("page1");
      _net.getPages().add(_firstPage);
      
      EMap<Place, fr.lip6.move.pnml.ptnet.Place> placeMap = 
        new BasicEMap<Place, fr.lip6.move.pnml.ptnet.Place>();
      
      for (Place p : net.getPlaces()) {
        fr.lip6.move.pnml.ptnet.Place _p = ptf.createPlace();
        _p.setId("id"+(id++));
        fr.lip6.move.pnml.ptnet.Name _pName = ptf.createName();
        _pName.setText(p.getName());
        _p.setName(_pName);
        if (p.getToken() > 0) {
          fr.lip6.move.pnml.ptnet.PTMarking _mark = ptf.createPTMarking();
          _mark.setText(new Integer(p.getToken()));
          _p.setInitialMarking(_mark);
        }
        _firstPage.getObjects().add(_p);
        placeMap.put(p, _p);
      }
      
      for (Transition t : net.getTransitions()) {
        fr.lip6.move.pnml.ptnet.Transition _t = ptf.createTransition();
        _t.setId("id"+(id++));
        fr.lip6.move.pnml.ptnet.Name _tName = ptf.createName();
        _tName.setText(t.getName());
        _t.setName(_tName);
        _firstPage.getObjects().add(_t);
        
        for (Place p : t.getPreSet()) {
          fr.lip6.move.pnml.ptnet.Place _p = placeMap.get(p);
          fr.lip6.move.pnml.ptnet.Arc _arc = ptf.createArc();
          
          _arc.setId("id"+(id++));
          _arc.setSource(_p);
          _arc.setTarget(_t);
          
          _firstPage.getObjects().add(_arc);
        }
        
        for (Place p : t.getPostSet()) {
          fr.lip6.move.pnml.ptnet.Place _p = placeMap.get(p);
          fr.lip6.move.pnml.ptnet.Arc _arc = ptf.createArc();
          
          _arc.setId("id"+(id++));
          _arc.setSource(_t);
          _arc.setTarget(_p);
          
          _firstPage.getObjects().add(_arc);
        }
      }
      
      return pnmlDoc;
    
    } else if (pnModel instanceof org.eclipse.gmf.runtime.notation.Diagram) {
      Diagram diag = (Diagram) pnModel;
      EObject underlyingModel = diag.getElement();
      
      if (underlyingModel instanceof PtNet) {
  
        PtNet net = (PtNet) underlyingModel;
        int id = 1;
        
        PtnetFactory ptf = PtnetPackage.eINSTANCE.getPtnetFactory();
  
        // create the document
        PetriNetDoc pnmlDoc = ptf.createPetriNetDoc();
        //create the model top-level class.
        fr.lip6.move.pnml.ptnet.PetriNet _net = ptf.createPetriNet();
        _net.setId("id"+(id++));
        pnmlDoc.getNets().add(_net);
        
        //we set the model name.
        fr.lip6.move.pnml.ptnet.Name _modelName = ptf.createName(); 
        _modelName.setText("name1");
        _net.setName(_modelName);
        //Every PNML model must have at least one Page.
        fr.lip6.move.pnml.ptnet.Page _firstPage = ptf.createPage();
        _firstPage.setId("page1");
        _net.getPages().add(_firstPage);
        
        EMap<Place, fr.lip6.move.pnml.ptnet.Place> placeMap = 
          new BasicEMap<Place, fr.lip6.move.pnml.ptnet.Place>();
        EMap<Transition, fr.lip6.move.pnml.ptnet.Transition> transitionMap = 
          new BasicEMap<Transition, fr.lip6.move.pnml.ptnet.Transition>();
        
        for (Place p : net.getPlaces()) {
          fr.lip6.move.pnml.ptnet.Place _p = ptf.createPlace();
          _p.setId("id"+(id++));
          fr.lip6.move.pnml.ptnet.Name _pName = ptf.createName();
          _pName.setText(p.getName());
          _p.setName(_pName);
          if (p.getToken() > 0) {
            fr.lip6.move.pnml.ptnet.PTMarking _mark = ptf.createPTMarking();
            _mark.setText(new Integer(p.getToken()));
            _p.setInitialMarking(_mark);
          }
          _firstPage.getObjects().add(_p);
          placeMap.put(p, _p);
        }
        
        for (Transition t : net.getTransitions()) {
          fr.lip6.move.pnml.ptnet.Transition _t = ptf.createTransition();
          _t.setId("id"+(id++));
          fr.lip6.move.pnml.ptnet.Name _tName = ptf.createName();
          _tName.setText(t.getName());
          _t.setName(_tName);
          _firstPage.getObjects().add(_t);
          transitionMap.put(t, _t);
          
          for (Place p : t.getPreSet()) {
            fr.lip6.move.pnml.ptnet.Place _p = placeMap.get(p);
            fr.lip6.move.pnml.ptnet.Arc _arc = ptf.createArc();
            
            _arc.setId("id"+(id++));
            _arc.setSource(_p);
            _arc.setTarget(_t);
            
            _firstPage.getObjects().add(_arc);
          }
          
          for (Place p : t.getPostSet()) {
            fr.lip6.move.pnml.ptnet.Place _p = placeMap.get(p);
            fr.lip6.move.pnml.ptnet.Arc _arc = ptf.createArc();
            
            _arc.setId("id"+(id++));
            _arc.setSource(_t);
            _arc.setTarget(_p);
            
            _firstPage.getObjects().add(_arc);
          }
        }
        
        EList<org.eclipse.gmf.runtime.notation.Node> nodes = diag.getChildren();
        for (org.eclipse.gmf.runtime.notation.Node node : nodes) {
          if (node.getElement() instanceof Place) {
            Place p = (Place) node.getElement();
            fr.lip6.move.pnml.ptnet.Place _p = placeMap.get(p);
            
            // if diagram node has bounds-layout (x,y,w,h):
            if (node.getLayoutConstraint() instanceof Bounds) {
              Bounds bound = (Bounds) node.getLayoutConstraint();
              fr.lip6.move.pnml.ptnet.NodeGraphics _ng = ptf.createNodeGraphics();
              fr.lip6.move.pnml.ptnet.Position _pos = ptf.createPosition();
              _pos.setX(new Integer(bound.getX()));
              _pos.setY(new Integer(bound.getY()));
              _ng.setPosition(_pos);
              _p.setNodegraphics(_ng);
            }
          }
          
          if (node.getElement() instanceof Transition) {
            Transition t = (Transition) node.getElement();
            fr.lip6.move.pnml.ptnet.Transition _t = transitionMap.get(t);
            
            // if diagram node has bounds-layout (x,y,w,h):
            if (node.getLayoutConstraint() instanceof Bounds) {
              Bounds bound = (Bounds) node.getLayoutConstraint();
              fr.lip6.move.pnml.ptnet.NodeGraphics _ng = ptf.createNodeGraphics();
              fr.lip6.move.pnml.ptnet.Position _pos = ptf.createPosition();
              _pos.setX(new Integer(bound.getX()));
              _pos.setY(new Integer(bound.getY()));
              _ng.setPosition(_pos);
              _t.setNodegraphics(_ng);
            }
          }
        }
        
        /*
        EList<EObject> edge = diag.getEdges();
        for (EObject ed : edge) {
          System.err.println(ed+ " "+ed.getClass());
        }
        */
        return pnmlDoc;
      } // underlying model PtNet
        
      return null;
    }
    return null;
  }
}
