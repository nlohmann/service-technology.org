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
package hub.top.lang.flowcharts.diagram.edit.policies;

import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.helpers.FlowchartBaseEditHelper;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import java.util.Collections;
import java.util.Iterator;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.requests.ReconnectRequest;
import org.eclipse.gmf.runtime.common.core.command.ICommand;
import org.eclipse.gmf.runtime.diagram.core.commands.DeleteCommand;
import org.eclipse.gmf.runtime.diagram.core.util.ViewUtil;
import org.eclipse.gmf.runtime.diagram.ui.commands.CommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.SemanticEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.requests.EditCommandRequestWrapper;
import org.eclipse.gmf.runtime.emf.commands.core.command.CompositeTransactionalCommand;
import org.eclipse.gmf.runtime.emf.type.core.ElementTypeRegistry;
import org.eclipse.gmf.runtime.emf.type.core.IEditHelperContext;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.type.core.requests.ConfigureRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyReferenceRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DuplicateElementsRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.GetEditContextRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.IEditCommandRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.MoveRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientReferenceRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.SetRequest;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartBaseItemSemanticEditPolicy extends SemanticEditPolicy {

  /**
   * Extended request data key to hold editpart visual id.
   * 
   * @generated
   */
  public static final String VISUAL_ID_KEY = "visual_id"; //$NON-NLS-1$

  /**
   * Extended request data key to hold editpart visual id.
   * Add visual id of edited editpart to extended data of the request
   * so command switch can decide what kind of diagram element is being edited.
   * It is done in those cases when it's not possible to deduce diagram
   * element kind from domain element.
   * 
   * @generated
   */
  public Command getCommand(Request request) {
    if (request instanceof ReconnectRequest) {
      Object view = ((ReconnectRequest) request).getConnectionEditPart()
          .getModel();
      if (view instanceof View) {
        Integer id = new Integer(FlowchartVisualIDRegistry
            .getVisualID((View) view));
        request.getExtendedData().put(VISUAL_ID_KEY, id);
      }
    }
    return super.getCommand(request);
  }

  /**
   * Returns visual id from request parameters.
   * 
   * @generated
   */
  protected int getVisualID(IEditCommandRequest request) {
    Object id = request.getParameter(VISUAL_ID_KEY);
    return id instanceof Integer ? ((Integer) id).intValue() : -1;
  }

  /**
   * @generated
   */
  protected Command getSemanticCommand(IEditCommandRequest request) {
    IEditCommandRequest completedRequest = completeRequest(request);
    Object editHelperContext = completedRequest.getEditHelperContext();
    if (editHelperContext instanceof View
        || (editHelperContext instanceof IEditHelperContext && ((IEditHelperContext) editHelperContext)
            .getEObject() instanceof View)) {
      // no semantic commands are provided for pure design elements
      return null;
    }
    if (editHelperContext == null) {
      editHelperContext = ViewUtil.resolveSemanticElement((View) getHost()
          .getModel());
    }
    IElementType elementType = ElementTypeRegistry.getInstance()
        .getElementType(editHelperContext);
    if (elementType == ElementTypeRegistry.getInstance().getType(
        "org.eclipse.gmf.runtime.emf.type.core.default")) { //$NON-NLS-1$ 
      elementType = null;
    }
    Command semanticCommand = getSemanticCommandSwitch(completedRequest);
    if (elementType != null) {
      if (semanticCommand != null) {
        ICommand command = semanticCommand instanceof ICommandProxy ? ((ICommandProxy) semanticCommand)
            .getICommand()
            : new CommandProxy(semanticCommand);
        completedRequest.setParameter(
            FlowchartBaseEditHelper.EDIT_POLICY_COMMAND, command);
      }
      ICommand command = elementType.getEditCommand(completedRequest);
      if (command != null) {
        if (!(command instanceof CompositeTransactionalCommand)) {
          TransactionalEditingDomain editingDomain = ((IGraphicalEditPart) getHost())
              .getEditingDomain();
          command = new CompositeTransactionalCommand(editingDomain, command
              .getLabel()).compose(command);
        }
        semanticCommand = new ICommandProxy(command);
      }
    }
    boolean shouldProceed = true;
    if (completedRequest instanceof DestroyRequest) {
      shouldProceed = shouldProceed((DestroyRequest) completedRequest);
    }
    if (shouldProceed) {
      if (completedRequest instanceof DestroyRequest) {
        TransactionalEditingDomain editingDomain = ((IGraphicalEditPart) getHost())
            .getEditingDomain();
        Command deleteViewCommand = new ICommandProxy(new DeleteCommand(
            editingDomain, (View) getHost().getModel()));
        semanticCommand = semanticCommand == null ? deleteViewCommand
            : semanticCommand.chain(deleteViewCommand);
      }
      return semanticCommand;
    }
    return null;
  }

  /**
   * @generated
   */
  protected Command getSemanticCommandSwitch(IEditCommandRequest req) {
    if (req instanceof CreateRelationshipRequest) {
      return getCreateRelationshipCommand((CreateRelationshipRequest) req);
    } else if (req instanceof CreateElementRequest) {
      return getCreateCommand((CreateElementRequest) req);
    } else if (req instanceof ConfigureRequest) {
      return getConfigureCommand((ConfigureRequest) req);
    } else if (req instanceof DestroyElementRequest) {
      return getDestroyElementCommand((DestroyElementRequest) req);
    } else if (req instanceof DestroyReferenceRequest) {
      return getDestroyReferenceCommand((DestroyReferenceRequest) req);
    } else if (req instanceof DuplicateElementsRequest) {
      return getDuplicateCommand((DuplicateElementsRequest) req);
    } else if (req instanceof GetEditContextRequest) {
      return getEditContextCommand((GetEditContextRequest) req);
    } else if (req instanceof MoveRequest) {
      return getMoveCommand((MoveRequest) req);
    } else if (req instanceof ReorientReferenceRelationshipRequest) {
      return getReorientReferenceRelationshipCommand((ReorientReferenceRelationshipRequest) req);
    } else if (req instanceof ReorientRelationshipRequest) {
      return getReorientRelationshipCommand((ReorientRelationshipRequest) req);
    } else if (req instanceof SetRequest) {
      return getSetCommand((SetRequest) req);
    }
    return null;
  }

  /**
   * @generated
   */
  protected Command getConfigureCommand(ConfigureRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getCreateRelationshipCommand(CreateRelationshipRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getCreateCommand(CreateElementRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getSetCommand(SetRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getEditContextCommand(GetEditContextRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getDestroyElementCommand(DestroyElementRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getDestroyReferenceCommand(DestroyReferenceRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getDuplicateCommand(DuplicateElementsRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getMoveCommand(MoveRequest req) {
    return null;
  }

  /**
   * @generated
   */
  protected Command getReorientReferenceRelationshipCommand(
      ReorientReferenceRelationshipRequest req) {
    return UnexecutableCommand.INSTANCE;
  }

  /**
   * @generated
   */
  protected Command getReorientRelationshipCommand(
      ReorientRelationshipRequest req) {
    return UnexecutableCommand.INSTANCE;
  }

  /**
   * @generated
   */
  protected final Command getGEFWrapper(ICommand cmd) {
    return new ICommandProxy(cmd);
  }

  /**
   * @deprecated use getGEFWrapper() instead
   * @generated
   */
  protected final Command getMSLWrapper(ICommand cmd) {
    // XXX deprecated: use getGEFWrapper() instead
    return getGEFWrapper(cmd);
  }

  /**
   * @generated
   */
  protected EObject getSemanticElement() {
    return ViewUtil.resolveSemanticElement((View) getHost().getModel());
  }

  /**
   * Returns editing domain from the host edit part.
   * 
   * @generated
   */
  protected TransactionalEditingDomain getEditingDomain() {
    return ((IGraphicalEditPart) getHost()).getEditingDomain();
  }

  /**
   * Creates command to destroy the link.
   * 
   * @generated
   */
  protected Command getDestroyElementCommand(View view) {
    EditPart editPart = (EditPart) getHost().getViewer().getEditPartRegistry()
        .get(view);
    DestroyElementRequest request = new DestroyElementRequest(
        getEditingDomain(), false);
    return editPart.getCommand(new EditCommandRequestWrapper(request,
        Collections.EMPTY_MAP));
  }

  /**
   * Creates commands to destroy all host incoming and outgoing links.
   * 
   * @generated
   */
  protected CompoundCommand getDestroyEdgesCommand() {
    CompoundCommand cmd = new CompoundCommand();
    View view = (View) getHost().getModel();
    for (Iterator it = view.getSourceEdges().iterator(); it.hasNext();) {
      cmd.add(getDestroyElementCommand((Edge) it.next()));
    }
    for (Iterator it = view.getTargetEdges().iterator(); it.hasNext();) {
      cmd.add(getDestroyElementCommand((Edge) it.next()));
    }
    return cmd;
  }

  /**
   * @generated
   */
  protected void addDestroyShortcutsCommand(CompoundCommand command) {
    View view = (View) getHost().getModel();
    if (view.getEAnnotation("Shortcut") != null) { //$NON-NLS-1$
      return;
    }
    for (Iterator it = view.getDiagram().getChildren().iterator(); it.hasNext();) {
      View nextView = (View) it.next();
      if (nextView.getEAnnotation("Shortcut") == null || !nextView.isSetElement() || nextView.getElement() != view.getElement()) { //$NON-NLS-1$
        continue;
      }
      command.add(getDestroyElementCommand(nextView));
    }
  }

  /**
   * @generated
   */
  public static class LinkConstraints {

    /**
     * @generated
     */
    public static boolean canCreateDiagramArc_4001(WorkflowDiagram container,
        DiagramNode source, DiagramNode target) {
      return canExistDiagramArc_4001(container, source, target);
    }

    /**
     * @generated
     */
    public static boolean canExistDiagramArc_4001(WorkflowDiagram container,
        DiagramNode source, DiagramNode target) {

      return true;
    }
  }

}
