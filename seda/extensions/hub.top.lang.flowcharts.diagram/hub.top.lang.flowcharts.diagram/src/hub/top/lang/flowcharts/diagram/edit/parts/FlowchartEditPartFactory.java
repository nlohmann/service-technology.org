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

import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.gmf.runtime.diagram.ui.editparts.ITextAwareEditPart;
import org.eclipse.gmf.runtime.draw2d.ui.figures.WrappingLabel;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;

/**
 * @generated
 */
public class FlowchartEditPartFactory implements EditPartFactory {

  /**
   * @generated
   */
  public EditPart createEditPart(EditPart context, Object model) {
    if (model instanceof View) {
      View view = (View) model;
      switch (FlowchartVisualIDRegistry.getVisualID(view)) {

      case WorkflowDiagramEditPart.VISUAL_ID:
        return new WorkflowDiagramEditPart(view);

      case StartNodeEditPart.VISUAL_ID:
        return new StartNodeEditPart(view);

      case StartNodeLabelEditPart.VISUAL_ID:
        return new StartNodeLabelEditPart(view);

      case EndnodeEditPart.VISUAL_ID:
        return new EndnodeEditPart(view);

      case EndnodeLabelEditPart.VISUAL_ID:
        return new EndnodeLabelEditPart(view);

      case SimpleActivityEditPart.VISUAL_ID:
        return new SimpleActivityEditPart(view);

      case SimpleActivityLabelEditPart.VISUAL_ID:
        return new SimpleActivityLabelEditPart(view);

      case SubprocessEditPart.VISUAL_ID:
        return new SubprocessEditPart(view);

      case SubprocessLabelEditPart.VISUAL_ID:
        return new SubprocessLabelEditPart(view);

      case EventEditPart.VISUAL_ID:
        return new EventEditPart(view);

      case SplitNodeEditPart.VISUAL_ID:
        return new SplitNodeEditPart(view);

      case SplitNodeLabelEditPart.VISUAL_ID:
        return new SplitNodeLabelEditPart(view);

      case MergeNodeEditPart.VISUAL_ID:
        return new MergeNodeEditPart(view);

      case FlowEditPart.VISUAL_ID:
        return new FlowEditPart(view);

      case TransientResourceEditPart.VISUAL_ID:
        return new TransientResourceEditPart(view);

      case TransientResourceLabelEditPart.VISUAL_ID:
        return new TransientResourceLabelEditPart(view);

      case DocumentEditPart.VISUAL_ID:
        return new DocumentEditPart(view);

      case DocumentLabelEditPart.VISUAL_ID:
        return new DocumentLabelEditPart(view);

      case PersistentResourceEditPart.VISUAL_ID:
        return new PersistentResourceEditPart(view);

      case PersistentResourceLabelEditPart.VISUAL_ID:
        return new PersistentResourceLabelEditPart(view);

      case StartNode2EditPart.VISUAL_ID:
        return new StartNode2EditPart(view);

      case StartNodeLabel2EditPart.VISUAL_ID:
        return new StartNodeLabel2EditPart(view);

      case Endnode2EditPart.VISUAL_ID:
        return new Endnode2EditPart(view);

      case EndnodeLabel2EditPart.VISUAL_ID:
        return new EndnodeLabel2EditPart(view);

      case SimpleActivity2EditPart.VISUAL_ID:
        return new SimpleActivity2EditPart(view);

      case SimpleActivityLabel2EditPart.VISUAL_ID:
        return new SimpleActivityLabel2EditPart(view);

      case Subprocess2EditPart.VISUAL_ID:
        return new Subprocess2EditPart(view);

      case SubprocessLabel2EditPart.VISUAL_ID:
        return new SubprocessLabel2EditPart(view);

      case Event2EditPart.VISUAL_ID:
        return new Event2EditPart(view);

      case SplitNode2EditPart.VISUAL_ID:
        return new SplitNode2EditPart(view);

      case SplitNodeLabel2EditPart.VISUAL_ID:
        return new SplitNodeLabel2EditPart(view);

      case MergeNode2EditPart.VISUAL_ID:
        return new MergeNode2EditPart(view);

      case Flow2EditPart.VISUAL_ID:
        return new Flow2EditPart(view);

      case TransientResource2EditPart.VISUAL_ID:
        return new TransientResource2EditPart(view);

      case TransientResourceLabel2EditPart.VISUAL_ID:
        return new TransientResourceLabel2EditPart(view);

      case Document2EditPart.VISUAL_ID:
        return new Document2EditPart(view);

      case DocumentLabel2EditPart.VISUAL_ID:
        return new DocumentLabel2EditPart(view);

      case PersistentResource2EditPart.VISUAL_ID:
        return new PersistentResource2EditPart(view);

      case PersistentResourceLabel2EditPart.VISUAL_ID:
        return new PersistentResourceLabel2EditPart(view);

      case FlowFlowCompartmentEditPart.VISUAL_ID:
        return new FlowFlowCompartmentEditPart(view);

      case FlowFlowCompartment2EditPart.VISUAL_ID:
        return new FlowFlowCompartment2EditPart(view);

      case DiagramArcEditPart.VISUAL_ID:
        return new DiagramArcEditPart(view);

      case DiagramArcLabelEditPart.VISUAL_ID:
        return new DiagramArcLabelEditPart(view);
      }
    }
    return createUnrecognizedEditPart(context, model);
  }

  /**
   * @generated
   */
  private EditPart createUnrecognizedEditPart(EditPart context, Object model) {
    // Handle creation of unrecognized child node EditParts here
    return null;
  }

  /**
   * @generated
   */
  public static CellEditorLocator getTextCellEditorLocator(
      ITextAwareEditPart source) {
    if (source.getFigure() instanceof WrappingLabel)
      return new TextCellEditorLocator((WrappingLabel) source.getFigure());
    else {
      return new LabelCellEditorLocator((Label) source.getFigure());
    }
  }

  /**
   * @generated
   */
  static private class TextCellEditorLocator implements CellEditorLocator {

    /**
     * @generated
     */
    private WrappingLabel wrapLabel;

    /**
     * @generated
     */
    public TextCellEditorLocator(WrappingLabel wrapLabel) {
      this.wrapLabel = wrapLabel;
    }

    /**
     * @generated
     */
    public WrappingLabel getWrapLabel() {
      return wrapLabel;
    }

    /**
     * @generated
     */
    public void relocate(CellEditor celleditor) {
      Text text = (Text) celleditor.getControl();
      Rectangle rect = getWrapLabel().getTextBounds().getCopy();
      getWrapLabel().translateToAbsolute(rect);
      if (getWrapLabel().isTextWrapOn()
          && getWrapLabel().getText().length() > 0) {
        rect.setSize(new Dimension(text.computeSize(rect.width, SWT.DEFAULT)));
      } else {
        int avr = FigureUtilities.getFontMetrics(text.getFont())
            .getAverageCharWidth();
        rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT, SWT.DEFAULT))
            .expand(avr * 2, 0));
      }
      if (!rect.equals(new Rectangle(text.getBounds()))) {
        text.setBounds(rect.x, rect.y, rect.width, rect.height);
      }
    }
  }

  /**
   * @generated
   */
  private static class LabelCellEditorLocator implements CellEditorLocator {

    /**
     * @generated
     */
    private Label label;

    /**
     * @generated
     */
    public LabelCellEditorLocator(Label label) {
      this.label = label;
    }

    /**
     * @generated
     */
    public Label getLabel() {
      return label;
    }

    /**
     * @generated
     */
    public void relocate(CellEditor celleditor) {
      Text text = (Text) celleditor.getControl();
      Rectangle rect = getLabel().getTextBounds().getCopy();
      getLabel().translateToAbsolute(rect);
      int avr = FigureUtilities.getFontMetrics(text.getFont())
          .getAverageCharWidth();
      rect.setSize(new Dimension(text.computeSize(SWT.DEFAULT, SWT.DEFAULT))
          .expand(avr * 2, 0));
      if (!rect.equals(new Rectangle(text.getBounds()))) {
        text.setBounds(rect.x, rect.y, rect.width, rect.height);
      }
    }
  }
}
