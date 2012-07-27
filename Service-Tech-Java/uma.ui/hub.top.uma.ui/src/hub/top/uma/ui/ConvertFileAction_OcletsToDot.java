/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Uma UI
 *                       (Unfolding-Based Model Analyzer) 
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

package hub.top.uma.ui;

import hub.top.editor.eclipse.ActionHelper;
import hub.top.petrinet.PetriNet;
import hub.top.scenario.OcletIO;
import hub.top.scenario.OcletIO_Out;
import hub.top.scenario.OcletSpecification;

import java.io.IOException;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class ConvertFileAction_OcletsToDot implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.oclets.ConvertToDot";
	
	private IWorkbenchWindow workbenchWindow;
	
	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile_system = null;
	
	public void dispose() {
		// TODO Auto-generated method stub

	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
		
		if (selectedFile_system == null)
		  return;
		
		String fileName = selectedFile_system.getRawLocation().toOSString();
		
		try {
      String targetPathStr = selectedFile_system.getFullPath().removeFileExtension().toString();
      IPath targetPath = new Path(targetPathStr+"_rendered.dot");

      PetriNet net = OcletIO.readNetFromFile(fileName);
      if (selectedFile_system.getFileExtension().equals("oclets")) {
        OcletSpecification os = new OcletSpecification(net);
        ActionHelper.writeFile(targetPath, OcletIO_Out.toDot(os));
        
      } else {
        ActionHelper.writeFile(targetPath, net.toDot());
      }
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName, e);
    }
	}

  public void selectionChanged(IAction action, ISelection selection) {

    selectedFile_system = null;

    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false)
    {
      return;
    }
    
    try {
      Object[] selected = ((IStructuredSelection) selection).toArray();
      if (selected.length == 1) {
      
        String[] sysExt = new String[] { "oclets", "lola", "tpn" };
        LinkedList<IFile> systemFiles = ActionHelper.filterFiles(selected, sysExt);
        
        if (systemFiles.size() == 1) {
          selectedFile_system = systemFiles.getFirst();
          action.setEnabled(true);
        }
      }
        
    } catch (ClassCastException e) {
      // just catch, do nothing
    } catch (NullPointerException e) {
      // just catch, do nothing
    }
  }
}
