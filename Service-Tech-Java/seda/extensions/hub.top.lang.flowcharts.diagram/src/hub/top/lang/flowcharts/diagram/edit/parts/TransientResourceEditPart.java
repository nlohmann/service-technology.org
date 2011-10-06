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

import hub.top.lang.flowcharts.diagram.edit.policies.TransientResourceItemSemanticEditPolicy;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
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
public class TransientResourceEditPart extends ShapeNodeEditPart {

  /**
   * @generated
   */
  public static final int VISUAL_ID = 2009;

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
  public TransientResourceEditPart(View view) {
    super(view);
  }

  /**
   * @generated
   */
  protected void createDefaultEditPolicies() {
    super.createDefaultEditPolicies();
    installEditPolicy(EditPolicyRoles.SEMANTIC_ROLE,
        new TransientResourceItemSemanticEditPolicy());
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
    ResourceNodeFigure figure = new ResourceNodeFigure();
    return primaryShape = figure;
  }

  /**
   * @generated
   */
  public ResourceNodeFigure getPrimaryShape() {
    return (ResourceNodeFigure) primaryShape;
  }

  /**
   * @generated
   */
  protected boolean addFixedChild(EditPart childEditPart) {
    if (childEditPart instanceof TransientResourceLabelEditPart) {
      ((TransientResourceLabelEditPart) childEditPart)
          .setLabel(getPrimaryShape().getFigureResourceNodeLabel());
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
        .getType(TransientResourceLabelEditPart.VISUAL_ID));
  }

  /**
   * @generated
   */
  public class ResourceNodeFigure extends RectangleFigure {

    /**
     * @generated
     */
    private WrappingLabel fFigureResourceNodeLabel;

    /**
     * @generated
     */
    public ResourceNodeFigure() {
      this.setLayoutManager(new StackLayout());
      this.setFill(false);
      this.setOutline(false);
      createContents();
    }

    /**
     * @generated
     */
    private void createContents() {

      class ResourceNodeShape0Class extends Shape {
        /**
         * @generated
         */
        private final PointList myTemplate = new PointList();
        /**
         * @generated
         */
        private Rectangle myTemplateBounds;

        /**
         * @generated
         */
        public void addPoint(Point point) {
          myTemplate.addPoint(point);
          myTemplateBounds = null;
        }

        /**
         * @generated
         */
        protected void fillShape(Graphics graphics) {
          Rectangle bounds = getBounds();
          graphics.pushState();
          graphics.translate(bounds.x, bounds.y);
          graphics.fillPolygon(scalePointList());
          graphics.popState();
        }

        /**
         * @generated
         */
        protected void outlineShape(Graphics graphics) {
          Rectangle bounds = getBounds();
          graphics.pushState();
          graphics.translate(bounds.x, bounds.y);
          graphics.drawPolygon(scalePointList());
          graphics.popState();
        }

        /**
         * @generated
         */
        private Rectangle getTemplateBounds() {
          if (myTemplateBounds == null) {
            myTemplateBounds = myTemplate.getBounds().getCopy().union(0, 0);
            //just safety -- we are going to use this as divider 
            if (myTemplateBounds.width < 1) {
              myTemplateBounds.width = 1;
            }
            if (myTemplateBounds.height < 1) {
              myTemplateBounds.height = 1;
            }
          }
          return myTemplateBounds;
        }

        /**
         * @generated
         */
        private int[] scalePointList() {
          Rectangle pointsBounds = getTemplateBounds();
          Rectangle actualBounds = getBounds();

          float xScale = ((float) actualBounds.width) / pointsBounds.width;
          float yScale = ((float) actualBounds.height) / pointsBounds.height;

          if (xScale == 1 && yScale == 1) {
            return myTemplate.toIntArray();
          }
          int[] scaled = (int[]) myTemplate.toIntArray().clone();
          for (int i = 0; i < scaled.length; i += 2) {
            scaled[i] = (int) Math.floor(scaled[i] * xScale);
            scaled[i + 1] = (int) Math.floor(scaled[i + 1] * yScale);
          }
          return scaled;
        }
      }
      ;
      ResourceNodeShape0Class resourceNodeShape0 = new ResourceNodeShape0Class();

      resourceNodeShape0.addPoint(new Point(getMapMode().DPtoLP(10),
          getMapMode().DPtoLP(0)));
      resourceNodeShape0.addPoint(new Point(getMapMode().DPtoLP(50),
          getMapMode().DPtoLP(0)));
      resourceNodeShape0.addPoint(new Point(getMapMode().DPtoLP(40),
          getMapMode().DPtoLP(30)));
      resourceNodeShape0.addPoint(new Point(getMapMode().DPtoLP(0),
          getMapMode().DPtoLP(30)));
      resourceNodeShape0.setFill(true);
      resourceNodeShape0.setBackgroundColor(RESOURCENODESHAPE0_BACK);
      resourceNodeShape0.setPreferredSize(new Dimension(
          getMapMode().DPtoLP(50), getMapMode().DPtoLP(30)));

      this.add(resourceNodeShape0);
      resourceNodeShape0.setLayoutManager(new XYLayout());

      fFigureResourceNodeLabel = new WrappingLabel();
      fFigureResourceNodeLabel.setText("");

      this.add(fFigureResourceNodeLabel);

    }

    /**
     * @generated
     */
    private boolean myUseLocalCoordinates = true;

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
    public WrappingLabel getFigureResourceNodeLabel() {
      return fFigureResourceNodeLabel;
    }

  }

  /**
   * @generated
   */
  static final Color RESOURCENODESHAPE0_BACK = new Color(null, 255, 204, 153);

}
