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
package hub.top.lang.flowcharts.diagram.edit.policies;

import java.util.Collections;
import java.util.List;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.handles.MoveHandle;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.NonResizableEditPolicyEx;
import org.eclipse.gmf.runtime.diagram.ui.tools.DragEditPartsTrackerEx;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;

/**
 * @generated
 */
public class FlowchartTextNonResizableEditPolicy extends
    NonResizableEditPolicyEx {

  /**
   * @generated
   */
  private IFigure selectionFeedbackFigure;

  /**
   * @generated
   */
  private IFigure focusFeedbackFigure;

  /**
   * @generated
   */
  protected void showPrimarySelection() {
    if (getHostFigure() instanceof WrappingLabel) {
      ((WrappingLabel) getHostFigure()).setSelected(true);
      ((WrappingLabel) getHostFigure()).setFocus(true);
    } else {
      showSelection();
      showFocus();
    }
  }

  /**
   * @generated
   */
  protected void showSelection() {
    if (getHostFigure() instanceof WrappingLabel) {
      ((WrappingLabel) getHostFigure()).setSelected(true);
      ((WrappingLabel) getHostFigure()).setFocus(false);
    } else {
      hideSelection();
      addFeedback(selectionFeedbackFigure = createSelectionFeedbackFigure());
      refreshSelectionFeedback();
      hideFocus();
    }
  }

  /**
   * @generated
   */
  protected void hideSelection() {
    if (getHostFigure() instanceof WrappingLabel) {
      ((WrappingLabel) getHostFigure()).setSelected(false);
      ((WrappingLabel) getHostFigure()).setFocus(false);
    } else {
      if (selectionFeedbackFigure != null) {
        removeFeedback(selectionFeedbackFigure);
        selectionFeedbackFigure = null;
      }
      hideFocus();
    }
  }

  /**
   * @generated
   */
  protected void showFocus() {
    if (getHostFigure() instanceof WrappingLabel) {
      ((WrappingLabel) getHostFigure()).setFocus(true);
    } else {
      hideFocus();
      addFeedback(focusFeedbackFigure = createFocusFeedbackFigure());
      refreshFocusFeedback();
    }
  }

  /**
   * @generated
   */
  protected void hideFocus() {
    if (getHostFigure() instanceof WrappingLabel) {
      ((WrappingLabel) getHostFigure()).setFocus(false);
    } else {
      if (focusFeedbackFigure != null) {
        removeFeedback(focusFeedbackFigure);
        focusFeedbackFigure = null;
      }
    }
  }

  /**
   * @generated
   */
  protected Rectangle getFeedbackBounds() {
    Rectangle bounds;
    if (getHostFigure() instanceof Label) {
      bounds = ((Label) getHostFigure()).getTextBounds();
      bounds.intersect(getHostFigure().getBounds());
    } else {
      bounds = getHostFigure().getBounds().getCopy();
    }
    getHostFigure().getParent().translateToAbsolute(bounds);
    getFeedbackLayer().translateToRelative(bounds);
    return bounds;
  }

  /**
   * @generated
   */
  protected IFigure createSelectionFeedbackFigure() {
    if (getHostFigure() instanceof Label) {
      Label feedbackFigure = new Label();
      feedbackFigure.setOpaque(true);
      feedbackFigure.setBackgroundColor(ColorConstants.menuBackgroundSelected);
      feedbackFigure.setForegroundColor(ColorConstants.menuForegroundSelected);
      return feedbackFigure;
    } else {
      RectangleFigure feedbackFigure = new RectangleFigure();
      feedbackFigure.setFill(false);
      return feedbackFigure;
    }
  }

  /**
   * @generated
   */
  protected IFigure createFocusFeedbackFigure() {
    return new Figure() {

      protected void paintFigure(Graphics graphics) {
        graphics.drawFocus(getBounds().getResized(-1, -1));
      }
    };
  }

  /**
   * @generated
   */
  protected void updateLabel(Label target) {
    Label source = (Label) getHostFigure();
    target.setText(source.getText());
    target.setTextAlignment(source.getTextAlignment());
    target.setFont(source.getFont());
  }

  /**
   * @generated
   */
  protected void refreshSelectionFeedback() {
    if (selectionFeedbackFigure != null) {
      if (selectionFeedbackFigure instanceof Label) {
        updateLabel((Label) selectionFeedbackFigure);
        selectionFeedbackFigure.setBounds(getFeedbackBounds());
      } else {
        selectionFeedbackFigure.setBounds(getFeedbackBounds().expand(5, 5));
      }
    }
  }

  /**
   * @generated
   */
  protected void refreshFocusFeedback() {
    if (focusFeedbackFigure != null) {
      focusFeedbackFigure.setBounds(getFeedbackBounds());
    }
  }

  /**
   * @generated
   */
  public void refreshFeedback() {
    refreshSelectionFeedback();
    refreshFocusFeedback();
  }

  /**
   * @generated
   */
  protected List createSelectionHandles() {
    MoveHandle moveHandle = new MoveHandle((GraphicalEditPart) getHost());
    moveHandle.setBorder(null);
    moveHandle.setDragTracker(new DragEditPartsTrackerEx(getHost()));
    return Collections.singletonList(moveHandle);
  }
}
