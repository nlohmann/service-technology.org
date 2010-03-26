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

import hub.top.lang.flowcharts.diagram.edit.policies.DiagramArcItemSemanticEditPolicy;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.PolylineDecoration;
import org.eclipse.draw2d.RotatableDecoration;
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ConnectionNodeEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ITreeBranchEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.draw2d.ui.figures.PolylineConnectionEx;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class DiagramArcEditPart extends ConnectionNodeEditPart implements
    ITreeBranchEditPart {

  /**
   * @generated
   */
  public static final int VISUAL_ID = 3001;

  /**
   * @generated
   */
  public DiagramArcEditPart(View view) {
    super(view);
  }

  /**
   * @generated
   */
  protected void createDefaultEditPolicies() {
    super.createDefaultEditPolicies();
    installEditPolicy(EditPolicyRoles.SEMANTIC_ROLE,
        new DiagramArcItemSemanticEditPolicy());
  }

  /**
   * @generated
   */
  protected boolean addFixedChild(EditPart childEditPart) {
    if (childEditPart instanceof DiagramArcLabelEditPart) {
      ((DiagramArcLabelEditPart) childEditPart).setLabel(getPrimaryShape()
          .getFigureNodeSuccLabelFigure());
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
   * Creates figure for this edit part.
   * 
   * Body of this method does not depend on settings in generation model
   * so you may safely remove <i>generated</i> tag and modify it.
   * 
   * @generated
   */

  protected Connection createConnectionFigure() {
    return new NodeSuccFigure();
  }

  /**
   * @generated
   */
  public NodeSuccFigure getPrimaryShape() {
    return (NodeSuccFigure) getFigure();
  }

  /**
   * @generated
   */
  public class NodeSuccFigure extends PolylineConnectionEx {

    /**
     * @generated
     */
    private WrappingLabel fFigureNodeSuccLabelFigure;

    /**
     * @generated
     */
    public NodeSuccFigure() {

      createContents();
      setTargetDecoration(createTargetDecoration());
    }

    /**
     * @generated
     */
    private void createContents() {

      fFigureNodeSuccLabelFigure = new WrappingLabel();
      fFigureNodeSuccLabelFigure.setText("");

      this.add(fFigureNodeSuccLabelFigure);

    }

    /**
     * @generated
     */
    private RotatableDecoration createTargetDecoration() {
      PolylineDecoration df = new PolylineDecoration();
      return df;
    }

    /**
     * @generated
     */
    public WrappingLabel getFigureNodeSuccLabelFigure() {
      return fFigureNodeSuccLabelFigure;
    }

  }

}
