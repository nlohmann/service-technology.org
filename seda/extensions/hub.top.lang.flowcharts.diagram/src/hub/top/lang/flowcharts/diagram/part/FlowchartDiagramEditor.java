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
package hub.top.lang.flowcharts.diagram.part;

import hub.top.lang.flowcharts.diagram.navigator.FlowchartNavigatorItem;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.ui.URIEditorInput;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gmf.runtime.common.ui.services.marker.MarkerNavigationService;
import org.eclipse.gmf.runtime.diagram.core.preferences.PreferencesHint;
import org.eclipse.gmf.runtime.diagram.ui.actions.ActionIds;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDiagramDocument;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDocument;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDocumentProvider;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorMatchingStrategy;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.navigator.resources.ProjectExplorer;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;

/**
 * @generated
 */
public class FlowchartDiagramEditor extends DiagramDocumentEditor implements
    IGotoMarker {

  /**
   * @generated
   */
  public static final String ID = "hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorID"; //$NON-NLS-1$

  /**
   * @generated
   */
  public static final String CONTEXT_ID = "hub.top.lang.flowcharts.diagram.ui.diagramContext"; //$NON-NLS-1$

  /**
   * @generated
   */
  public FlowchartDiagramEditor() {
    super(true);
  }

  /**
   * @generated
   */
  protected String getContextID() {
    return CONTEXT_ID;
  }

  /**
   * @generated
   */
  protected PaletteRoot createPaletteRoot(PaletteRoot existingPaletteRoot) {
    PaletteRoot root = super.createPaletteRoot(existingPaletteRoot);
    new FlowchartPaletteFactory().fillPalette(root);
    return root;
  }

  /**
   * @generated
   */
  protected PreferencesHint getPreferencesHint() {
    return FlowchartDiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT;
  }

  /**
   * @generated
   */
  public String getContributorId() {
    return FlowchartDiagramEditorPlugin.ID;
  }

  /**
   * @generated
   */
  public Object getAdapter(Class type) {
    if (type == IShowInTargetList.class) {
      return new IShowInTargetList() {
        public String[] getShowInTargetIds() {
          return new String[] { ProjectExplorer.VIEW_ID };
        }
      };
    }
    return super.getAdapter(type);
  }

  /**
   * @generated
   */
  protected IDocumentProvider getDocumentProvider(IEditorInput input) {
    if (input instanceof IFileEditorInput || input instanceof URIEditorInput) {
      return FlowchartDiagramEditorPlugin.getInstance().getDocumentProvider();
    }
    return super.getDocumentProvider(input);
  }

  /**
   * @generated
   */
  public TransactionalEditingDomain getEditingDomain() {
    IDocument document = getEditorInput() != null ? getDocumentProvider()
        .getDocument(getEditorInput()) : null;
    if (document instanceof IDiagramDocument) {
      return ((IDiagramDocument) document).getEditingDomain();
    }
    return super.getEditingDomain();
  }

  /**
   * @generated
   */
  protected void setDocumentProvider(IEditorInput input) {
    if (input instanceof IFileEditorInput || input instanceof URIEditorInput) {
      setDocumentProvider(FlowchartDiagramEditorPlugin.getInstance()
          .getDocumentProvider());
    } else {
      super.setDocumentProvider(input);
    }
  }

  /**
   * @generated
   */
  public void gotoMarker(IMarker marker) {
    MarkerNavigationService.getInstance().gotoMarker(this, marker);
  }

  /**
   * @generated
   */
  public boolean isSaveAsAllowed() {
    return true;
  }

  /**
   * @generated
   */
  public void doSaveAs() {
    performSaveAs(new NullProgressMonitor());
  }

  /**
   * @generated
   */
  protected void performSaveAs(IProgressMonitor progressMonitor) {
    Shell shell = getSite().getShell();
    IEditorInput input = getEditorInput();
    SaveAsDialog dialog = new SaveAsDialog(shell);
    IFile original = input instanceof IFileEditorInput ? ((IFileEditorInput) input)
        .getFile()
        : null;
    if (original != null) {
      dialog.setOriginalFile(original);
    }
    dialog.create();
    IDocumentProvider provider = getDocumentProvider();
    if (provider == null) {
      // editor has been programmatically closed while the dialog was open
      return;
    }
    if (provider.isDeleted(input) && original != null) {
      String message = NLS
          .bind(Messages.FlowchartDiagramEditor_SavingDeletedFile, original
              .getName());
      dialog.setErrorMessage(null);
      dialog.setMessage(message, IMessageProvider.WARNING);
    }
    if (dialog.open() == Window.CANCEL) {
      if (progressMonitor != null) {
        progressMonitor.setCanceled(true);
      }
      return;
    }
    IPath filePath = dialog.getResult();
    if (filePath == null) {
      if (progressMonitor != null) {
        progressMonitor.setCanceled(true);
      }
      return;
    }
    IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
    IFile file = workspaceRoot.getFile(filePath);
    final IEditorInput newInput = new FileEditorInput(file);
    // Check if the editor is already open
    IEditorMatchingStrategy matchingStrategy = getEditorDescriptor()
        .getEditorMatchingStrategy();
    IEditorReference[] editorRefs = PlatformUI.getWorkbench()
        .getActiveWorkbenchWindow().getActivePage().getEditorReferences();
    for (int i = 0; i < editorRefs.length; i++) {
      if (matchingStrategy.matches(editorRefs[i], newInput)) {
        MessageDialog.openWarning(shell,
            Messages.FlowchartDiagramEditor_SaveAsErrorTitle,
            Messages.FlowchartDiagramEditor_SaveAsErrorMessage);
        return;
      }
    }
    boolean success = false;
    try {
      provider.aboutToChange(newInput);
      getDocumentProvider(newInput).saveDocument(progressMonitor, newInput,
          getDocumentProvider().getDocument(getEditorInput()), true);
      success = true;
    } catch (CoreException x) {
      IStatus status = x.getStatus();
      if (status == null || status.getSeverity() != IStatus.CANCEL) {
        ErrorDialog.openError(shell,
            Messages.FlowchartDiagramEditor_SaveErrorTitle,
            Messages.FlowchartDiagramEditor_SaveErrorMessage, x.getStatus());
      }
    } finally {
      provider.changed(newInput);
      if (success) {
        setInput(newInput);
      }
    }
    if (progressMonitor != null) {
      progressMonitor.setCanceled(!success);
    }
  }

  /**
   * @generated
   */
  public ShowInContext getShowInContext() {
    return new ShowInContext(getEditorInput(), getNavigatorSelection());
  }

  /**
   * @generated
   */
  private ISelection getNavigatorSelection() {
    IDiagramDocument document = getDiagramDocument();
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
  protected void configureGraphicalViewer() {
    super.configureGraphicalViewer();
    DiagramEditorContextMenuProvider provider = new DiagramEditorContextMenuProvider(
        this, getDiagramGraphicalViewer());
    getDiagramGraphicalViewer().setContextMenu(provider);
    getSite().registerContextMenu(ActionIds.DIAGRAM_EDITOR_CONTEXT_MENU,
        provider, getDiagramGraphicalViewer());
  }

}
