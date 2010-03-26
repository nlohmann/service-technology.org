/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
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
 */
package hub.top.lang.flowcharts.diagram.part;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;

/**
 * @generated
 */
public class FlowchartCreationWizardPage extends WizardNewFileCreationPage {

  /**
   * @generated
   */
  private final String fileExtension;

  /**
   * @generated
   */
  public FlowchartCreationWizardPage(String pageName,
      IStructuredSelection selection, String fileExtension) {
    super(pageName, selection);
    this.fileExtension = fileExtension;
  }

  /**
   * Override to create files with this extension.
   * 
   * @generated
   */
  protected String getExtension() {
    return fileExtension;
  }

  /**
   * @generated
   */
  public URI getURI() {
    return URI.createPlatformResourceURI(getFilePath().toString(), false);
  }

  /**
   * @generated
   */
  protected IPath getFilePath() {
    IPath path = getContainerFullPath();
    if (path == null) {
      path = new Path(""); //$NON-NLS-1$
    }
    String fileName = getFileName();
    if (fileName != null) {
      path = path.append(fileName);
    }
    return path;
  }

  /**
   * @generated
   */
  public void createControl(Composite parent) {
    super.createControl(parent);
    setFileName(FlowchartDiagramEditorUtil.getUniqueFileName(
        getContainerFullPath(), getFileName(), getExtension()));
    setPageComplete(validatePage());
  }

  /**
   * @generated
   */
  protected boolean validatePage() {
    if (!super.validatePage()) {
      return false;
    }
    String extension = getExtension();
    if (extension != null
        && !getFilePath().toString().endsWith("." + extension)) {
      setErrorMessage(NLS.bind(
          Messages.FlowchartCreationWizardPageExtensionError, extension));
      return false;
    }
    return true;
  }
}
