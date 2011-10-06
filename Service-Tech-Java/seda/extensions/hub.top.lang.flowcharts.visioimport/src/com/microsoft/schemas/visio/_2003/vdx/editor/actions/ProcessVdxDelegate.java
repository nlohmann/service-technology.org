/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package com.microsoft.schemas.visio._2003.vdx.editor.actions;

import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorUtil;
import hub.top.lang.flowcharts.diagram.part.Messages;
import hub.top.lang.flowcharts.vdximport.Activator;
import hub.top.lang.flowcharts.vdximport.VdxtoFC;

import java.io.IOException;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.diagram.core.services.ViewService;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorActionDelegate;
import org.eclipse.ui.IEditorPart;

import com.microsoft.schemas.visio._2003.vdx.DocumentRoot;
import com.microsoft.schemas.visio._2003.vdx.presentation.vdxEditor;

public class ProcessVdxDelegate implements IEditorActionDelegate {

  private vdxEditor _targetEditor; ///< current editor windows

  public void setActiveEditor(IAction action, IEditorPart targetEditor) {
    // remember current editor windows
    _targetEditor = (vdxEditor) targetEditor;
  }

  /**
   * This method should be called within a workspace modify operation since it
   * creates resources.
   */
  public static AbstractTransactionalCommand createDiagram(URI diagramURI, URI modelURI,
      final WorkflowDiagram wfSheet, final VdxtoFC vdxtofc, TransactionalEditingDomain editingDomain,
      IProgressMonitor progressMonitor) {

    progressMonitor.beginTask(
        Messages.FlowchartDiagramEditorUtil_CreateDiagramProgressTask, 3);
    final Resource diagramResource = editingDomain.getResourceSet()
        .createResource(diagramURI);
    // final Resource modelResource = editingDomain.getResourceSet()
    // .createResource(modelURI);
    final String diagramName = diagramURI.lastSegment();

    AbstractTransactionalCommand command = new AbstractTransactionalCommand(
        editingDomain,
        Messages.FlowchartDiagramEditorUtil_CreateDiagramCommandLabel,
        null) {
      protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
          IAdaptable info) throws ExecutionException {
        // attachModelToResource(wfSheet, modelResource);

        Diagram diagram = ViewService.createDiagram(wfSheet,
            WorkflowDiagramEditPart.MODEL_ID,
            FlowchartDiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT);
        if (diagram != null) {
          diagramResource.getContents().add(diagram);
          diagram.setName(diagramName);
          diagram.setElement(wfSheet);
        }

        try {
          // modelResource.save(hub.top.lang.flowcharts.diagram.part.FlowChartDiagramEditorUtil
          // .getSaveOptions());
          diagramResource
              .save(hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorUtil
                  .getSaveOptions());
          FlowchartDiagramEditorUtil.setCharset(WorkspaceSynchronizer
              .getFile(diagramResource));
        } catch (IOException e) {

          FlowchartDiagramEditorPlugin.getInstance().logError(
              "Unable to store model and diagram resources", e); //$NON-NLS-1$
        }
        return CommandResult.newOKCommandResult();
      }
    };


    // FlowChartDiagramEditorUtil.setCharset(WorkspaceSynchronizer.getFile(modelResource));
    //return new ICommandProxy(command);
    return command;
  }
  
  /**
   * Translate the given Visio {@link DocumentRoot} to Flowchart
   * {@link Workflow} object and generate diagram files for all
   * subprocesses in a separate folder.
   * 
   * @param root
   */
  public static void convertVisioToFlow(DocumentRoot root) {
    final VdxtoFC vdxtofc = new VdxtoFC(root);

    final PluginHelper pluginHelper =  Activator.getPluginHelper();
    final URI uri = root.eResource().getURI().trimFileExtension()
        .appendFileExtension("flow");
    final Workflow wf = vdxtofc.translateToFC();

    if (wf == null)
      return;

    WorkspaceJob job = new WorkspaceJob("Saving flowchart model") {
      public IStatus runInWorkspace(IProgressMonitor monitor) 
            throws CoreException {
        try {
          ResourceSet resources = new ResourceSetImpl();
          Resource resource = resources.createResource(uri);
          resource.getContents().add(wf);

          // Save the contents of the resource to the file system.
          resource.save(ResourceHelper.getSaveOptions());
        } catch (Exception exception) {
          pluginHelper.logError(exception);
        }
        
        try {
          TransactionalEditingDomain editingDomain = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
          //org.eclipse.gef.commands.CompoundCommand cc = new org.eclipse.gef.commands.CompoundCommand("Create flowchart diagrams");
          for (WorkflowDiagram wfProcess : wf.getProcesses()) {
  
            URI processDiagramURI = uri.trimFileExtension().appendSegment(
                  wfProcess.getName()).appendFileExtension("flow_diagram");
            //cc.add(createDiagram(processDiagramURI, uri, wfProcess, vdxtofc, editingDomain, monitor));
            AbstractTransactionalCommand command = createDiagram(processDiagramURI, uri, wfProcess, vdxtofc, editingDomain, monitor);
            try {
              OperationHistoryFactory.getOperationHistory().execute(command,
                  new SubProgressMonitor(monitor, 1), null);
            } catch (ExecutionException e) {
              FlowchartDiagramEditorPlugin.getInstance().logError(
                  "Unable to create model and diagram", e); //$NON-NLS-1$
            }

          }
          

        } catch (Exception exception) {
          pluginHelper.logError(exception);
        }
        return Status.OK_STATUS;
      }
    };
    job.schedule();
  }


  public void run(IAction action) {

    EList<Resource> res = _targetEditor.getEditingDomain().getResourceSet()
        .getResources();

    for (Resource r : res) {
      DocumentRoot root = (DocumentRoot) r.getContents().get(0);
      convertVisioToFlow(root);
    }
  }

  public void selectionChanged(IAction action, ISelection selection) {
    // TODO Auto-generated method stub
  }

}
