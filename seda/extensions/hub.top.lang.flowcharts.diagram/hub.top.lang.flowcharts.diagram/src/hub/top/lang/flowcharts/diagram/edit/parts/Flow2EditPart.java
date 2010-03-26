/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.lang.flowcharts.diagram.edit.parts;

import hub.top.lang.flowcharts.diagram.edit.policies.Flow2ItemSemanticEditPolicy;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.NonResizableEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ShapeNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.draw2d.ui.figures.ConstrainedToolbarLayout;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class Flow2EditPart extends ShapeNodeEditPart {

  /**
   * @generated
   */
  public static final int VISUAL_ID = 2008;

  /**
   * @generated
   */
  protected IFigure contentPane;

  /**
   * @generated
   */
  protected IFigure primaryShape;

  /**
   * @generated
   */
  public Flow2EditPart(View view) {
    super(view);
  }

  /**
   * @generated
   */
  protected void createDefaultEditPolicies() {
    super.createDefaultEditPolicies();
    installEditPolicy(EditPolicyRoles.SEMANTIC_ROLE,
        new Flow2ItemSemanticEditPolicy());
    installEditPolicy(EditPolicy.LAYOUT_ROLE, createLayoutEditPolicy());
    // XXX need an SCR to runtime to have another abstract superclass that would let children add reasonable editpolicies
    // removeEditPolicy(org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles.CONNECTION_HANDLES_ROLE);
  }

  /**
   * @generated
   */
  protected LayoutEditPolicy createLayoutEditPolicy() {
    LayoutEditPolicy lep = new LayoutEditPolicy() {

      protected EditPolicy createChildEditPolicy(EditPart child) {
        EditPolicy result = child.getEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE);
        if (result == null) {
          result = new NonResizableEditPolicy();
        }
        return result;
      }

      protected Command getMoveChildrenCommand(Request request) {
        return null;
      }

      protected Command getCreateCommand(CreateRequest request) {
        return null;
      }
    };
    return lep;
  }

  /**
   * @generated
   */
  protected IFigure createNodeShape() {
    FlowFigure2 figure = new FlowFigure2();
    return primaryShape = figure;
  }

  /**
   * @generated
   */
  public FlowFigure2 getPrimaryShape() {
    return (FlowFigure2) primaryShape;
  }

  /**
   * @generated
   */
  protected boolean addFixedChild(EditPart childEditPart) {
    if (childEditPart instanceof FlowFlowCompartment2EditPart) {
      IFigure pane = getPrimaryShape().getFigureFlowFigure2Pane();
      setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
      pane.add(((FlowFlowCompartment2EditPart) childEditPart).getFigure());
      return true;
    }
    return false;
  }

  /**
   * @generated
   */
  protected boolean removeFixedChild(EditPart childEditPart) {

    if (childEditPart instanceof FlowFlowCompartment2EditPart) {
      IFigure pane = getPrimaryShape().getFigureFlowFigure2Pane();
      setupContentPane(pane); // FIXME each comparment should handle his content pane in his own way 
      pane.remove(((FlowFlowCompartment2EditPart) childEditPart).getFigure());
      return true;
    }
    return false;
  }

  /**
   * @generated
   */
  protected void addChildVisual(EditPart childEditPart, int index) {
    if (addFixedChild(childEditPart)) {
      return;
    }
    super.addChildVisual(childEditPart, -1);
  }

  /**
   * @generated
   */
  protected void removeChildVisual(EditPart childEditPart) {
    if (removeFixedChild(childEditPart)) {
      return;
    }
    super.removeChildVisual(childEditPart);
  }

  /**
   * @generated
   */
  protected IFigure getContentPaneFor(IGraphicalEditPart editPart) {

    if (editPart instanceof FlowFlowCompartment2EditPart) {
      return getPrimaryShape().getFigureFlowFigure2Pane();
    }
    return super.getContentPaneFor(editPart);
  }

  /**
   * @generated
   */
  protected NodeFigure createNodePlate() {
    DefaultSizeNodeFigure result = new DefaultSizeNodeFigure(getMapMode()
        .DPtoLP(100), getMapMode().DPtoLP(110));
    return result;
  }

  /**
   * Creates figure for this edit part.
   * 
   * Body of this method does not depend on settings in generation model
   * so you may safely remove <i>generated</i> tag and modify it.
   * 
   * @generated
   */
  protected NodeFigure createNodeFigure() {
    NodeFigure figure = createNodePlate();
    figure.setLayoutManager(new StackLayout());
    IFigure shape = createNodeShape();
    figure.add(shape);
    contentPane = setupContentPane(shape);
    return figure;
  }

  /**
   * Default implementation treats passed figure as content pane.
   * Respects layout one may have set for generated figure.
   * @param nodeShape instance of generated figure class
   * @generated
   */
  protected IFigure setupContentPane(IFigure nodeShape) {
    if (nodeShape.getLayoutManager() == null) {
      ConstrainedToolbarLayout layout = new ConstrainedToolbarLayout();
      layout.setSpacing(getMapMode().DPtoLP(5));
      nodeShape.setLayoutManager(layout);
    }
    return nodeShape; // use nodeShape itself as contentPane
  }

  /**
   * @generated
   */
  public IFigure getContentPane() {
    if (contentPane != null) {
      return contentPane;
    }
    return super.getContentPane();
  }

  /**
   * @generated
   */
  public class FlowFigure2 extends RectangleFigure {

    /**
     * @generated
     */
    private RectangleFigure fFigureFlowFigure2Pane;

    /**
     * @generated
     */
    public FlowFigure2() {
      this.setFill(false);
      this.setOutline(false);
      this.setPreferredSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(110)));
      this.setMinimumSize(new Dimension(getMapMode().DPtoLP(100), getMapMode()
          .DPtoLP(110)));

      this.setBorder(new MarginBorder(getMapMode().DPtoLP(0), getMapMode()
          .DPtoLP(0), getMapMode().DPtoLP(0), getMapMode().DPtoLP(0)));
      createContents();
    }

    /**
     * @generated
     */
    private void createContents() {

      RectangleFigure flow2Title0 = new RectangleFigure();
      flow2Title0.setBackgroundColor(FLOW2TITLE0_BACK);
      flow2Title0.setPreferredSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(5)));
      flow2Title0.setMaximumSize(new Dimension(getMapMode().DPtoLP(1000),
          getMapMode().DPtoLP(5)));
      flow2Title0.setMinimumSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(5)));

      this.add(flow2Title0);

      RectangleFigure flow2Body0 = new RectangleFigure();
      flow2Body0.setPreferredSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(90)));
      flow2Body0.setMinimumSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(90)));

      this.add(flow2Body0);
      flow2Body0.setLayoutManager(new StackLayout());

      RectangleFigure flow2Background1 = new RectangleFigure();
      flow2Background1.setBackgroundColor(FLOW2BACKGROUND1_BACK);
      flow2Background1.setPreferredSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(90)));
      flow2Background1.setMinimumSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(90)));

      flow2Body0.add(flow2Background1);

      fFigureFlowFigure2Pane = new RectangleFigure();
      fFigureFlowFigure2Pane.setFill(false);
      fFigureFlowFigure2Pane.setOutline(false);
      fFigureFlowFigure2Pane.setPreferredSize(new Dimension(getMapMode()
          .DPtoLP(50), getMapMode().DPtoLP(50)));
      fFigureFlowFigure2Pane.setMinimumSize(new Dimension(getMapMode().DPtoLP(
          100), getMapMode().DPtoLP(90)));

      flow2Body0.add(fFigureFlowFigure2Pane);

      RectangleFigure flow2Bottom0 = new RectangleFigure();
      flow2Bottom0.setBackgroundColor(FLOW2BOTTOM0_BACK);
      flow2Bottom0.setPreferredSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(5)));
      flow2Bottom0.setMaximumSize(new Dimension(getMapMode().DPtoLP(1000),
          getMapMode().DPtoLP(5)));
      flow2Bottom0.setMinimumSize(new Dimension(getMapMode().DPtoLP(100),
          getMapMode().DPtoLP(5)));

      this.add(flow2Bottom0);

    }

    /**
     * @generated
     */
    private boolean myUseLocalCoordinates = false;

    /**
     * @generated
     */
    protected boolean useLocalCoordinates() {
      return myUseLocalCoordinates;
    }

    /**
     * @generated
     */
    protected void setUseLocalCoordinates(boolean useLocalCoordinates) {
      myUseLocalCoordinates = useLocalCoordinates;
    }

    /**
     * @generated
     */
    public RectangleFigure getFigureFlowFigure2Pane() {
      return fFigureFlowFigure2Pane;
    }

  }

  /**
   * @generated
   */
  static final Color FLOW2TITLE0_BACK = new Color(null, 0, 0, 0);

  /**
   * @generated
   */
  static final Color FLOW2BACKGROUND1_BACK = new Color(null, 250, 250, 250);

  /**
   * @generated
   */
  static final Color FLOW2BOTTOM0_BACK = new Color(null, 0, 0, 0);

}
