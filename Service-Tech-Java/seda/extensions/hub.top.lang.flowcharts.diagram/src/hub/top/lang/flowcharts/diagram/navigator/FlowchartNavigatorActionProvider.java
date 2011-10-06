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
package hub.top.lang.flowcharts.diagram.navigator;

import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditor;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;
import hub.top.lang.flowcharts.diagram.part.Messages;

import java.util.Iterator;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.common.ui.URIEditorInput;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.navigator.CommonActionProvider;
import org.eclipse.ui.navigator.ICommonActionConstants;
import org.eclipse.ui.navigator.ICommonActionExtensionSite;
import org.eclipse.ui.navigator.ICommonViewerWorkbenchSite;
import org.eclipse.ui.part.FileEditorInput;

/**
 * @generated
 */
public class FlowchartNavigatorActionProvider extends CommonActionProvider {

  /**
   * @generated
   */
  private boolean myContribute;

  /**
   * @generated
   */
  private OpenDiagramAction myOpenDiagramAction;

  /**
   * @generated
   */
  public void init(ICommonActionExtensionSite aSite) {
    super.init(aSite);
    if (aSite.getViewSite() instanceof ICommonViewerWorkbenchSite) {
      myContribute = true;
      makeActions((ICommonViewerWorkbenchSite) aSite.getViewSite());
    } else {
      myContribute = false;
    }
  }

  /**
   * @generated
   */
  private void makeActions(ICommonViewerWorkbenchSite viewerSite) {
    myOpenDiagramAction = new OpenDiagramAction(viewerSite);
  }

  /**
   * @generated
   */
  public void fillActionBars(IActionBars actionBars) {
    if (!myContribute) {
      return;
    }
    IStructuredSelection selection = (IStructuredSelection) getContext()
        .getSelection();
    myOpenDiagramAction.selectionChanged(selection);
    if (myOpenDiagramAction.isEnabled()) {
      actionBars.setGlobalActionHandler(ICommonActionConstants.OPEN,
          myOpenDiagramAction);
    }
  }

  /**
   * @generated
   */
  public void fillContextMenu(IMenuManager menu) {
  }

  /**
   * @generated
   */
  private class OpenDiagramAction extends Action {

    /**
     * @generated
     */
    private Diagram myDiagram;

    /**
     * @generated
     */
    private ICommonViewerWorkbenchSite myViewerSite;

    /**
     * @generated
     */
    public OpenDiagramAction(ICommonViewerWorkbenchSite viewerSite) {
      super(Messages.NavigatorActionProvider_OpenDiagramActionName);
      myViewerSite = viewerSite;
    }

    /**
     * @generated
     */
    public final void selectionChanged(IStructuredSelection selection) {
      myDiagram = null;
      if (selection.size() == 1) {
        Object selectedElement = selection.getFirstElement();
        if (selectedElement instanceof FlowchartNavigatorItem) {
          selectedElement = ((FlowchartNavigatorItem) selectedElement)
              .getView();
        } else if (selectedElement instanceof IAdaptable) {
          selectedElement = ((IAdaptable) selectedElement)
              .getAdapter(View.class);
        }
        if (selectedElement instanceof Diagram) {
          Diagram diagram = (Diagram) selectedElement;
          if (WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
              .getModelID(diagram))) {
            myDiagram = diagram;
          }
        }
      }
      setEnabled(myDiagram != null);
    }

    /**
     * @generated
     */
    public void run() {
      if (myDiagram == null || myDiagram.eResource() == null) {
        return;
      }

      IEditorInput editorInput = getEditorInput();
      IWorkbenchPage page = myViewerSite.getPage();
      try {
        page.openEditor(editorInput, FlowchartDiagramEditor.ID);
      } catch (PartInitException e) {
        FlowchartDiagramEditorPlugin.getInstance().logError(
            "Exception while openning diagram", e); //$NON-NLS-1$
      }
    }

    /**
     * @generated
     */
    private IEditorInput getEditorInput() {
      for (Iterator it = myDiagram.eResource().getContents().iterator(); it
          .hasNext();) {
        EObject nextEObject = (EObject) it.next();
        if (nextEObject == myDiagram) {
          return new FileEditorInput(WorkspaceSynchronizer.getFile(myDiagram
              .eResource()));
        }
        if (nextEObject instanceof Diagram) {
          break;
        }
      }
      URI uri = EcoreUtil.getURI(myDiagram);
      String editorName = uri.lastSegment()
          + "#" + myDiagram.eResource().getContents().indexOf(myDiagram); //$NON-NLS-1$
      IEditorInput editorInput = new URIEditorInput(uri, editorName);
      return editorInput;
    }

  }

}
