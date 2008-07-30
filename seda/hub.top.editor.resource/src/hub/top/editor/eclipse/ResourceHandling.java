/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * 		Dirk Fahland
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
package hub.top.editor.eclipse;


import java.io.FileNotFoundException;
import java.lang.reflect.InvocationTargetException;
import java.util.UnknownFormatConversionException;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.ui.IWorkbenchWindow;

/**
 * provides utility methods to access and manipulate Eclipse
 * workbench files and resources regarding petri net models
 *  
 * @author Dirk Fahland
 *
 */
public class ResourceHandling {

	
	private static IPath createTargetPath(IPath sourcePath, String sourceExt, String targetExt) {
		if (sourcePath.getFileExtension().equals(sourceExt))
			return sourcePath.removeFileExtension().addFileExtension(targetExt);
		else
			return null;
	}
	
	/**
	 * Create a path to the target file based on the current input
	 * of a file editor. Replaces the file extension with the
	 * correct model file extension . 
	 * @param currentInput  current input of the file editor from which
	 *						the new path is constructed
	 * @param sourceExt feasible extension (that the currentInput must have)
	 * @param targetExt extension of the target file
	 * @return target path if this could be constructed (<code>null</code> otherwise)
	 */
	public static IPath createTargetPath(ResourceHelper sourceHelper, String targetExt)
	{
		IPath targetPath = null;
		
		try {
			// resolve input to resource in the workspace
			IFile currentFile = sourceHelper.getFile(true);
			if (currentFile == null)
				throw new FileNotFoundException("Convert to "+targetExt+" failed: no input file given");
			
			targetPath = createTargetPath(currentFile.getFullPath(), sourceHelper.getFeasibleExtension(), targetExt);
			if (targetPath == null)
				throw new UnknownFormatConversionException("Failed to convert "+currentFile+". Wrong file format.");
			
		} catch (FileNotFoundException e) {
			sourceHelper.getPluginHelper().logError(e.getMessage(), e);
			return null;
		} catch (UnknownFormatConversionException e) {
			sourceHelper.getPluginHelper().logError(e.getMessage(), e);
			return null;
		}
		
		return targetPath;
	}
	
	public static void createModelResourceFromResource(IWorkbenchWindow window, ResourceHelper sourceHelper, ResourceHelper targetHelper) 
	{
		// resolve target file name from current editor input
		IPath modelFilePath = createTargetPath(sourceHelper, targetHelper.getFeasibleExtension());
		if (modelFilePath == null)
			return;

		// create a file object and retrieve the contents
		// for the new model file
		EObject netObject = (EObject) sourceHelper.getModelRoot();

		targetHelper.setPath(modelFilePath, false);
		targetHelper.addToModelContents(netObject);

		// create the operation to write the new model file
		NewModelFileOperation op = targetHelper.storeModelContentsInResource(true);
		try {
			window.run(true, false, op);	
		} catch (InvocationTargetException e) {
			targetHelper.getPluginHelper().logError(new Error("Unable to store model in "+targetHelper.getURI(true)+". Could not invoke workspace operation: "+e.getCause(), e));
		} catch (InterruptedException ex) {
			targetHelper.getPluginHelper().logError(new Error("Storing resource "+targetHelper.getURI(true)+" has been interrupted by "+ex.getCause(), ex));
		}
	}

	public static void createDiagramResourceFromResource(IWorkbenchWindow window, ResourceHelper sourceHelper, ResourceHelper targetModelHelper, DiagramEditorHelper targetDiagramHelper)
	{
		// prepare target model helper that is needed for the diagram
		// unless the source is already the correct target model helper
		if (sourceHelper != targetModelHelper) {
			// resolve target file name from current editor input
			IPath modelFilePath = createTargetPath(sourceHelper, targetModelHelper.getFeasibleExtension());
			targetModelHelper.setPath(modelFilePath, false);
	
			EObject netObject = (EObject) sourceHelper.getModelRoot();
			targetModelHelper.addToModelContents(netObject);
		}

		// ----------------- generate diagram-file ----------------------
		// create file handle
		IPath diagramFilePath = createTargetPath(targetModelHelper, targetDiagramHelper.getFeasibleExtension());
		targetDiagramHelper.setPath(diagramFilePath, false);

		try {
			NewDiagramFileOperation diagramOp = targetDiagramHelper.createDiagramResource();
			//window.run(true, false, diagramOp);
			OperationHistoryFactory.getOperationHistory().execute(diagramOp,
					new NullProgressMonitor(), null);
			
			// diagram helper has initialized an empty model: restore contents from targetModelHelper
			if (!targetDiagramHelper.diagramInSync()) {
				UpdateDiagramFileOperation updateOp = targetDiagramHelper.updateDiagramContentsInResource(true);
				//window.run(true, false, updateOp);
				OperationHistoryFactory.getOperationHistory().execute(updateOp,
						new NullProgressMonitor(), null);
			}

		} /*catch (InvocationTargetException ex) {
			targetDiagramHelper.getPluginHelper().logError("Unable to complete diagram creation. Could not invoke method: "+ex.getCause(), ex);
		} catch (InterruptedException ex) {
			targetDiagramHelper.getPluginHelper().logError("Creation of the diagram has been interrupted by "+ex.getCause(), ex);
		}*/ catch (ExecutionException ex) {
			targetDiagramHelper.getPluginHelper().logError("Unable to create model and diagram", ex);
		}
	}
}
