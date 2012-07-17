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

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class PluginHelper {
	
	private final AbstractUIPlugin fHostPlugin;
	private final String fHostPluginID;
	
	public PluginHelper (AbstractUIPlugin hostPlugin, String hostPluginID) {
		fHostPlugin = hostPlugin;
		fHostPluginID = hostPluginID;
	}

	/**
	 * log an error within the host plugin of this helper
	 * @param error message to be logged
	 */
	public void logError(String error) {
		logError(error, null);
	}

	/**
	 * log an error within the host plugin of this helper
	 * @param throwable exception that caused the error
	 */
	public void logError(Throwable throwable) {
		logError(throwable.getMessage(), throwable);
	}
	
	/**
	 * log an error within the host plugin of this helper
	 * @param error message to be logged
	 * @param throwable exception causing the error
	 */
	public void logError(String error, Throwable throwable) {
		if (error == null && throwable != null) {
			error = throwable.getMessage();
		}
		fHostPlugin.getLog().log(new Status(
				IStatus.ERROR, fHostPluginID, IStatus.OK, error, throwable)
			);
	}
	
	 /**
   * log a warning within the host plugin of this helper
   * @param warning message to be logged
   */
  public void logWarning(String warning) {
    logWarning(warning, null);
  }
  
  /**
  * log a warning within the host plugin of this helper
  * @param warning message to be logged
  * @param throwable exception causing the warning
  */
 public void logWarning(String warning, Throwable throwable) {
   fHostPlugin.getLog().log(new Status(
       IStatus.WARNING, fHostPluginID, IStatus.OK, warning, throwable)
     );
 }
  
  /**
   * Show error dialog with the given title and the error message and log the error
   * in the error view.
   * 
   * @param title
   * @param error
   * @param cause
   */
  public void showErrorToUser(final String title, final String error, final Throwable cause) {
    Display.getDefault().syncExec( new ErrorRunnable(this, title, error, cause) );
  }
  
  /**
   * Show error dialog with the given title and the error message and log the error
   * in the error view.
   * 
   * @param title
   * @param error
   * @param cause
   */
  public void showInfoToUser(final String title, final String info) {
    Display.getDefault().syncExec( new InfoRunnable(this, title, info) );
  }
  
  /**
   * Runnable class that displays an error dialog and logs the error to the Eclipse error log.
   * 
   * @author dfahland
   */
  public static class ErrorRunnable implements Runnable {
    
    private final String title;
    private final String error;
    private final Throwable cause;
    private final PluginHelper helper;
    
    public ErrorRunnable(final PluginHelper helper, final String title, final String error, final Throwable cause) {
      this.title = title;
      this.error = error;
      this.cause = cause;
      this.helper = helper;
    }
    
    public void run() {
      MessageDialog.openError(null, title, error+"\nSee error log for details.");
      if (cause != null) helper.logError(error, cause);  
    };
  }
  
  /**
   * Runnable class that displays an error dialog and logs the error to the Eclipse error log.
   * 
   * @author dfahland
   */
  public static class InfoRunnable implements Runnable {
    
    private final String title;
    private final String info;
    
    public InfoRunnable(final PluginHelper helper, final String title, final String info) {
      this.title = title;
      this.info = info;
    }
    
    public void run() {
      MessageDialog.openInformation(null, title, info);
    };
  }
}
