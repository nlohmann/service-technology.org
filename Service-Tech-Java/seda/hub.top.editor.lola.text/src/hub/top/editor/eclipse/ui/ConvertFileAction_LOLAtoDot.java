/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010 Dirk Fahland. EPL1.0/GPL3.0/LGPL3.0
 * All rights reserved.
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *    Dirk Fahland
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
\*****************************************************************************/

package hub.top.editor.eclipse.ui;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.lola.text.Activator;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;

import java.io.IOException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class ConvertFileAction_LOLAtoDot implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.editor.lola.text.ConvertToDot";
	
	private IWorkbenchWindow workbenchWindow;
	
	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile = null;
	private URI 	selectedURI = null;
	
	public void dispose() {
		// TODO Auto-generated method stub

	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
		
		if (selectedFile == null)
		  return;
		
		String fileName = selectedFile.getRawLocation().toOSString();
		
		try {
      PetriNet net = PetriNetIO.readNetFromFile(fileName);
      writeDotFile(net, selectedFile, "_rendered");
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName, e);
    }
	}

	public void selectionChanged(IAction action, ISelection selection) {
	    selectedURI = null;
	    selectedFile = null;
	    action.setEnabled(false);
	    if (selection instanceof IStructuredSelection == false
	        || selection.isEmpty()) {
	      return;
	    }
	    try {
	      Object o = ((IStructuredSelection) selection).getFirstElement();
	      if (o instanceof IFile) {
	        selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
	        selectedURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
	            .toString(), true);
	        
	      } else {
	        return;
	      }
	      
		    action.setEnabled(true);
	    } catch (ClassCastException e) {
	    	// just catch, do nothing
	    } catch (NullPointerException e) {
	      // just catch, do nothing
	    }
	}

	private void writeDotFile (PetriNet net, IFile inputFile, String suffix) {

		String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
		IPath targetPath = new Path(targetPathStr+suffix+".dot");

		FileIOHelper.writeFile (targetPath, net.toDot());
	}
}
