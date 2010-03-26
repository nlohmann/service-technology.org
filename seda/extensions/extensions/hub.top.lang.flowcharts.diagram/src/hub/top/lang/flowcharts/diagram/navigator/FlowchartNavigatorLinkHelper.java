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

import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;

import java.util.Iterator;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.ui.URIEditorInput;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gmf.runtime.diagram.ui.parts.DiagramEditor;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDiagramDocument;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.navigator.ILinkHelper;
import org.eclipse.ui.part.FileEditorInput;

/**
 * @generated
 */
public class FlowchartNavigatorLinkHelper implements ILinkHelper {

  /**
   * @generated
   */
  private static IEditorInput getEditorInput(Diagram diagram) {
    Resource diagramResource = diagram.eResource();
    for (Iterator it = diagramResource.getContents().iterator(); it.hasNext();) {
      EObject nextEObject = (EObject) it.next();
      if (nextEObject == diagram) {
        return new FileEditorInput(WorkspaceSynchronizer
            .getFile(diagramResource));
      }
      if (nextEObject instanceof Diagram) {
        break;
      }
    }
    URI uri = EcoreUtil.getURI(diagram);
    String editorName = uri.lastSegment()
        + "#" + diagram.eResource().getContents().indexOf(diagram); //$NON-NLS-1$
    IEditorInput editorInput = new URIEditorInput(uri, editorName);
    return editorInput;
  }

  /**
   * @generated
   */
  public IStructuredSelection findSelection(IEditorInput anInput) {
    IDiagramDocument document = FlowchartDiagramEditorPlugin.getInstance()
        .getDocumentProvider().getDiagramDocument(anInput);
    if (document == null) {
      return StructuredSelection.EMPTY;
    }
    Diagram diagram = document.getDiagram();
    IFile file = WorkspaceSynchronizer.getFile(diagram.eResource());
    if (file != null) {
      FlowchartNavigatorItem item = new FlowchartNavigatorItem(diagram, file,
          false);
      return new StructuredSelection(item);
    }
    return StructuredSelection.EMPTY;
  }

  /**
   * @generated
   */
  public void activateEditor(IWorkbenchPage aPage,
      IStructuredSelection aSelection) {
    if (aSelection == null || aSelection.isEmpty()) {
      return;
    }
    if (false == aSelection.getFirstElement() instanceof FlowchartAbstractNavigatorItem) {
      return;
    }

    FlowchartAbstractNavigatorItem abstractNavigatorItem = (FlowchartAbstractNavigatorItem) aSelection
        .getFirstElement();
    View navigatorView = null;
    if (abstractNavigatorItem instanceof FlowchartNavigatorItem) {
      navigatorView = ((FlowchartNavigatorItem) abstractNavigatorItem)
          .getView();
    } else if (abstractNavigatorItem instanceof FlowchartNavigatorGroup) {
      FlowchartNavigatorGroup navigatorGroup = (FlowchartNavigatorGroup) abstractNavigatorItem;
      if (navigatorGroup.getParent() instanceof FlowchartNavigatorItem) {
        navigatorView = ((FlowchartNavigatorItem) navigatorGroup.getParent())
            .getView();
      }
    }
    if (navigatorView == null) {
      return;
    }
    IEditorInput editorInput = getEditorInput(navigatorView.getDiagram());
    IEditorPart editor = aPage.findEditor(editorInput);
    if (editor == null) {
      return;
    }
    aPage.bringToTop(editor);
    if (editor instanceof DiagramEditor) {
      DiagramEditor diagramEditor = (DiagramEditor) editor;
      ResourceSet diagramEditorResourceSet = diagramEditor.getEditingDomain()
          .getResourceSet();
      EObject selectedView = diagramEditorResourceSet.getEObject(EcoreUtil
          .getURI(navigatorView), true);
      if (selectedView == null) {
        return;
      }
      GraphicalViewer graphicalViewer = (GraphicalViewer) diagramEditor
          .getAdapter(GraphicalViewer.class);
      EditPart selectedEditPart = (EditPart) graphicalViewer
          .getEditPartRegistry().get(selectedView);
      if (selectedEditPart != null) {
        graphicalViewer.select(selectedEditPart);
      }
    }
  }

}
