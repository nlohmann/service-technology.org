/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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

package hub.top.editor.eclipse;

import hub.top.editor.resource.Activator;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.emf.ecore.xmi.XMLResource;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

public class ActionHelper {
  /**
   * Write <code>contents</code> to a resource of the Eclipse framework
   * at <code>uri</code>.
   * 
   * @param win
   * @param uri
   * @param contents
   */
  public static void writeEcoreResourceToFile (IWorkbenchWindow win, final URI uri, final EObject contents) {
    WorkspaceModifyOperation op = new WorkspaceModifyOperation () {
      
      @Override
      protected void execute(IProgressMonitor monitor) throws CoreException,
          InvocationTargetException, InterruptedException {
        try {
          ResourceSet resources = new ResourceSetImpl();
          Resource resource = resources.createResource(uri);
          resource.getContents().add(contents);

          // Save the contents of the resource to the file system.
          resource.save(ActionHelper.getSaveOptions());
        } catch (Exception exception) {
          ResourcesPlugin.getPlugin().getLog().log(
            new Status(Status.ERROR, Activator.PLUGIN_ID, "Unable to write resource to "+uri+". "+exception.getCause(), exception));
        }
      }
    };
    try {
      win.run(true, false, op);
    } catch (InvocationTargetException e) {
      ResourcesPlugin.getPlugin().getLog().log(
          new Status(Status.ERROR, Activator.PLUGIN_ID, "Unable to store model in "+uri+". Could not invoke workspace operation: "+e.getCause(), e));
    } catch (InterruptedException ex) {
      ResourcesPlugin.getPlugin().getLog().log(
          new Status(Status.ERROR, Activator.PLUGIN_ID, "Storing resource "+uri+" has been interrupted by "+ex.getCause(), ex));
    }
  }
  
  /**
   * @return standard UTF-8 encoding save options
   */
  public static Map<String, String> getSaveOptions() {
    // Save the contents of the resource to the file system.
    Map<String, String> options = new HashMap<String, String>();
    options.put(XMLResource.OPTION_ENCODING, "UTF-8");
    return options;
  }
  
  /**
   * Write <code>contents</code> into the file at the <code>targetPath</code>.
   * 
   * @param targetPath
   * @param contents
   */
  public static void writeFile (IPath targetPath, InputStream contents) {
    IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
    IFile targetFile = root.getFile(targetPath);
    try {
      if (targetFile.exists())
        targetFile.setContents(contents, true, true, null);
      else
        targetFile.create(contents, true, null);
      
    } catch (CoreException e) {
      ResourcesPlugin.getPlugin().getLog().log(
        new Status(Status.ERROR, Activator.PLUGIN_ID, "Could not save file "+targetPath+".", e));
    }
  }
  
  /**
   * Write <code>contents</code> into the file at the <code>targetPath</code>.
   * 
   * @param targetPath
   * @param contents
   */
  public static void writeFile (IPath targetPath, String contents) {
    ByteArrayInputStream cStream = new ByteArrayInputStream(contents.getBytes());
    writeFile(targetPath, cStream);
  }
  
  /**
   * @param selected
   * @param extensions
   * @return all files in the selection which end with one of the given extensions
   */
  public static LinkedList<IFile> filterFiles(Object[] selected, String[] extensions) {
    LinkedList<IFile> filtered = new LinkedList<IFile>();
    for (int i=0;i<selected.length; i++) {
      if (selected[i] instanceof IFile == false) continue;
      
      for (int j=0;j<extensions.length; j++) {
        if ( ((IFile)selected[i]).getFileExtension().equals(extensions[j]) )
          filtered.add((IFile)selected[i]);
      }
    }
    return filtered;
  }
  

}
