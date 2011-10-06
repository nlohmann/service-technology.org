/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
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
 * The Original Code is this file as it was released on March 16, 2009.
 * The Initial Developer of the Original Code is
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
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
\*****************************************************************************/

package hub.top.editor.ui.actions;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;

/**
 * A helper action that establishes backwards compatibility with old
 * versions of this package. The action allows to refactor a .ptnetlola
 * model file and corresponding diagram into a .ptnet model file
 * and corresponding diagram. Thereby, all cross-references from the
 * diagram to the model file are updated as well.
 * 
 * @author Dirk Fahland
 */
public class PtNetLoLARename implements IObjectActionDelegate {

  private IWorkbenchPart targetPart;
  private URI domainModelURI;
  
  private IFile selectedFile;

  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }

  /**
   * track selection changes in the workbench
   * @param action
   * @param selection
   */
  public void selectionChanged(IAction action, ISelection selection) {
    domainModelURI = null;
    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    // store selection and URI, re-enable action
    selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
    domainModelURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
        .toString(), true);
    action.setEnabled(true);
  }
  
  /**
   * Convert an input stream into a string
   * @param in
   * @return contents of the input stream as string
   * @throws IOException
   */
  private String inputStreamToString(InputStream in) throws IOException {
    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(in));
    StringBuilder stringBuilder = new StringBuilder();
    String line = null;

    while ((line = bufferedReader.readLine()) != null) {
      stringBuilder.append(line + "\n");
    }

    bufferedReader.close();
    return stringBuilder.toString();
  }

  /**
   * Execute the action, rename the selected file (by copying and deleting the old file),
   * and update name and contents of the corresponding diagram file.
   * 
   * @param action
   */
  public void run(IAction action) {
    // get the shell to pop up message boxes
    Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
    
    // copy old model file to new file with new extension
    IPath targetPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("ptnet");
    IFile targetFile = ResourcesPlugin.getWorkspace().getRoot().getFile(targetPath);
    try {
      targetFile.create(selectedFile.getContents(), true, new NullProgressMonitor());
    } catch (CoreException e) {
      
      MessageDialog.openError(shell,
          "Could not create target file "+targetFile.getName(), e.getMessage());
    }

    // get diagram file
    IPath diagramPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("ptnetlola_diagram");
    IFile diagramFile = ResourcesPlugin.getWorkspace().getRoot().getFile(diagramPath);
    if (diagramFile != null) {
      
      // copy diagram file to file with new extension
      IPath diagramTargetPath = selectedFile.getFullPath().removeFileExtension().addFileExtension("ptnet_diagram");
      IFile diagramTargetFile = ResourcesPlugin.getWorkspace().getRoot().getFile(diagramTargetPath);
      try {
        // when copying, update all cross-references to the old file in the diagram file
        String diagram = inputStreamToString(diagramFile.getContents());
        String diagram2 = diagram.replaceAll(".ptnetlola", ".ptnet");
        ByteArrayInputStream dStream = new ByteArrayInputStream(diagram2.getBytes());
        // write updated contents
        diagramTargetFile.create(dStream, true, new NullProgressMonitor());
      } catch (CoreException e) {
        MessageDialog.openError(shell,
            "Could not create target file "+diagramTargetFile.getName(), e.getMessage());
      } catch (IOException e) {
        MessageDialog.openError(shell,
            "Could not create target file "+diagramTargetFile.getName(), e.getMessage());
      }
    }
    
    // now delete old files
    try {
      selectedFile.delete(true, true, new NullProgressMonitor());
      if (diagramFile != null)
        diagramFile.delete(true, true, new NullProgressMonitor());
    } catch (CoreException e) {
      MessageDialog.openError(shell,
          "Could not delete old files, please check your workspace.", e.getMessage());
    }
  }
}
