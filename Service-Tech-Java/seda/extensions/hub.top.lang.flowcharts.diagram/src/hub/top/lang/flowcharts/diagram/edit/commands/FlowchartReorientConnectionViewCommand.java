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
package hub.top.lang.flowcharts.diagram.edit.commands;

import java.util.List;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartReorientConnectionViewCommand extends
    AbstractTransactionalCommand {

  /**
   * @generated
   */
  private IAdaptable edgeAdaptor;

  /**
   * @generated
   */
  public FlowchartReorientConnectionViewCommand(
      TransactionalEditingDomain editingDomain, String label) {
    super(editingDomain, label, null);
  }

  /**
   * @generated
   */
  public List getAffectedFiles() {
    View view = (View) edgeAdaptor.getAdapter(View.class);
    if (view != null) {
      return getWorkspaceFiles(view);
    }
    return super.getAffectedFiles();
  }

  /**
   * @generated
   */
  public IAdaptable getEdgeAdaptor() {
    return edgeAdaptor;
  }

  /**
   * @generated
   */
  public void setEdgeAdaptor(IAdaptable edgeAdaptor) {
    this.edgeAdaptor = edgeAdaptor;
  }

  /**
   * @generated
   */
  protected CommandResult doExecuteWithResult(IProgressMonitor progressMonitor,
      IAdaptable info) {
    assert null != edgeAdaptor : "Null child in FlowchartReorientConnectionViewCommand"; //$NON-NLS-1$
    Edge edge = (Edge) getEdgeAdaptor().getAdapter(Edge.class);
    assert null != edge : "Null edge in FlowchartReorientConnectionViewCommand"; //$NON-NLS-1$
    View tempView = edge.getSource();
    edge.setSource(edge.getTarget());
    edge.setTarget(tempView);
    return CommandResult.newOKCommandResult();
  }
}
