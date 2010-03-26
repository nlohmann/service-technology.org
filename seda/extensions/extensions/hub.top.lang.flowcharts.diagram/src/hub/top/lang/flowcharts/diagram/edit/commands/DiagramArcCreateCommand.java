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

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.FlowFactory;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.policies.FlowchartBaseItemSemanticEditPolicy;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.type.core.commands.CreateElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.ConfigureRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;

/**
 * @generated
 */
public class DiagramArcCreateCommand extends CreateElementCommand {

  /**
   * @generated
   */
  private final EObject source;

  /**
   * @generated
   */
  private final EObject target;

  /**
   * @generated
   */
  private WorkflowDiagram container;

  /**
   * @generated
   */
  public DiagramArcCreateCommand(CreateRelationshipRequest request,
      EObject source, EObject target) {
    super(request);
    this.source = source;
    this.target = target;
    if (request.getContainmentFeature() == null) {
      setContainmentFeature(FlowPackage.eINSTANCE
          .getWorkflowDiagram_DiagramArcs());
    }

    // Find container element for the new link.
    // Climb up by containment hierarchy starting from the source
    // and return the first element that is instance of the container class.
    for (EObject element = source; element != null; element = element
        .eContainer()) {
      if (element instanceof WorkflowDiagram) {
        container = (WorkflowDiagram) element;
        super.setElementToEdit(container);
        break;
      }
    }
  }

  /**
   * @generated
   */
  public boolean canExecute() {
    if (source == null && target == null) {
      return false;
    }
    if (source != null && false == source instanceof DiagramNode) {
      return false;
    }
    if (target != null && false == target instanceof DiagramNode) {
      return false;
    }
    if (getSource() == null) {
      return true; // link creation is in progress; source is not defined yet
    }
    // target may be null here but it's possible to check constraint
    if (getContainer() == null) {
      return false;
    }
    return FlowchartBaseItemSemanticEditPolicy.LinkConstraints
        .canCreateDiagramArc_3001(getContainer(), getSource(), getTarget());
  }

  /**
   * @generated
   */
  protected EObject doDefaultElementCreation() {
    DiagramArc newElement = FlowFactory.eINSTANCE.createDiagramArc();
    getContainer().getDiagramArcs().add(newElement);
    newElement.setSrc(getSource());
    newElement.setTarget(getTarget());
    return newElement;
  }

  /**
   * @generated
   */
  protected EClass getEClassToEdit() {
    return FlowPackage.eINSTANCE.getWorkflowDiagram();
  }

  /**
   * @generated
   */
  protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
      IAdaptable info) throws ExecutionException {
    if (!canExecute()) {
      throw new ExecutionException("Invalid arguments in create link command"); //$NON-NLS-1$
    }
    return super.doExecuteWithResult(monitor, info);
  }

  /**
   * @generated
   */
  protected ConfigureRequest createConfigureRequest() {
    ConfigureRequest request = super.createConfigureRequest();
    request.setParameter(CreateRelationshipRequest.SOURCE, getSource());
    request.setParameter(CreateRelationshipRequest.TARGET, getTarget());
    return request;
  }

  /**
   * @generated
   */
  protected void setElementToEdit(EObject element) {
    throw new UnsupportedOperationException();
  }

  /**
   * @generated
   */
  protected DiagramNode getSource() {
    return (DiagramNode) source;
  }

  /**
   * @generated
   */
  protected DiagramNode getTarget() {
    return (DiagramNode) target;
  }

  /**
   * @generated
   */
  public WorkflowDiagram getContainer() {
    return container;
  }
}
