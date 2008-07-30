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

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

/**
 * An operation to write an Eclipse resource file into the workspace.
 */
public class NewModelFileOperation extends WorkspaceModifyOperation {

	/** the file where the contents shall be written */
	private ResourceHelper resHelper;
	private boolean resOverwrite;
	
	/**
	 * an operation to write an Eclipse resource file into the
	 * workspace
	 * @param ResourceHelper helper that describes the
	 * resource and its contents to be created
	 * @param overwrite whether to overwrite an existing resource
	 * with new contents, or whether to extend the contents of
	 * the resource
	 */
	public NewModelFileOperation(ResourceHelper helper, boolean overwrite) {
		resHelper = helper;
		resOverwrite = overwrite;
	}
	
	/**
	 * execute the operation by creating the file, its resource and
	 * writing the contents to this file
	 * 
	 * @see org.eclipse.ui.actions.WorkspaceModifyOperation#execute(org.eclipse.core.runtime.IProgressMonitor)
	 */
	@Override
	protected void execute(IProgressMonitor monitor) throws CoreException,
			InvocationTargetException, InterruptedException {
		try {
			if (resOverwrite)
				resHelper.createNewModelFile();	// write resource to disk
			else
				resHelper.updateModelFile(true, resOverwrite);
		}
		catch (Exception exception) {
			resHelper.getPluginHelper().logError(exception);
		}
		finally {
			monitor.done();
		}
	}
}
