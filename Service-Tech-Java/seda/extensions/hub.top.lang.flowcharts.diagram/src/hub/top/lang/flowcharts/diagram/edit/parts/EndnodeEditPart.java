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

import hub.top.lang.flowcharts.diagram.edit.policies.EndnodeItemSemanticEditPolicy;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import org.eclipse.draw2d.FlowLayout;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.RoundedRectangle;
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
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.gef.ui.figures.DefaultSizeNodeFigure;
import org.eclipse.gmf.runtime.gef.ui.figures.NodeFigure;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.swt.graphics.Color;

/**
 * @generated
 */
public class EndnodeEditPart extends ShapeNodeEditPart {

  /**
   * @generated
   */
  public static final int VISUAL_ID = 2002;

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
  public EndnodeEditPart(View view) {
    super(view);
  }

  /**
   * @generated
   */
  protected void createDefaultEditPolicies() {
    super.createDefaultEditPolicies();
    installEditPolicy(EditPolicyRoles.SEMANTIC_ROLE,
        new EndnodeItemSemanticEditPolicy());
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
    StartEndNodeFigure figure = new StartEndNodeFigure();
    return primaryShape = figure;
  }

  /**
   * @generated
   */
  public StartEndNodeFigure getPrimaryShape() {
    return (StartEndNodeFigure) primaryShape;
  }

  /**
   * @generated
   */
  protected boolean addFixedChild(EditPart childEditPart) {
    if (childEditPart instanceof EndnodeLabelEditPart) {
      ((EndnodeLabelEditPart) childEditPart).setLabel(getPrimaryShape()
          .getFigureStartEndNodeLabelFigure());
      return true;
    }
    return false;
  }

  /**
   * @generated
   */
  protected boolean removeFixedChild(EditPart childEditPart) {

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

    return super.getContentPaneFor(editPart);
  }

  /**
   * @generated
   */
  protected NodeFigure createNodePlate() {
    DefaultSizeNodeFigure result = new DefaultSizeNodeFigure(getMapMode()
        .DPtoLP(40), getMapMode().DPtoLP(40));
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
  public EditPart getPrimaryChildEditPart() {
    return getChildBySemanticHint(FlowchartVisualIDRegistry
        .getType(EndnodeLabelEditPart.VISUAL_ID));
  }

  /**
   * @generated
   */
  public class StartEndNodeFigure extends RectangleFigure {

    /**
     * @generated
     */
    private WrappingLabel fFigureStartEndNodeLabelFigure;

    /**
     * @generated
     */
    public StartEndNodeFigure() {
      this.setLayoutManager(new StackLayout());
      this.setFill(false);
      this.setOutline(false);
      createContents();
    }

    /**
     * @generated
     */
    private void createContents() {

      RoundedRectangle startEndNodeFigure0 = new RoundedRectangle();
      startEndNodeFigure0.setCornerDimensions(new Dimension(getMapMode()
          .DPtoLP(8), getMapMode().DPtoLP(8)));
      startEndNodeFigure0.setBackgroundColor(STARTENDNODEFIGURE0_BACK);
      startEndNodeFigure0.setPreferredSize(new Dimension(getMapMode()
          .DPtoLP(60), getMapMode().DPtoLP(10)));
      startEndNodeFigure0.setMinimumSize(new Dimension(getMapMode().DPtoLP(10),
          getMapMode().DPtoLP(5)));

      this.add(startEndNodeFigure0);

      FlowLayout layoutStartEndNodeFigure0 = new FlowLayout();
      layoutStartEndNodeFigure0.setStretchMinorAxis(false);
      layoutStartEndNodeFigure0.setMinorAlignment(FlowLayout.ALIGN_LEFTTOP);

      layoutStartEndNodeFigure0.setMajorAlignment(FlowLayout.ALIGN_LEFTTOP);
      layoutStartEndNodeFigure0.setMajorSpacing(5);
      layoutStartEndNodeFigure0.setMinorSpacing(5);
      layoutStartEndNodeFigure0.setHorizontal(true);

      startEndNodeFigure0.setLayoutManager(layoutStartEndNodeFigure0);

      fFigureStartEndNodeLabelFigure = new WrappingLabel();
      fFigureStartEndNodeLabelFigure.setText("");

      this.add(fFigureStartEndNodeLabelFigure);

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
    public WrappingLabel getFigureStartEndNodeLabelFigure() {
      return fFigureStartEndNodeLabelFigure;
    }

  }

  /**
   * @generated
   */
  static final Color STARTENDNODEFIGURE0_BACK = new Color(null, 220, 220, 240);

}
