package hub.top.editor.ptnetLoLA.diagram.part;

import org.eclipse.gmf.runtime.diagram.ui.parts.DiagramActionBarContributor;
import org.eclipse.gmf.runtime.diagram.ui.printing.render.actions.EnhancedPrintActionHelper;
import org.eclipse.gmf.runtime.diagram.ui.printing.render.actions.RenderedPrintPreviewAction;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;

/**
 * @generated
 */
public class PtnetLoLADiagramActionBarContributor extends
    DiagramActionBarContributor {

  /**
   * @generated
   */
  protected Class getEditorClass() {
    return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditor.class;
  }

  /**
   * @generated
   */
  protected String getEditorId() {
    return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditor.ID;
  }

  /**
   * @generated
   */
  public void init(IActionBars bars, IWorkbenchPage page) {
    super.init(bars, page);
    // print preview
    IMenuManager fileMenu = bars.getMenuManager().findMenuUsingPath(
        IWorkbenchActionConstants.M_FILE);
    assert fileMenu != null;
    IAction printPreviewAction = new RenderedPrintPreviewAction(
        new EnhancedPrintActionHelper());
    fileMenu.insertBefore("print", printPreviewAction); //$NON-NLS-1$
  }
}
