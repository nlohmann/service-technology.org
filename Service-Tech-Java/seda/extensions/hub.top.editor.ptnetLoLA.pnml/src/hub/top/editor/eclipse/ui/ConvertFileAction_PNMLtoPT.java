/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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

package hub.top.editor.eclipse.ui;

import hub.top.editor.ptnetlola.pnml.ReadPNMLfile;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class ConvertFileAction_PNMLtoPT  extends ConvertFileAction {

  /**
   * Create a {@link ConvertFileWizard_PNMLtoPT} to convert a selected
   * Flowchart diagram to a PNML document.
   * 
   * @see hub.top.editor.eclipse.ui.ConvertDiagramFileAction#createConvertFileWizard(org.eclipse.emf.common.util.URI, org.eclipse.emf.ecore.EObject, org.eclipse.emf.transaction.TransactionalEditingDomain)
   */
  @Override
  public ConvertFileWizard createConvertFileWizard(URI srcURI, Object root,
      TransactionalEditingDomain editingDomain)
  {
    ConvertFileWizard wizard = new ConvertFileWizard_PNMLtoPT(
        srcURI, root, editingDomain, "ptnet");
    wizard.setWindowTitle("Convert PNML to P/T net");
    return wizard;
  }

  @Override
  protected Object getModelObject(TransactionalEditingDomain editingDomain,
      URI uri) {
    IPath path = new Path(uri.toPlatformString(false));
    IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(path);
    String pathString = file.getLocation().toString();
    return ReadPNMLfile.readPNMLFile(pathString);
  }
}
