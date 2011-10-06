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

package hub.top.seda.rcp;

import java.net.URL;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.application.IWorkbenchConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchAdvisor;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.internal.ide.IDEInternalWorkbenchImages;
import org.eclipse.ui.internal.ide.IDEWorkbenchPlugin;
import org.eclipse.ui.internal.ide.model.WorkbenchAdapterBuilder;
import org.osgi.framework.Bundle;

@SuppressWarnings("restriction")
public class ApplicationWorkbenchAdvisor extends WorkbenchAdvisor {

	private static final String PERSPECTIVE_ID = "hub.top.seda.rcp.perspective";

    public WorkbenchWindowAdvisor createWorkbenchWindowAdvisor(IWorkbenchWindowConfigurer configurer) {
        return new ApplicationWorkbenchWindowAdvisor(configurer);
    }
    
    /**
     * this application runs a workspace, get the workspace of this RCP
     */
    @Override
	public IAdaptable getDefaultPageInput() {
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		return workspace.getRoot();
	}

  /**
   * initialize the workbench
   */
	@Override
  public void initialize(IWorkbenchConfigurer configurer) {
    	super.initialize(configurer);
    	
    	// make project explorer available, project explorer
    	// will be rendered in view "hub.top.seda.rcp.view.explorer"
    	// (see plugin.xml)
		WorkbenchAdapterBuilder.registerAdapters();

		// register images for rendering explorer view
		final String ICONS_PATH = "icons/full/";
		final String PATH_OBJECT = ICONS_PATH + "obj16/";
		Bundle ideBundle = Platform.getBundle(IDEWorkbenchPlugin.IDE_WORKBENCH);
		declareWorkbenchImage(configurer, ideBundle,
				IDE.SharedImages.IMG_OBJ_PROJECT, PATH_OBJECT + "prj_obj.gif",
				true);
		declareWorkbenchImage(configurer, ideBundle,
				IDE.SharedImages.IMG_OBJ_PROJECT_CLOSED, PATH_OBJECT
						+ "cprj_obj.gif", true);
		
		// register images for rendering problem view
		declareWorkbenchImage(configurer, ideBundle,
				IDEInternalWorkbenchImages.IMG_ETOOL_PROBLEM_CATEGORY,
				ICONS_PATH+"elcl16/collapseall.gif",
				true );
		declareWorkbenchImage(configurer, ideBundle,
				IDEInternalWorkbenchImages.IMG_OBJS_ERROR_PATH,
				PATH_OBJECT+"error_tsk.gif",
				true );
		declareWorkbenchImage(configurer, ideBundle,
				IDEInternalWorkbenchImages.IMG_OBJS_WARNING_PATH,
				PATH_OBJECT+"warn_tsk.gif",
				true );
		declareWorkbenchImage(configurer, ideBundle,
				IDEInternalWorkbenchImages.IMG_OBJS_INFO_PATH,
				PATH_OBJECT+"info_tsk.gif",
				true );
		
		
		configurer.setSaveAndRestore(true);	// remember last view settings of user
  }
    
    /**
     * TODO method comment
     * @param configurer_p
     * @param ideBundle
     * @param symbolicName
     * @param path
     * @param shared
     */
	private void declareWorkbenchImage(IWorkbenchConfigurer configurer_p,
			Bundle ideBundle, String symbolicName, String path, boolean shared) {
		URL url = ideBundle.getEntry(path);
		ImageDescriptor desc = ImageDescriptor.createFromURL(url);
		configurer_p.declareImage(symbolicName, desc, shared);
	}

	/**
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#preShutdown()
	 */
	@Override
		public boolean preShutdown() {
			getWorkbenchConfigurer().getWorkbench().saveAllEditors(true);
			try {
				ResourcesPlugin.getWorkspace().save(true, new NullProgressMonitor());
			} catch (CoreException e) {
				Activator.getDefault().getLog().log(
						new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Error during workspace shutdown.", e));
			}
			return super.preShutdown();
		}

	public String getInitialWindowPerspectiveId() {
		return PERSPECTIVE_ID;
	}
}
