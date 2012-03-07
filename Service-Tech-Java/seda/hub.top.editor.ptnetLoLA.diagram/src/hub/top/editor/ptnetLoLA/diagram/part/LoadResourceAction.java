/*
 *  Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 *  
 *  ServiceTechnolog.org - PetriNet Editor Framework
 *  
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License v1.0, which accompanies this
 *  distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 * 
 *  The Original Code is this file as it was released on July 30, 2008.
 *  The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 *  Portions created by the Initial Developer are Copyright (c) 2008
 *  the Initial Developer. All Rights Reserved.
 * 
 *  Contributor(s):
 * 
 *  Alternatively, the contents of this file may be used under the terms of
 *  either the GNU General Public License Version 3 or later (the "GPL"), or
 *  the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 *  in which case the provisions of the GPL or the LGPL are applicable instead
 *  of those above. If you wish to allow use of your version of this file only
 *  under the terms of either the GPL or the LGPL, and not to allow others to
 *  use your version of this file under the terms of the EPL, indicate your
 *  decision by deleting the provisions above and replace them with the notice
 *  and other provisions required by the GPL or the LGPL. If you do not delete
 *  the provisions above, a recipient may use your version of this file under
 *  the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.editor.ptnetLoLA.diagram.part;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.diagram.ui.parts.DiagramEditor;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.handlers.HandlerUtil;

/**
 * @generated
 */
public class LoadResourceAction extends AbstractHandler {
  /**
   * @generated
   */
  public Object execute(ExecutionEvent event) throws ExecutionException {
    IEditorPart diagramEditor = HandlerUtil.getActiveEditorChecked(event);
    Shell shell = diagramEditor.getEditorSite().getShell();
    assert diagramEditor instanceof DiagramEditor;
    TransactionalEditingDomain editingDomain = ((DiagramEditor) diagramEditor)
        .getEditingDomain();
    org.eclipse.emf.edit.ui.action.LoadResourceAction.LoadResourceDialog loadResourceDialog = new org.eclipse.emf.edit.ui.action.LoadResourceAction.LoadResourceDialog(
        shell, editingDomain);
    loadResourceDialog.open();
    return null;
  }

}
