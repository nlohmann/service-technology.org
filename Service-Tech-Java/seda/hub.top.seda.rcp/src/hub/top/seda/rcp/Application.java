/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - RCP Application 
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
 * The Original Code is this file as it was released on April 06, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Emil Crumhorn
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

package hub.top.seda.rcp;

import java.io.IOException;
import java.net.URL;

import hub.top.seda.rcp.ui.PickWorkspaceDialog;

import org.eclipse.core.runtime.Platform;
import org.eclipse.equinox.app.IApplication;
import org.eclipse.equinox.app.IApplicationContext;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.osgi.service.datalocation.Location;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;

/**
 * This class controls all aspects of the application's execution
 */
public class Application implements IApplication {

	public static final String PLUGIN_ID = "hub.top.seda.rcp";

	/* (non-Javadoc)
	 * @see org.eclipse.equinox.app.IApplication#start(org.eclipse.equinox.app.IApplicationContext)
	 */
	public Object start(IApplicationContext context) throws Exception {
		Display display = PlatformUI.createDisplay();
		try {
			// check the location of the workspace, display user dialog for choosing
			// a workspace location if necessary
			int checkCode = checkWorkSpaceLocation(display);
			if (checkCode != 0)
				return checkCode;
			
			int returnCode = PlatformUI.createAndRunWorkbench(display, new ApplicationWorkbenchAdvisor());
			if (returnCode == PlatformUI.RETURN_RESTART)
				return IApplication.EXIT_RESTART;
			else
				return IApplication.EXIT_OK;
		} finally {
			display.dispose();
		}
		
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.equinox.app.IApplication#stop()
	 */
	public void stop() {
		final IWorkbench workbench = PlatformUI.getWorkbench();
		if (workbench == null)
			return;
		final Display display = workbench.getDisplay();
		display.syncExec(new Runnable() {
			public void run() {
				if (!display.isDisposed())
					workbench.close();
			}
		});
	}
	
	private Integer checkWorkSpaceLocation (Display display) throws IOException {

		// fetch the Location that we will be modifying
		Location instanceLoc = Platform.getInstanceLocation();
		
		if (instanceLoc.isSet()) {
			// we are running within Eclipse PDE: workspace location is already set
			//MessageDialog.openInformation(display.getActiveShell(), "Workspace already set.", "The workspace is already set to "+instanceLoc.getURL());
			return 0;
		}

		
		// get what the user last said about remembering the workspace location
		boolean remember = PickWorkspaceDialog.isRememberWorkspace();

		// get the last used workspace location
		String lastUsedWs = PickWorkspaceDialog.getLastSetWorkspaceDirectory();
		
		// see if we just changed workspace: then temporarily remember the change
		// and do not display the dialog
		if (!remember && PickWorkspaceDialog.updateJustRestartedWorkspace()) {
			remember = true;
		}

		// if we have a "remember" but no last used workspace, it's not much to remember
		if (remember && (lastUsedWs == null || lastUsedWs.length() == 0)) {
		    remember = false;
		}

		// check to ensure the workspace location is still OK
		if (remember) {
		    // if there's any problem whatsoever with the workspace, force a dialog which in its turn will tell them what's bad
		    String ret = PickWorkspaceDialog.checkWorkspaceDirectory(Display.getDefault().getActiveShell(), lastUsedWs, false, false);
		    if (ret != null) {
		    	remember = false;
		    }

		}

		// if we don't remember the workspace, show the dialog
		if (!remember) {
		    PickWorkspaceDialog pwd = new PickWorkspaceDialog(false, null);
		    int pick = pwd.open();

		    // if the user canceled, we can't do anything as we need a workspace, so in this case, we tell them and exit
		    if (pick == Window.CANCEL) {
				if (pwd.getSelectedWorkspaceLocation()  == null) {
				    MessageDialog.openError(display.getActiveShell(), "Error",
					    "Seda cannot start without a workspace root and will now exit.");
				    try {
				    	PlatformUI.getWorkbench().close();
				    } catch (Exception err) {
	
				    }
				    System.exit(0);
				    return IApplication.EXIT_OK;
				}
		    }
		    else {
				try {
					// tell Eclipse what the selected location was and continue
					instanceLoc.set(new URL("file", null, pwd.getSelectedWorkspaceLocation()), false);
				} catch (IllegalStateException e) {
					MessageDialog.openError(display.getActiveShell(), "Error", "The workspace location was already set to "+instanceLoc.getURL());
				}
		    }
		}
		else {
			try {
			    // set the last used location and continue
			    instanceLoc.set(new URL("file", null, lastUsedWs), false);
			} catch (IllegalStateException e) {
				MessageDialog.openError(display.getActiveShell(), "Error", "The workspace location was already set to "+instanceLoc.getURL());
			}
		}
		
		return 0;
	}
}
