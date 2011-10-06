/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on March 08, 2009.
 * The Initial Developer of the Original Code is
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

package hub.top.editor.eclipse.ui;

import hub.top.editor.resource.Activator;

import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.common.util.WrappedException;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;


/**
 * Action class to invoke a model conversion wizard. The wizard is
 * implemented in {@link ConvertFileWizard}. To use this action, create a
 * corresponding handler in plugin.xml
 * 
 * <code>
 *    <extension point="org.eclipse.ui.popupMenus">
 *      <objectContribution
 *          id="my.id" nameFilter="*.ext"
 *          objectClass="org.eclipse.core.resources.IFile">
 *        <action
 *             label="Convert Model..."
 *             class="my.package.ConvertDiagramFileAction"
 *             menubarPath="additions"
 *             enablesFor="1"
 *             id="my.action.id">
 *        </action>
 *      </objectContribution>
 *    </extension>
 *  </code>
 * 
 * @author Dirk Fahland
 */
public class ConvertDiagramFileAction extends ConvertFileAction {

  @Override
  protected Object getModelObject(TransactionalEditingDomain editingDomain, URI uri) {
    ResourceSet resourceSet = editingDomain.getResourceSet();
    EObject diagramRoot = null;
    try {
      Resource resource = resourceSet.getResource(uri, true);
      diagramRoot = (EObject) resource.getContents().get(0);
    } catch (WrappedException ex) {
      Activator.getPluginHelper().logError(
          "Unable to load resource: " + uri, ex);
    }
    return diagramRoot;
  }
}
